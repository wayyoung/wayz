/*
 * Zephyr RTOS application using SerialTransfer protocol with ZCBOR encoding
 * Uses Zephyr's native CRC-8 function only
 * Sends voltage, current, and temperature readings every 100ms
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/crc.h>
#include <zcbor_common.h>
#include <zcbor_encode.h>
#include <string.h>
#include <stdlib.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_data_uart)
#define MAX_PACKET_SIZE 254
#define START_BYTE 0x7E
#define STOP_BYTE 0x81

/* CRC-8 polynomial used by Zephyr (0x07) */
#define CRC8_POLYNOMIAL 0x07
#define CRC8_INITIAL 0x00

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Sensor data structure */
struct sensor_data {
    int voltage;
    int current;
    int temperature;
};

/* COBS (Consistent Overhead Byte Stuffing) encoding */
static size_t cobs_encode(const uint8_t *src, size_t len, uint8_t *dst)
{
    const uint8_t *end = src + len;
    uint8_t *encode = dst;
    uint8_t *code_ptr = encode++;
    uint8_t code = 1;

    while (src < end) {
        if (*src == 0) {
            *code_ptr = code;
            code_ptr = encode++;
            code = 1;
        } else {
            *encode++ = *src;
            code++;
            if (code == 0xFF) {
                *code_ptr = code;
                code_ptr = encode++;
                code = 1;
            }
        }
        src++;
    }
    *code_ptr = code;
    
    return encode - dst;
}

extern int actualv, actuali;
/* Simulated sensor reading functions */
static int read_v(void)
{
    return actualv;
    // /* Simulate voltage reading between 11.5V and 14.5V */
    // static float voltage = 12.0f;
    // voltage += ((k_cycle_get_32() % 200) - 100) / 100.0f;
    // if (voltage < 11.5f) voltage = 11.5f;
    // if (voltage > 14.5f) voltage = 14.5f;
    // return voltage;
}

static int read_i(void)
{
    // /* Simulate current reading between 0.5A and 5.5A */
    // static float current = 3.0f;
    // current += ((k_cycle_get_32() % 200) - 100) / 100.0f;
    // if (current < 0.5f) current = 0.5f;
    // if (current > 5.5f) current = 5.5f;
    // return current;
    return actuali;
}

static int read_t(void)
{
    // /* Simulate temperature reading between 20.0°C and 35.0°C */
    // static float temp = 25.0f;
    // temp += ((k_cycle_get_32() % 100) - 50) / 50.0f;
    // if (temp < 20.0f) temp = 20.0f;
    // if (temp > 35.0f) temp = 35.0f;
    // return temp;
    return 0xFFFFF;
}

/* Encode sensor data using ZCBOR */
static int encode_sensor_data(struct sensor_data *data, uint8_t *buffer, 
                              size_t buffer_size, size_t *encoded_len)
{
    ZCBOR_STATE_E(states, 3, buffer, buffer_size, 1);
    
    bool success = zcbor_map_start_encode(states, 3) &&
                   zcbor_tstr_put_lit(states, "voltage") &&
                   zcbor_int32_put(states, data->voltage) &&
                   zcbor_tstr_put_lit(states, "current") &&
                   zcbor_int32_put(states, data->current) &&
                   zcbor_tstr_put_lit(states, "temperature") &&
                   zcbor_int32_put(states, data->temperature) &&
                   zcbor_map_end_encode(states, 3);
    
    if (!success) {
        return -1;
    }
    
    *encoded_len = states->payload - buffer;
    return 0;
}

/* Alternative simple encoding without ZCBOR (fallback) */
static int encode_sensor_data_simple(struct sensor_data *data, uint8_t *buffer, 
                                     size_t buffer_size, size_t *encoded_len)
{
    if (buffer_size < sizeof(struct sensor_data)) {
        return -1;
    }
    
    memcpy(buffer, &data->voltage, sizeof(int));
    memcpy(buffer + sizeof(int), &data->current, sizeof(int));
    memcpy(buffer + 2 * sizeof(int), &data->temperature, sizeof(int));
    
    *encoded_len = 3 * sizeof(int);
    return 0;
}

/* Send packet using SerialTransfer protocol with Zephyr CRC-8 */
static void send_packet(uint8_t packet_id, uint8_t *payload, size_t payload_len)
{
    uint8_t packet[MAX_PACKET_SIZE + 10];
    uint8_t cobs_buffer[MAX_PACKET_SIZE + 10];
    size_t packet_idx = 0;
    
    /* Build packet without delimiters */
    packet[packet_idx++] = packet_id;
    packet[packet_idx++] = (uint8_t)payload_len;
    
    /* Add payload */
    memcpy(&packet[packet_idx], payload, payload_len);
    packet_idx += payload_len;
    
    /* Calculate and add CRC-8 using Zephyr's native function */
    uint8_t crc = crc8(packet, packet_idx, CRC8_POLYNOMIAL, CRC8_INITIAL, false);
    packet[packet_idx++] = crc;
    
    /* COBS encode the packet */
    size_t cobs_len = cobs_encode(packet, packet_idx, cobs_buffer);
    
    /* Send the complete frame */
    uart_poll_out(uart_dev, START_BYTE);
    for (size_t i = 0; i < cobs_len; i++) {
        uart_poll_out(uart_dev, cobs_buffer[i]);
    }
    uart_poll_out(uart_dev, STOP_BYTE);
}



static bool data_uart_enabled = false;

void set_data_uart_enabled(bool enable)
{
    data_uart_enabled = enable;
}



void data_uart_task(void)
{
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }
    
    uint8_t packet_id = 0;
    uint32_t send_count = 0;
    
    while (1) {
        struct sensor_data data;
        uint8_t encode_buffer[128];
        size_t encoded_len;
        int ret;

        if(!data_uart_enabled) {
            goto SLEEP;
        }
        
        /* Read sensor values */
        data.voltage = read_v();
        data.current = read_i();
        data.temperature = read_t();
        
        /* Try to encode with ZCBOR */
        ret = encode_sensor_data(&data, encode_buffer, sizeof(encode_buffer), &encoded_len);
        
        /* If ZCBOR encoding fails, fall back to simple encoding */
        if (ret != 0) {
            printk("ZCBOR encoding failed, using simple encoding\n");
            ret = encode_sensor_data_simple(&data, encode_buffer, sizeof(encode_buffer), &encoded_len);
        }
        
        if (ret == 0) {
            /* Send packet using SerialTransfer protocol */
            send_packet(packet_id++, encode_buffer, encoded_len);
            send_count++;
            
            /* Print status every 10 packets */
            if (send_count % 10 == 0) {
                printk("[%d] V=%d, I=%d, T=%d, P=%d\n", 
                       send_count, data.voltage, data.current, data.temperature,
                       data.voltage * data.current);
            }
            
            /* Wrap packet ID at 255 */
            if (packet_id == 0) {
                printk("Packet ID wrapped around\n");
            }
        } 
        else {
            printk("Encoding failed\n");
        }
SLEEP:
        /* Wait 100ms */
        k_msleep(100);
    }
}
