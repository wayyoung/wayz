/*
 * Zephyr RTOS application using SerialTransfer protocol with ZCBOR encoding
 * Uses Zephyr's native CRC-8 function only
 * Sends voltage, current, and temperature readings every 100ms
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/arch/cpu.h>

#include <zcbor_common.h>
#include <zcbor_encode.h>
#include <zcbor_decode.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <zephyr/shell/shell.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>


#define UART_DEVICE_NODE DT_CHOSEN(zephyr_data_uart)
#define MAX_PACKET_SIZE 114
static const uint8_t SYNC_BYTE = 0x00;

#define ENABLE_CRC 1

#if ENABLE_CRC
#include <zephyr/sys/crc.h>
/* CRC-8 polynomial used by Zephyr (0x07) */
#define CRC8_POLYNOMIAL 0x07
#define CRC8_INITIAL 0x00
#endif

static void process_command(uint8_t *buf, size_t len);

static uint8_t tx_buf[MAX_PACKET_SIZE + 12];
static uint8_t packet_buf[MAX_PACKET_SIZE + 10];
static uint8_t rx_buf[MAX_PACKET_SIZE];
static size_t rx_buf_len = 0;

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static void send_response_work_handler(struct k_work *work);

static struct k_work send_response_work;

static struct k_work_q response_work_q;
static K_THREAD_STACK_DEFINE(response_stack_area, 1024);
static uint16_t response_crid;

static void uart_irq_callback(const struct device *dev, void *user_data)
{
	uart_irq_update(dev);
	if (uart_irq_rx_ready(dev)) {
		uint8_t byte;
		static int64_t last_byte_timestamp;

		if (rx_buf_len > 0 && (k_uptime_get() - last_byte_timestamp > 100)) {
			/* Timeout, clear buffer */
			rx_buf_len = 0;
		}

		while (uart_fifo_read(dev, &byte, 1) > 0) {
			last_byte_timestamp = k_uptime_get();
			if (byte == SYNC_BYTE) {
				if (rx_buf_len > 0) {
					process_command(rx_buf, rx_buf_len);
				}
				rx_buf_len = 0; /* Reset for next packet */
			} else {
				if (rx_buf_len < sizeof(rx_buf)) {
					rx_buf[rx_buf_len++] = byte;
				} else {
					/* Buffer overflow, discard and reset */
					rx_buf_len = 0;
				}
			}
		}
	}
}

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

static size_t cobs_decode(const uint8_t *src, size_t len, uint8_t *dst)
{
    const uint8_t *end = src + len;
    uint8_t *decode = dst;
    while (src < end) {
        int code = *src++;
        for (int i = 1; i < code; i++) {
            *decode++ = *src++;
        }
        if (code < 0xFF && src < end) {
            *decode++ = 0;
        }
    }
    return decode - dst;
}

