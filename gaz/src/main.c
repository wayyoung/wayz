/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/dt-bindings/sensor/ina230.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include <zephyr/display/cfb.h>
#include "cfb_font_dice.h"


#if defined(CONFIG_LOG)
#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(main);
#define INFO  LOG_INF
#define ERROR LOG_ERR
#else
#define INFO  printk
#define ERROR printk
#endif

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100

const struct device *const ina237_dev = DEVICE_DT_GET(DT_NODELABEL(ina237));
static struct device *ina_dev = (struct device *)ina237_dev;

int actualv = 0, actuali = 0, actualp = 0, vshunt = 0;
bool ina_on = true;


static void gather_ina_input()
{
	struct sensor_value sensor_val;
	if (ina_on) {
		int ret = sensor_sample_fetch(ina_dev);
		if (ret < 0) {
			ERROR("error fetching sensor input!! %d", ret);
		}

		sensor_channel_get(ina_dev, SENSOR_CHAN_VOLTAGE, &sensor_val);
		actualv = (sensor_value_to_double(&sensor_val) * 1000);
		sensor_channel_get(ina_dev, SENSOR_CHAN_CURRENT, &sensor_val);
		actuali = (sensor_value_to_double(&sensor_val) * 1000);
		sensor_channel_get(ina_dev, SENSOR_CHAN_POWER, &sensor_val);
		actualp = (sensor_value_to_double(&sensor_val) * 1000);
		sensor_channel_get(ina_dev, SENSOR_CHAN_VSHUNT, &sensor_val);
		vshunt = (sensor_value_to_double(&sensor_val) * 1000000);
	}
}

static void ina_enable(bool on)
{
	ina_on = on;

	if (!ina_on) {
		actualv = 0;
		actuali = 0;
		actualp = 0;
		vshunt = 0;
	}
}

static int cmd_read_input(const struct shell *sh, size_t argc, char **argv)
{
	if (argc > 1) {
		if (!strcasecmp("off", argv[1])) {
			ina_enable(false);
		} else if (!strcasecmp("on", argv[1])) {
			ina_enable(true);
		}
	}

	shell_print(sh, "Shunt voltage: 0 => %d uV", vshunt);
	shell_print(sh, "Bus voltage  : 0 => %d mV", actualv);
	shell_print(sh, "Power        : 0 => %d mW", actualp);
	shell_print(sh, "Current      : 0 => %d mA", actuali);
	shell_print(sh, "On           : 0 => %d", ina_on);

	return 0;
}

SHELL_CMD_ARG_REGISTER(ina, NULL, "Read the input voltate and current on port H", cmd_read_input, 0, 0);

extern void mgr_init(void);
int main(void)
{
	// int ret;
	// bool led_state = true;

	// if (!gpio_is_ready_dt(&led)) {
	// 	return 0;
	// }

	// ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	// if (ret < 0) {
	// 	return 0;
	// }
	mgr_init();

	while (1) {
		gather_ina_input();
	// 	ret = gpio_pin_toggle_dt(&led);
	// 	if (ret < 0) {
	// 		return 0;
	// 	}

	// 	led_state = !led_state;
	// 	// printf("LED state: %s\n", led_state ? "ON" : " OFF");
		k_sleep(K_MSEC(SLEEP_TIME_MS));
	}
	return 0;
}



const struct device *const display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
void configure_cfb(void)
{

	int err;

	if (!device_is_ready(display)) {
		ERROR("Display device not ready\n");
		return;
	}

	if (display_set_pixel_format(display, PIXEL_FORMAT_MONO10) != 0) {
		if (display_set_pixel_format(display, PIXEL_FORMAT_MONO01) != 0) {
			ERROR("Failed to set required pixel format");
			return;
		}
	}

	if (display_blanking_off(display) != 0) {
		ERROR("Failed to turn off display blanking\n");
		return;
	}

	err = cfb_framebuffer_init(display);
	if (err) {
		ERROR("Could not initialize framebuffer (err %d)\n", err);
	}

	cfb_framebuffer_invert(display);
	err = cfb_framebuffer_clear(display, true);
	if (err) {
		ERROR("Could not clear framebuffer (err %d)\n", err);
	}

	err = cfb_print(display, "A**__**A", 0, 0);
	if (err) {
		ERROR("Could not display custom font (err %d)\n", err);
	}

	err = cfb_framebuffer_finalize(display);
	if (err) {
		ERROR("Could not finalize framebuffer (err %d)\n", err);
	}
	return;
}


/* size of stack area used by each thread */
#define STACKSIZE 512

/* scheduling priority used by each thread */
#define PRIORITY 7

#define TEXT_H 10

void cfb_task(void)
{
	configure_cfb();
	char sbuf[10] = {0};
	k_sleep(K_MSEC(3000));


	while (1) {
		int text_y = 0;
		int err = 0;
		err = cfb_framebuffer_clear(display, true);
		if (err) {
			ERROR("Could not clear framebuffer (err %d)\n", err);
			goto SLEEP;
		}

		memset(sbuf, 0, sizeof(sbuf));
		if (ina_on) {
			snprintf(sbuf, sizeof(sbuf) - 1, "%d mV", actualv);
		} else {
			snprintf(sbuf, sizeof(sbuf) - 1, "-- mV");
		}
		err = cfb_print(display, sbuf, 0, text_y);
		text_y += TEXT_H;
		if (err) {
			ERROR("Could not display custom font 1(err %d)\n", err);
			goto SLEEP;
		}

		memset(sbuf, 0, sizeof(sbuf));
		if (ina_on) {
			snprintf(sbuf, sizeof(sbuf) - 1, "%d mA", actuali);
		} else {
			snprintf(sbuf, sizeof(sbuf) - 1, "-- mA");
		}
		err = cfb_print(display, sbuf, 0, text_y);
		text_y += TEXT_H;

		if (err) {
			ERROR("Could not display custom font 1(err %d)\n", err);
			goto SLEEP;
		}

		err = cfb_framebuffer_finalize(display);
		if (err) {
			ERROR("Could not finalize framebuffer (err %d)\n", err);
		}
	SLEEP:
		k_sleep(K_MSEC(500));
	}
	return;
}

K_THREAD_DEFINE(cfb_task_id, STACKSIZE, cfb_task, NULL, NULL, NULL, PRIORITY, 0, 0);
