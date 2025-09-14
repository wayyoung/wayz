/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <soc.h>
#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/clock_control_n32.h>
#include <zephyr/drivers/interrupt_controller/exti_n32.h>
#include <zephyr/drivers/gpio/gpio_n32.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/sys_io.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(gpio_n32, CONFIG_GPIO_LOG_LEVEL);

#define DT_DRV_COMPAT nsing_n32_gpio

struct gpio_n32_config {
	uint32_t base;
	uint32_t clkid;
};

struct gpio_n32_data {
	struct gpio_driver_data common;
	sys_slist_t callbacks;
};

#define DEV_CFG(dev)  ((const struct gpio_n32_config *)(dev)->config)
#define DEV_DATA(dev) ((struct gpio_n32_data *)(dev)->data)

static inline void gpio_n32_isr(uint8_t line, const struct device *dev)
{
	struct gpio_n32_data *data = dev->data;

	gpio_fire_callbacks(&data->callbacks, dev, BIT(line));
}

static int gpio_n32_init(const struct device *dev)
{
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));
	int ret;

	ret = clock_control_on(clk, (clock_control_subsys_t *)&cfg->clkid);

	return ret;
}

static int port2portsource(GPIO_Module *module)
{
	if (module == GPIOA) {
		return GPIOA_PORT_SOURCE;
	} else if (module == GPIOB) {
		return GPIOB_PORT_SOURCE;
	} else if (module == GPIOC) {
		return GPIOC_PORT_SOURCE;
	} else if (module == GPIOD) {
		return GPIOD_PORT_SOURCE;	
	} else {
		return GPIOA_PORT_SOURCE;
	}
}

static char gpioa_exti_settings[]={
	AFIO_EXTI_PA0,
	AFIO_EXTI_PA1,
	AFIO_EXTI_PA2,
	AFIO_EXTI_PA3,
	AFIO_EXTI_PA4,
	AFIO_EXTI_PA5,
	AFIO_EXTI_PA6,
	AFIO_EXTI_PA7,
	AFIO_EXTI_PA8,
	AFIO_EXTI_PA9,
	AFIO_EXTI_PA10,
	AFIO_EXTI_PA11,
	AFIO_EXTI_PA12,
	AFIO_EXTI_PA13,
	AFIO_EXTI_PA14,
	AFIO_EXTI_PA15,
};

static int gpio_n32_config_exti_line(GPIO_Module *gpio, gpio_pin_t pin)
{
#ifdef CONFIG_SOC_SERIES_N32G430
	int s = port2portsource(gpio);
	if (GPIOA_PORT_SOURCE == s && pin >= 0 && pin <=15)
	{
		char pin_exti = gpioa_exti_settings[pin];
		GPIO_EXTI_Line_Set(pin, pin_exti);
	}
	else
	{
		return -ENOTSUP;
	}
#else
	GPIO_ConfigEXTILine(port2portsource(gpio), pin);
#endif
	return 0;
}

#define _GPIO_MODE_INPUT        0
#define _GPIO_MODE_OUTPUT_2MHZ  1
#define _GPIO_MODE_OUTPUT_10MHZ 2
#define _GPIO_MODE_OUTPUT_50MHZ 3

#define _GPIO_CFG_INPUT_AN    (0 << 4)
#define _GPIO_CFG_INPUT_FLOAT (1 << 4)
#define _GPIO_CFG_INPUT_PD_PU (2 << 4)

#define _GPIO_CFG_OUTPUT_OD (1 << 4)

static int gpio_n32_pin_configure(const struct device *dev, gpio_pin_t pin, gpio_flags_t flags)
{
	GPIO_InitType init_structure;
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;

	GPIO_Structure_Initialize(&init_structure);

	if ((flags & GPIO_OUTPUT) && (flags & GPIO_INPUT)) {
		return -ENOTSUP;
	}

	if ((flags & GPIO_OUTPUT_INIT_HIGH)) {
		gpio->PBSC |= BIT(pin);
	} else if ((flags & GPIO_OUTPUT_INIT_LOW)) {
		gpio->PBC |= BIT(pin);
	}

	if (flags & GPIO_OUTPUT) {
		init_structure.GPIO_Mode =
			(flags & GPIO_OPEN_DRAIN) ? GPIO_MODE_OUT_OD : GPIO_MODE_OUT_PP;
	} else if (flags & GPIO_INPUT) {
		if (flags & (GPIO_PULL_UP | GPIO_PULL_DOWN)) {
			init_structure.GPIO_Mode = GPIO_MODE_ANALOG;
			init_structure.GPIO_Pull =
				(flags & GPIO_PULL_UP) ? GPIO_PS_PULL_UP : GPIO_PS_PULL_DOWN;
		} else {
			init_structure.GPIO_Mode = GPIO_MODE_INPUT;
		}
	}

	init_structure.Pin = 1 << pin;

	GPIO_Peripheral_Initialize(gpio, &init_structure);


	return 0;
}