extern int actualv, actuali;
/* Simulated sensor reading functions */
static int read_v(void)
{
    // /* Simulate voltage reading between 11.5V and 14.5V */
    // static float voltage = 12.0f;
    // voltage += ((k_cycle_get_32() % 200) - 100) / 100.0f;
    // if (voltage < 11.5f) voltage = 11.5f;
    // if (voltage > 14.5f) voltage = 14.5f;
    // return voltage;
    return actualv;
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
static int encode_sensor_data(struct sensor_data *data, uint16_t crid, uint8_t *buffer,
                              size_t buffer_size, size_t *encoded_len)
{
    ZCBOR_STATE_E(states, 4, buffer, buffer_size, 1);

    bool success = zcbor_map_start_encode(states, 4) &&
                   zcbor_tstr_put_lit(states, "volt") &&
                   zcbor_int32_put(states, data->voltage) &&
                   zcbor_tstr_put_lit(states, "curr") &&
                   zcbor_int32_put(states, data->current) &&
                   zcbor_tstr_put_lit(states, "temp") &&
                   zcbor_int32_put(states, data->temperature) &&
                   zcbor_tstr_put_lit(states, "crid") &&
                   zcbor_uint32_put(states, crid) &&
                   zcbor_map_end_encode(states, 4);

    if (!success) {
        return -1;
    }

    *encoded_len = states->payload - buffer;
    return 0;
}


/* Send packet using SerialTransfer protocol with Zephyr CRC-8 */
static void send_packet(uint8_t *packet, uint8_t packet_len)
{
	int tx_len = 0;

	memset(tx_buf, 0, sizeof(tx_buf));
	/* COBS encode the packet */
	size_t cobs_len = cobs_encode(packet, packet_len, &tx_buf[tx_len]);
	tx_len += cobs_len;
	tx_buf[tx_len++] = SYNC_BYTE;

	for (int i = 0; i < tx_len; i++) {
		uart_poll_out(uart_dev, tx_buf[i]);
	}
}



static atomic_t packet_id = ATOMIC_INIT(0);

static void send_sensor_data_response(uint16_t crid)
{
    struct sensor_data data;
    size_t encoded_len;
    int ret;

    atomic_val_t current_id = atomic_inc(&packet_id);
    if (current_id == 255) {
        atomic_set(&packet_id, 1);
    }

    memset(packet_buf, 0, sizeof(packet_buf));

    data.voltage = read_v();
    data.current = read_i();
    data.temperature = read_t();

    ret = encode_sensor_data(&data, crid, &packet_buf[2], MAX_PACKET_SIZE, &encoded_len);

    if (ret != 0) {
        printk("ZCBOR failed! ret=%d\n", ret);
    } else {
        packet_buf[0] = (uint8_t)current_id;
        packet_buf[1] = (uint8_t)encoded_len;
#if ENABLE_CRC
        uint8_t crc = crc8(packet_buf, encoded_len + 2, CRC8_POLYNOMIAL, CRC8_INITIAL, false);
        packet_buf[encoded_len + 2] = crc;
#endif
        send_packet(packet_buf, encoded_len + 3);
    }
}


static void send_response_work_handler(struct k_work *work)
{
	send_sensor_data_response(response_crid);
}

typedef struct {
    uint32_t crid;
    uint32_t sample_rate;
    bool enabled;
    char pwr[10];
    uint32_t baud;
    char mux[10];
} command_payload_t;

typedef void (*command_handler_t)(command_payload_t *payload);

typedef struct {
    const char *name;
    command_handler_t handler;
} command_t;

static void handle_getd(command_payload_t *payload)
{
    if (!k_work_is_pending(&send_response_work)) {
        response_crid = (uint16_t)payload->crid;
        k_work_submit_to_queue(&response_work_q, &send_response_work);
    }
}

static void handle_ping(command_payload_t *payload)
{
    size_t encoded_len;
    ZCBOR_STATE_E(states, 4, &packet_buf[2], MAX_PACKET_SIZE, 0);

    bool ok = zcbor_map_start_encode(states, 1) &&
              zcbor_tstr_put_lit(states, "pong") &&
              zcbor_bool_put(states, true) &&
              zcbor_map_end_encode(states, 1);

    if (ok) {
        encoded_len = states->payload - &packet_buf[2];
        atomic_val_t current_id = atomic_inc(&packet_id);
        if (current_id == 255) {
            atomic_set(&packet_id, 1);
        }
        packet_buf[0] = (uint8_t)current_id;
        packet_buf[1] = (uint8_t)encoded_len;
#if ENABLE_CRC
        uint8_t crc = crc8(packet_buf, encoded_len + 2, CRC8_POLYNOMIAL, CRC8_INITIAL, false);
        packet_buf[encoded_len + 2] = crc;
#endif
        send_packet(packet_buf, encoded_len + 3);
    }
}


static const command_t commands[] = {
    {"getd", handle_getd},
    {"ping", handle_ping},
};

static void process_command(uint8_t *buf, size_t len)
{
	uint8_t decoded_buf[MAX_PACKET_SIZE];
	size_t decoded_len = cobs_decode(buf, len, decoded_buf);

	if (decoded_len == 0) {
		return;
	}

	ZCBOR_STATE_D(states, 4, decoded_buf, decoded_len, 15, 0);
	char cmd_str[5] = {0};
	command_payload_t payload = {0};
	bool cmd_found = false;

	if (!zcbor_map_start_decode(states)) {
		printk("Failed to start map decode\n");
		return;
	}

	while (true) {
		struct zcbor_string key;
		if (!zcbor_tstr_decode(states, &key)) {
			break;
		}

		if (strncmp(key.value, "cmd", key.len) == 0) {
			struct zcbor_string val;
			if (!zcbor_tstr_decode(states, &val) || val.len >= sizeof(cmd_str)) {
				return;
			}
			memcpy(cmd_str, val.value, val.len);
			cmd_str[val.len] = '\0';
			cmd_found = true;
		} else if (strncmp(key.value, "id", key.len) == 0) {
			zcbor_uint32_decode(states, &payload.crid);
		} else if (strncmp(key.value, "pwr", key.len) == 0) {
			struct zcbor_string pwr_str;
			if (zcbor_tstr_decode(states, &pwr_str) && pwr_str.len < sizeof(payload.pwr)) {
				memcpy(payload.pwr, pwr_str.value, pwr_str.len);
				payload.pwr[pwr_str.len] = '\0';
			}
		} else {
			zcbor_any_skip(states, NULL);
		}
	}

	if (!zcbor_map_end_decode(states)) {
		printk("Failed to decode command map\n");
		return;
	}

	if (!cmd_found) {
		return;
	}

	for (int i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(cmd_str, commands[i].name) == 0) {
			commands[i].handler(&payload);
			return;
		}
	}
}

void mgr_init(void)
{
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }

    k_work_queue_start(&response_work_q, response_stack_area,
		       K_THREAD_STACK_SIZEOF(response_stack_area), 5, NULL);

    k_work_init(&send_response_work, send_response_work_handler);
    uart_irq_callback_user_data_set(uart_dev, uart_irq_callback, NULL);
    uart_irq_rx_enable(uart_dev);
}
