/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _I2C_N32_H
#define _I2C_N32_H

#include <zephyr/device.h>

#ifdef CONFIG_SOC_SERIES_N32G430
#include <n32g430_i2c.h>
#include <stm32f1xx_ll_i2c.h>
#endif

#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end)))) 

#define I2C_MASTER_ADDR   0x30

/* I2C clock frequency, MHz */
#define I2CCLK_MAX                    ((uint32_t)0x00000054U)                  /*!< i2cclk maximum value */
#define I2CCLK_MIN                    ((uint32_t)0x00000002U)                  /*!< i2cclk minimum value for standard mode */


enum i2c_n32_mode {
	I2CN32MODE_I2C,
	I2CN32MODE_SMBUSHOST,
	I2CN32MODE_SMBUSDEVICE,
	I2CN32MODE_SMBUSDEVICEARP,
};

void i2c_n32_set_smbus_mode(const struct device *dev, enum i2c_n32_mode mode);

#ifdef CONFIG_SMBUS_N32_SMBALERT
typedef void (*i2c_n32_smbalert_cb_func_t)(const struct device *dev);

void i2c_n32_smbalert_set_callback(const struct device *dev, i2c_n32_smbalert_cb_func_t func,
				     const struct device *cb_dev);
void i2c_n32_smbalert_enable(const struct device *dev);
void i2c_n32_smbalert_disable(const struct device *dev);
#endif /* CONFIG_SMBUS_N32_SMBALERT */



#ifdef CONFIG_I2C_N32_BUS_RECOVERY
#include <zephyr/drivers/gpio.h>
#endif /* CONFIG_I2C_N32_BUS_RECOVERY */

typedef void (*irq_config_func_t)(const struct device *port);


struct i2c_n32_config {
#ifdef CONFIG_I2C_N32_INTERRUPT
	irq_config_func_t irq_config_func;
#endif
#ifdef CONFIG_I2C_N32_BUS_RECOVERY
	struct gpio_dt_spec scl;
	struct gpio_dt_spec sda;
#endif /* CONFIG_I2C_N32_BUS_RECOVERY */
	uint16_t clkid;
	I2C_TypeDef *i2c;
	uint32_t bitrate;
	const struct pinctrl_dev_config *pcfg;
};

struct i2c_n32_data {
#ifdef CONFIG_I2C_N32_INTERRUPT
	struct k_sem device_sync_sem;
#endif
	struct k_sem bus_mutex;
	uint32_t dev_config;
	uint16_t slave_address;
	struct {
		unsigned int is_restart;
		unsigned int flags;
		unsigned int is_write;
		unsigned int is_arlo;
		unsigned int is_nack;
		unsigned int is_err;
		struct i2c_msg *msg;
		unsigned int len;
		uint8_t *buf;
	} current;
#ifdef CONFIG_I2C_TARGET
	bool master_active;
	struct i2c_target_config *slave_cfg;
	struct i2c_target_config *slave2_cfg;
	bool slave_attached;
#endif
	bool is_configured;
	bool smbalert_active;
	enum i2c_n32_mode mode;
#ifdef CONFIG_SMBUS_N32_SMBALERT
	i2c_n32_smbalert_cb_func_t smbalert_cb_func;
	const struct device *smbalert_cb_dev;
#endif
};

int32_t n32_i2c_transaction(const struct device *dev,
			    struct i2c_msg msg, uint8_t *next_msg_flags,
			    uint16_t periph);
int32_t n32_i2c_configure_timing(const struct device *dev, uint32_t clk);
int i2c_n32_runtime_configure(const struct device *dev, uint32_t config);
int i2c_n32_get_config(const struct device *dev, uint32_t *config);

void n32_i2c_event_isr(void *arg);
void n32_i2c_error_isr(void *arg);

#ifdef CONFIG_I2C_TARGET
int i2c_n32_target_register(const struct device *dev, struct i2c_target_config *config);
int i2c_n32_target_unregister(const struct device *dev, struct i2c_target_config *config);
#endif

#endif