static int gpio_n32_port_get_raw(const struct device *dev, gpio_port_value_t *value)
{
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;

	*value = gpio->PID;

	return 0;
}

/* API implementation: port_set_masked_raw */
static int gpio_n32_port_set_masked_raw(const struct device *dev, gpio_port_pins_t mask,
					gpio_port_value_t value)
{
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;

	gpio->POD = (gpio->POD & ~mask) | (value & mask);

	return 0;
}

static int gpio_n32_port_set_bits_raw(const struct device *dev, gpio_port_pins_t mask)
{
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;

	gpio->PBSC |= mask;

	return 0;
}

static int gpio_n32_port_clear_bits_raw(const struct device *dev, gpio_port_pins_t mask)
{
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;

	mask <<= 16;

	gpio->PBSC |= mask;

	return 0;
}

static int gpio_n32_port_toggle_bits(const struct device *dev, gpio_port_pins_t mask)
{
	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;

	gpio->POD = (gpio->POD ^ mask);

	return 0;
}

static int gpio_n32_pin_interrupt_configure(const struct device *dev, gpio_pin_t pin,
					    enum gpio_int_mode mode, enum gpio_int_trig trig)
{
	int rc = 0;

	const struct gpio_n32_config *cfg = DEV_CFG(dev);
	GPIO_Module *gpio = (GPIO_Module *)cfg->base;



	switch (mode) {
	case GPIO_INT_MODE_DISABLED: {
		exti_n32_disable(pin);
		(void)exti_n32_configure(pin, NULL, NULL);
		exti_n32_trigger(pin, 0);
	} break;
	case GPIO_INT_MODE_EDGE: {
		rc = exti_n32_configure(pin, gpio_n32_isr, dev);
		if (rc < 0) {
			return rc;
		}

		gpio_n32_config_exti_line(gpio, pin);

		switch (trig) {
		case GPIO_INT_TRIG_LOW:
			exti_n32_trigger(pin, EXTI_Trigger_Falling);
			break;
		case GPIO_INT_TRIG_HIGH:
			exti_n32_trigger(pin, EXTI_Trigger_Rising);
			break;
		case GPIO_INT_TRIG_BOTH:
			exti_n32_trigger(pin, EXTI_Trigger_Rising_Falling);
			break;
		default:
			return -ENOTSUP;
		}

		exti_n32_enable(pin);
	} break;

	default:
		rc = -ENOTSUP;
		break;
	}

	return rc;
}

static int gpio_n32_manage_callback(const struct device *dev, struct gpio_callback *callback,
				    bool set)
{
	struct gpio_n32_data *data = dev->data;

	return gpio_manage_callback(&data->callbacks, callback, set);
}

void gpio_n32_force_intr(const struct device *dev, gpio_pin_t pin)
{
	exti_n32_force(BIT(pin));
}

static const struct gpio_driver_api gpio_n32_driver_api = {
	.pin_configure = gpio_n32_pin_configure,
	.port_get_raw = gpio_n32_port_get_raw,
	.port_set_masked_raw = gpio_n32_port_set_masked_raw,
	.port_set_bits_raw = gpio_n32_port_set_bits_raw,
	.port_clear_bits_raw = gpio_n32_port_clear_bits_raw,
	.port_toggle_bits = gpio_n32_port_toggle_bits,
	.pin_interrupt_configure = gpio_n32_pin_interrupt_configure,
	.manage_callback = gpio_n32_manage_callback};

#define GPIO_N32_INIT(n)                                                                           \
	static const struct gpio_n32_config gpio_n32_config_##n = {                                \
		.base = DT_INST_REG_ADDR(n),                                                       \
		.clkid = DT_INST_CLOCKS_CELL(n, id),                                               \
	};                                                                                         \
	static struct gpio_n32_data gpio_n32_data_##n;                                             \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(n, gpio_n32_init, NULL, &gpio_n32_data_##n, &gpio_n32_config_##n,    \
			      PRE_KERNEL_1, CONFIG_GPIO_INIT_PRIORITY, &gpio_n32_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_N32_INIT)

static int afio_n32_init(void)
{
	const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));

	return clock_control_on(clk, (clock_control_subsys_t *)N32_CLOCK_AFIO);
}

SYS_INIT(afio_n32_init, PRE_KERNEL_1, CONFIG_GPIO_INIT_PRIORITY);