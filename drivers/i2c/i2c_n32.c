/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/clock_control_n32.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/pm/policy.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/reset.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2c/i2c_n32.h>
#include <zephyr/drivers/gpio.h>

#ifdef CONFIG_I2C_N32_BUS_RECOVERY
#include "i2c_bitbang.h"
#endif /* CONFIG_I2C_N32_BUS_RECOVERY */

#include <zephyr/logging/log.h>
#include <zephyr/irq.h>
LOG_MODULE_REGISTER(i2c_n32, CONFIG_I2C_LOG_LEVEL);

#include "i2c-priv.h"

#define DT_DRV_COMPAT nsing_n32_i2c

#include <stm32f1xx_ll_i2c.h>

#define N32_I2C_TRANSFER_TIMEOUT_MSEC 500

#define N32_I2C_TIMEOUT_USEC 1000
#define I2C_REQUEST_WRITE    0x00
#define I2C_REQUEST_READ     0x01
#define HEADER               0xF0

static void n32_i2c_generate_start_condition(I2C_TypeDef *i2c)
{
	uint16_t cr1 = LL_I2C_ReadReg(i2c, CR1);

	if (cr1 & I2C_CR1_STOP) {
		LOG_DBG("%s: START while STOP active!", __func__);
		LL_I2C_WriteReg(i2c, CR1, cr1 & ~I2C_CR1_STOP);
	}

	LL_I2C_GenerateStartCondition(i2c);
}

#ifdef CONFIG_I2C_N32_INTERRUPT

static void n32_i2c_disable_transfer_interrupts(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	LL_I2C_DisableIT_TX(i2c);
	LL_I2C_DisableIT_RX(i2c);
	LL_I2C_DisableIT_EVT(i2c);
	LL_I2C_DisableIT_BUF(i2c);

	if (!data->smbalert_active) {
		LL_I2C_DisableIT_ERR(i2c);
	}
}

static void n32_i2c_enable_transfer_interrupts(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	I2C_TypeDef *i2c = cfg->i2c;

	LL_I2C_EnableIT_ERR(i2c);
	LL_I2C_EnableIT_EVT(i2c);
	LL_I2C_EnableIT_BUF(i2c);
}

#endif /* CONFIG_I2C_N32_INTERRUPT */

static void n32_i2c_reset(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	I2C_TypeDef *i2c = cfg->i2c;
	uint16_t cr1, cr2, oar1, oar2, trise, ccr;
#if defined(I2C_FLTR_ANOFF) && defined(I2C_FLTR_DNF)
	uint16_t fltr;
#endif

	/* disable i2c and disable IRQ's */
	LL_I2C_Disable(i2c);
#ifdef CONFIG_I2C_N32_INTERRUPT
	n32_i2c_disable_transfer_interrupts(dev);
#endif

	/* save all important registers before reset */
	cr1 = LL_I2C_ReadReg(i2c, CR1);
	cr2 = LL_I2C_ReadReg(i2c, CR2);
	oar1 = LL_I2C_ReadReg(i2c, OAR1);
	oar2 = LL_I2C_ReadReg(i2c, OAR2);
	ccr = LL_I2C_ReadReg(i2c, CCR);
	trise = LL_I2C_ReadReg(i2c, TRISE);
#if defined(I2C_FLTR_ANOFF) && defined(I2C_FLTR_DNF)
	fltr = LL_I2C_ReadReg(i2c, FLTR);
#endif

	/* reset i2c hardware */
	LL_I2C_EnableReset(i2c);
	LL_I2C_DisableReset(i2c);

	/* restore all important registers after reset */
	LL_I2C_WriteReg(i2c, CR1, cr1);
	LL_I2C_WriteReg(i2c, CR2, cr2);

	/* bit 14 of OAR1 must always be 1 */
	oar1 |= (1 << 14);
	LL_I2C_WriteReg(i2c, OAR1, oar1);
	LL_I2C_WriteReg(i2c, OAR2, oar2);
	LL_I2C_WriteReg(i2c, CCR, ccr);
	LL_I2C_WriteReg(i2c, TRISE, trise);
#if defined(I2C_FLTR_ANOFF) && defined(I2C_FLTR_DNF)
	LL_I2C_WriteReg(i2c, FLTR, fltr);
#endif
}

static void n32_i2c_master_finish(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

#ifdef CONFIG_I2C_N32_INTERRUPT
	n32_i2c_disable_transfer_interrupts(dev);
#endif

#if defined(CONFIG_I2C_TARGET)
	data->master_active = false;
	if (!data->slave_attached && !data->smbalert_active) {
		LL_I2C_Disable(i2c);
	} else {
		n32_i2c_enable_transfer_interrupts(dev);
		LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
	}
#else
	if (!data->smbalert_active) {
		LL_I2C_Disable(i2c);
	}
#endif
}

static inline void msg_init(const struct device *dev, struct i2c_msg *msg, uint8_t *next_msg_flags,
			    uint16_t slave, uint32_t transfer)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	ARG_UNUSED(next_msg_flags);

#ifdef CONFIG_I2C_N32_INTERRUPT
	k_sem_reset(&data->device_sync_sem);
#endif

	data->current.len = msg->len;
	data->current.buf = msg->buf;
	data->current.flags = msg->flags;
	data->current.is_restart = 0U;
	data->current.is_write = (transfer == I2C_REQUEST_WRITE);
	data->current.is_arlo = 0U;
	data->current.is_err = 0U;
	data->current.is_nack = 0U;
	data->current.msg = msg;
#if defined(CONFIG_I2C_TARGET)
	data->master_active = true;
#endif
	data->slave_address = slave;

	LL_I2C_Enable(i2c);

	LL_I2C_DisableBitPOS(i2c);
	LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
	if (msg->flags & I2C_MSG_RESTART) {
		n32_i2c_generate_start_condition(i2c);
	}
}

static int32_t msg_end(const struct device *dev, uint8_t *next_msg_flags, const char *funcname)
{
	struct i2c_n32_data *data = dev->data;

	if (data->current.is_nack || data->current.is_err || data->current.is_arlo) {
		goto error;
	}

	if (!next_msg_flags) {
		n32_i2c_master_finish(dev);
	}

	return 0;

error:
	if (data->current.is_arlo) {
		LOG_DBG("%s: ARLO %d", funcname, data->current.is_arlo);
		data->current.is_arlo = 0U;
	}

	if (data->current.is_nack) {
		LOG_DBG("%s: NACK", funcname);
		data->current.is_nack = 0U;
	}

	if (data->current.is_err) {
		LOG_DBG("%s: ERR %d", funcname, data->current.is_err);
		data->current.is_err = 0U;
	}
	n32_i2c_master_finish(dev);

	return -EIO;
}

#ifdef CONFIG_I2C_N32_INTERRUPT

static void n32_i2c_master_mode_end(const struct device *dev)
{
	struct i2c_n32_data *data = dev->data;

	k_sem_give(&data->device_sync_sem);
}

static inline void handle_sb(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	uint16_t saddr = data->slave_address;
	uint8_t slave;

	if (I2C_ADDR_10_BITS & data->dev_config) {
		slave = (((saddr & 0x0300) >> 7) & 0xFF);
		uint8_t header = slave | HEADER;

		if (data->current.is_restart == 0U) {
			data->current.is_restart = 1U;
		} else {
			header |= I2C_REQUEST_READ;
			data->current.is_restart = 0U;
		}
		LL_I2C_TransmitData8(i2c, header);

		return;
	}
	slave = (saddr << 1) & 0xFF;
	if (data->current.is_write) {
		LL_I2C_TransmitData8(i2c, slave | I2C_REQUEST_WRITE);
	} else {
		LL_I2C_TransmitData8(i2c, slave | I2C_REQUEST_READ);
		if (data->current.len == 2) {
			LL_I2C_EnableBitPOS(i2c);
		}
	}
}

static inline void handle_addr(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	if (I2C_ADDR_10_BITS & data->dev_config) {
		if (!data->current.is_write && data->current.is_restart) {
			data->current.is_restart = 0U;
			LL_I2C_ClearFlag_ADDR(i2c);
			n32_i2c_generate_start_condition(i2c);

			return;
		}
	}

	if (data->current.is_write) {
		LL_I2C_ClearFlag_ADDR(i2c);
		return;
	}

	if (data->current.len == 0U && (IS_ENABLED(CONFIG_SOC_SERIES_N32G4FR)||IS_ENABLED(CONFIG_SOC_SERIES_N32G430))) {
		LL_I2C_GenerateStopCondition(i2c);
	} else if (data->current.len == 1U) {
		/* Single byte reception: enable NACK and clear POS */
		LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
#if (defined(CONFIG_SOC_SERIES_N32G4FR) || defined(CONFIG_SOC_SERIES_N32G430))
		LL_I2C_ClearFlag_ADDR(i2c);
		LL_I2C_GenerateStopCondition(i2c);
#endif
	} else if (data->current.len == 2U) {
#if (defined(CONFIG_SOC_SERIES_N32G4FR) || defined(CONFIG_SOC_SERIES_N32G430))
		LL_I2C_ClearFlag_ADDR(i2c);
#endif
		/* 2-byte reception: enable NACK and set POS */
		LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
		LL_I2C_EnableBitPOS(i2c);
	}
	LL_I2C_ClearFlag_ADDR(i2c);
}

static inline void handle_txe(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	if (data->current.len) {
		data->current.len--;
		if (data->current.len == 0U) {
			/*
			 * This is the last byte to transmit disable Buffer
			 * interrupt and wait for a BTF interrupt
			 */
			LL_I2C_DisableIT_BUF(i2c);
		}
		LL_I2C_TransmitData8(i2c, *data->current.buf);
		data->current.buf++;
	} else {
		if (data->current.flags & I2C_MSG_STOP) {
			LL_I2C_GenerateStopCondition(i2c);
		}
		if (LL_I2C_IsActiveFlag_BTF(i2c)) {
			/* Read DR to clear BTF flag */
			LL_I2C_ReceiveData8(i2c);
		}

		k_sem_give(&data->device_sync_sem);
	}
}

static inline void handle_rxne(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	if (data->current.len > 0) {
		switch (data->current.len) {
		case 1:
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
			LL_I2C_DisableBitPOS(i2c);
			/* Single byte reception */
			if (data->current.flags & I2C_MSG_STOP) {
				LL_I2C_GenerateStopCondition(i2c);
			}
			LL_I2C_DisableIT_BUF(i2c);
			data->current.len--;
			*data->current.buf = LL_I2C_ReceiveData8(i2c);
			data->current.buf++;

			k_sem_give(&data->device_sync_sem);
			break;
		case 2:
			/*
			 * 2-byte reception for N > 3 has already set the NACK
			 * bit, and must not set the POS bit. See pg. 854 in
			 * the F4 reference manual (RM0090).
			 */
			if (data->current.msg->len > 2) {
				break;
			}
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
			LL_I2C_EnableBitPOS(i2c);
			__fallthrough;
		case 3:
			/*
			 * 2-byte, 3-byte reception and for N-2, N-1,
			 * N when N > 3
			 */
			LL_I2C_DisableIT_BUF(i2c);
			break;
		default:
			/* N byte reception when N > 3 */
			data->current.len--;
			*data->current.buf = LL_I2C_ReceiveData8(i2c);
			data->current.buf++;
		}
	} else {

		if (data->current.flags & I2C_MSG_STOP) {
			LL_I2C_GenerateStopCondition(i2c);
		}
		k_sem_give(&data->device_sync_sem);
	}
}

static inline void handle_btf(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	if (data->current.is_write) {
		handle_txe(dev);
	} else {
		uint32_t counter = 0U;

		switch (data->current.len) {
		case 2:
			/*
			 * Stop condition must be generated before reading the
			 * last two bytes.
			 */
			if (data->current.flags & I2C_MSG_STOP) {
				LL_I2C_GenerateStopCondition(i2c);
			}

			for (counter = 2U; counter > 0; counter--) {
				data->current.len--;
				*data->current.buf = LL_I2C_ReceiveData8(i2c);
				data->current.buf++;
			}
			k_sem_give(&data->device_sync_sem);
			break;
		case 3:
			/* Set NACK before reading N-2 byte*/
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
			data->current.len--;
			*data->current.buf = LL_I2C_ReceiveData8(i2c);
			data->current.buf++;
			break;
		default:
			handle_rxne(dev);
		}
	}
}

#if defined(CONFIG_I2C_TARGET)
static void n32_i2c_slave_event(const struct device *dev)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;
	const struct i2c_target_callbacks *slave_cb = data->slave_cfg->callbacks;

	if (LL_I2C_IsActiveFlag_TXE(i2c) && LL_I2C_IsActiveFlag_BTF(i2c)) {
		uint8_t val;
		slave_cb->read_processed(data->slave_cfg, &val);
		LL_I2C_TransmitData8(i2c, val);
		return;
	}

	if (LL_I2C_IsActiveFlag_RXNE(i2c)) {
		uint8_t val = LL_I2C_ReceiveData8(i2c);
		if (slave_cb->write_received(data->slave_cfg, val)) {
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
		}
		return;
	}

	if (LL_I2C_IsActiveFlag_AF(i2c)) {
		LL_I2C_ClearFlag_AF(i2c);
	}

	if (LL_I2C_IsActiveFlag_STOP(i2c)) {
		LL_I2C_ClearFlag_STOP(i2c);
		slave_cb->stop(data->slave_cfg);
		/* Prepare to ACK next transmissions address byte */
		LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
	}

	if (LL_I2C_IsActiveFlag_ADDR(i2c)) {
		uint32_t dir = LL_I2C_GetTransferDirection(i2c);
		if (dir == LL_I2C_DIRECTION_READ) {
			slave_cb->write_requested(data->slave_cfg);
			LL_I2C_EnableIT_RX(i2c);
		} else {
			uint8_t val;
			slave_cb->read_requested(data->slave_cfg, &val);
			LL_I2C_TransmitData8(i2c, val);
			LL_I2C_EnableIT_TX(i2c);
		}

		n32_i2c_enable_transfer_interrupts(dev);
	}
}

/* Attach and start I2C as slave */
int i2c_n32_target_register(const struct device *dev, struct i2c_target_config *config)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;
	uint32_t bitrate_cfg;
	int ret;

	if (!config) {
		return -EINVAL;
	}

	if (data->slave_attached) {
		return -EBUSY;
	}

	if (data->master_active) {
		return -EBUSY;
	}

	bitrate_cfg = i2c_map_dt_bitrate(cfg->bitrate);

	ret = i2c_n32_runtime_configure(dev, bitrate_cfg);
	if (ret < 0) {
		LOG_ERR("i2c: failure initializing");
		return ret;
	}

	data->slave_cfg = config;

	LL_I2C_Enable(i2c);

	if (data->slave_cfg->flags == I2C_TARGET_FLAGS_ADDR_10_BITS) {
		return -ENOTSUP;
	}
	LL_I2C_SetOwnAddress1(i2c, config->address << 1U, LL_I2C_OWNADDRESS1_7BIT);
	data->slave_attached = true;

	LOG_DBG("i2c: target registered");

	n32_i2c_enable_transfer_interrupts(dev);
	LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);

	return 0;
}

int i2c_n32_target_unregister(const struct device *dev, struct i2c_target_config *config)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	if (!data->slave_attached) {
		return -EINVAL;
	}

	if (data->master_active) {
		return -EBUSY;
	}

	n32_i2c_disable_transfer_interrupts(dev);

	LL_I2C_ClearFlag_AF(i2c);
	LL_I2C_ClearFlag_STOP(i2c);
	LL_I2C_ClearFlag_ADDR(i2c);

	if (!data->smbalert_active) {
		LL_I2C_Disable(i2c);
	}

	data->slave_attached = false;

	LOG_DBG("i2c: slave unregistered");

	return 0;
}
#endif /* defined(CONFIG_I2C_TARGET) */

void n32_i2c_event_isr(void *arg)
{
	const struct device *dev = (const struct device *)arg;
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

#if defined(CONFIG_I2C_TARGET)
	if (data->slave_attached && !data->master_active) {
		n32_i2c_slave_event(dev);
		return;
	}
#endif

	if (LL_I2C_IsActiveFlag_SB(i2c)) {
		handle_sb(dev);
	} else if (LL_I2C_IsActiveFlag_ADD10(i2c)) {
		LL_I2C_TransmitData8(i2c, data->slave_address);
	} else if (LL_I2C_IsActiveFlag_ADDR(i2c)) {
		handle_addr(dev);
	} else if (LL_I2C_IsActiveFlag_BTF(i2c)) {
		handle_btf(dev);
	} else if (LL_I2C_IsActiveFlag_TXE(i2c) && data->current.is_write) {
		handle_txe(dev);
	} else if (LL_I2C_IsActiveFlag_RXNE(i2c) && !data->current.is_write) {
		handle_rxne(dev);
	}
}

void n32_i2c_error_isr(void *arg)
{
	const struct device *dev = (const struct device *)arg;
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

#if defined(CONFIG_I2C_TARGET)
	if (data->slave_attached && !data->master_active) {
		/* No need for a slave error function right now. */
		return;
	}
#endif

	if (LL_I2C_IsActiveFlag_AF(i2c)) {
		LL_I2C_ClearFlag_AF(i2c);
		LL_I2C_GenerateStopCondition(i2c);
		data->current.is_nack = 1U;
		goto end;
	}
	if (LL_I2C_IsActiveFlag_ARLO(i2c)) {
		LL_I2C_ClearFlag_ARLO(i2c);
		data->current.is_arlo = 1U;
		goto end;
	}

	if (LL_I2C_IsActiveFlag_BERR(i2c)) {
		LL_I2C_ClearFlag_BERR(i2c);
		data->current.is_err = 1U;
		goto end;
	}

#if defined(CONFIG_SMBUS_N32_SMBALERT)
	if (LL_I2C_IsActiveSMBusFlag_ALERT(i2c)) {
		LL_I2C_ClearSMBusFlag_ALERT(i2c);
		if (data->smbalert_cb_func != NULL) {
			data->smbalert_cb_func(data->smbalert_cb_dev);
		}
		goto end;
	}
#endif
	return;
end:
	n32_i2c_master_mode_end(dev);
}

static int32_t n32_i2c_msg_write(const struct device *dev, struct i2c_msg *msg,
				 uint8_t *next_msg_flags, uint16_t saddr)
{
	struct i2c_n32_data *data = dev->data;

	msg_init(dev, msg, next_msg_flags, saddr, I2C_REQUEST_WRITE);

	n32_i2c_enable_transfer_interrupts(dev);

	if (k_sem_take(&data->device_sync_sem, K_MSEC(N32_I2C_TRANSFER_TIMEOUT_MSEC)) != 0) {
		LOG_DBG("%s: WRITE timeout", __func__);
		n32_i2c_reset(dev);
		return -EIO;
	}

	return msg_end(dev, next_msg_flags, __func__);
}

static int32_t n32_i2c_msg_read(const struct device *dev, struct i2c_msg *msg,
				uint8_t *next_msg_flags, uint16_t saddr)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	msg_init(dev, msg, next_msg_flags, saddr, I2C_REQUEST_READ);

	n32_i2c_enable_transfer_interrupts(dev);
	LL_I2C_EnableIT_RX(i2c);

	if (k_sem_take(&data->device_sync_sem, K_MSEC(N32_I2C_TRANSFER_TIMEOUT_MSEC)) != 0) {
		LOG_DBG("%s: READ timeout", __func__);
		n32_i2c_reset(dev);
		return -EIO;
	}

	return msg_end(dev, next_msg_flags, __func__);
}

#else  /* CONFIG_I2C_N32_INTERRUPT */

static inline int check_errors(const struct device *dev, const char *funcname)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	if (LL_I2C_IsActiveFlag_AF(i2c)) {
		LL_I2C_ClearFlag_AF(i2c);
		LOG_ERR("%s: NACK", funcname);
		data->current.is_nack = 1U;
		goto error;
	}

	if (LL_I2C_IsActiveFlag_ARLO(i2c)) {
		LL_I2C_ClearFlag_ARLO(i2c);
		LOG_ERR("%s: ARLO", funcname);
		data->current.is_arlo = 1U;
		goto error;
	}

	if (LL_I2C_IsActiveFlag_OVR(i2c)) {
		LL_I2C_ClearFlag_OVR(i2c);
		LOG_ERR("%s: OVR", funcname);
		data->current.is_err = 1U;
		goto error;
	}

	if (LL_I2C_IsActiveFlag_BERR(i2c)) {
		LL_I2C_ClearFlag_BERR(i2c);
		LOG_ERR("%s: BERR", funcname);
		data->current.is_err = 1U;
		goto error;
	}

	return 0;
error:
	return -EIO;
}

static int n32_i2c_wait_timeout(uint16_t *timeout)
{
	if (*timeout == 0) {
		return 1;
	} else {
		k_busy_wait(1);
		(*timeout)--;
		return 0;
	}
}

static int32_t n32_i2c_msg_write(const struct device *dev, struct i2c_msg *msg,
				 uint8_t *next_msg_flags, uint16_t saddr)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;
	uint32_t len = msg->len;
	uint16_t timeout;
	uint8_t *buf = msg->buf;
	int32_t res;

	msg_init(dev, msg, next_msg_flags, saddr, I2C_REQUEST_WRITE);

	if (msg->flags & I2C_MSG_RESTART) {
		timeout = N32_I2C_TIMEOUT_USEC;
		while (!LL_I2C_IsActiveFlag_SB(i2c)) {
			if (n32_i2c_wait_timeout(&timeout)) {
				LL_I2C_GenerateStopCondition(i2c);
				data->current.is_err = 1U;
				goto end;
			}
		}

		if (I2C_ADDR_10_BITS & data->dev_config) {
			uint8_t slave = (((saddr & 0x0300) >> 7) & 0xFF);
			uint8_t header = slave | HEADER;

			LL_I2C_TransmitData8(i2c, header);
			timeout = N32_I2C_TIMEOUT_USEC;
			while (!LL_I2C_IsActiveFlag_ADD10(i2c)) {
				if (n32_i2c_wait_timeout(&timeout)) {
					LL_I2C_GenerateStopCondition(i2c);
					data->current.is_err = 1U;
					goto end;
				}
			}

			slave = data->slave_address & 0xFF;
			LL_I2C_TransmitData8(i2c, slave);
		} else {
			uint8_t slave = (saddr << 1) & 0xFF;

			LL_I2C_TransmitData8(i2c, slave | I2C_REQUEST_WRITE);
		}

		timeout = N32_I2C_TIMEOUT_USEC;
		while (!LL_I2C_IsActiveFlag_ADDR(i2c)) {
			if (LL_I2C_IsActiveFlag_AF(i2c) || n32_i2c_wait_timeout(&timeout)) {
				LL_I2C_ClearFlag_AF(i2c);
				LL_I2C_GenerateStopCondition(i2c);
				data->current.is_nack = 1U;
				goto end;
			}
		}
		LL_I2C_ClearFlag_ADDR(i2c);
	}

	while (len) {
		timeout = N32_I2C_TIMEOUT_USEC;
		while (1) {
			if (LL_I2C_IsActiveFlag_TXE(i2c)) {
				break;
			}
			if (LL_I2C_IsActiveFlag_AF(i2c) || n32_i2c_wait_timeout(&timeout)) {
				LL_I2C_ClearFlag_AF(i2c);
				LL_I2C_GenerateStopCondition(i2c);
				data->current.is_nack = 1U;
				goto end;
			}
		}
		LL_I2C_TransmitData8(i2c, *buf);
		buf++;
		len--;
	}

	timeout = N32_I2C_TIMEOUT_USEC;
	while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
		if (n32_i2c_wait_timeout(&timeout)) {
			LL_I2C_GenerateStopCondition(i2c);
			data->current.is_err = 1U;
			goto end;
		}
	}

	if (msg->flags & I2C_MSG_STOP) {
		LL_I2C_GenerateStopCondition(i2c);
	}

end:
	check_errors(dev, __func__);
	res = msg_end(dev, next_msg_flags, __func__);
	if (res < 0) {
		n32_i2c_reset(dev);
	}

	return res;
}

static int32_t n32_i2c_msg_read(const struct device *dev, struct i2c_msg *msg,
				uint8_t *next_msg_flags, uint16_t saddr)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;
	uint32_t len = msg->len;
	uint16_t timeout;
	uint8_t *buf = msg->buf;
	int32_t res;

	msg_init(dev, msg, next_msg_flags, saddr, I2C_REQUEST_READ);

	if (msg->flags & I2C_MSG_RESTART) {
		timeout = N32_I2C_TIMEOUT_USEC;
		while (!LL_I2C_IsActiveFlag_SB(i2c)) {
			if (n32_i2c_wait_timeout(&timeout)) {
				LL_I2C_GenerateStopCondition(i2c);
				data->current.is_err = 1U;
				goto end;
			}
		}

		if (I2C_ADDR_10_BITS & data->dev_config) {
			uint8_t slave = (((saddr & 0x0300) >> 7) & 0xFF);
			uint8_t header = slave | HEADER;

			LL_I2C_TransmitData8(i2c, header);
			timeout = N32_I2C_TIMEOUT_USEC;
			while (!LL_I2C_IsActiveFlag_ADD10(i2c)) {
				if (n32_i2c_wait_timeout(&timeout)) {
					LL_I2C_GenerateStopCondition(i2c);
					data->current.is_err = 1U;
					goto end;
				}
			}

			slave = saddr & 0xFF;
			LL_I2C_TransmitData8(i2c, slave);
			timeout = N32_I2C_TIMEOUT_USEC;
			while (!LL_I2C_IsActiveFlag_ADDR(i2c)) {
				if (n32_i2c_wait_timeout(&timeout)) {
					LL_I2C_GenerateStopCondition(i2c);
					data->current.is_err = 1U;
					goto end;
				}
			}

			LL_I2C_ClearFlag_ADDR(i2c);
			n32_i2c_generate_start_condition(i2c);
			timeout = N32_I2C_TIMEOUT_USEC;
			while (!LL_I2C_IsActiveFlag_SB(i2c)) {
				if (n32_i2c_wait_timeout(&timeout)) {
					LL_I2C_GenerateStopCondition(i2c);
					data->current.is_err = 1U;
					goto end;
				}
			}

			header |= I2C_REQUEST_READ;
			LL_I2C_TransmitData8(i2c, header);
		} else {
			uint8_t slave = ((saddr) << 1) & 0xFF;

			LL_I2C_TransmitData8(i2c, slave | I2C_REQUEST_READ);
		}

		timeout = N32_I2C_TIMEOUT_USEC;
		while (!LL_I2C_IsActiveFlag_ADDR(i2c)) {
			if (LL_I2C_IsActiveFlag_AF(i2c) || n32_i2c_wait_timeout(&timeout)) {
				LL_I2C_ClearFlag_AF(i2c);
				LL_I2C_GenerateStopCondition(i2c);
				data->current.is_nack = 1U;
				goto end;
			}
		}
		/* ADDR must be cleared before NACK generation. Either in 2 byte reception
		 * byte 1 will be NACK'ed and slave won't sent the last byte
		 */
		LL_I2C_ClearFlag_ADDR(i2c);
		if (len == 1U) {
			/* Single byte reception: enable NACK and set STOP */
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
		} else if (len == 2U) {
			/* 2-byte reception: enable NACK and set POS */
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
			LL_I2C_EnableBitPOS(i2c);
		}
	}

	while (len) {
		timeout = N32_I2C_TIMEOUT_USEC;
		while (!LL_I2C_IsActiveFlag_RXNE(i2c)) {
			if (n32_i2c_wait_timeout(&timeout)) {
				LL_I2C_GenerateStopCondition(i2c);
				data->current.is_err = 1U;
				goto end;
			}
		}

		timeout = N32_I2C_TIMEOUT_USEC;
		switch (len) {
		case 1:
			if (msg->flags & I2C_MSG_STOP) {
				LL_I2C_GenerateStopCondition(i2c);
			}
			len--;
			*buf = LL_I2C_ReceiveData8(i2c);
			buf++;
			break;
		case 2:
			while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
				if (n32_i2c_wait_timeout(&timeout)) {
					LL_I2C_GenerateStopCondition(i2c);
					data->current.is_err = 1U;
					goto end;
				}
			}

			/*
			 * Stop condition must be generated before reading the
			 * last two bytes.
			 */
			if (msg->flags & I2C_MSG_STOP) {
				LL_I2C_GenerateStopCondition(i2c);
			}

			for (uint32_t counter = 2; counter > 0; counter--) {
				len--;
				*buf = LL_I2C_ReceiveData8(i2c);
				buf++;
			}

			break;
		case 3:
			while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
				if (n32_i2c_wait_timeout(&timeout)) {
					LL_I2C_GenerateStopCondition(i2c);
					data->current.is_err = 1U;
					goto end;
				}
			}

			/* Set NACK before reading N-2 byte*/
			LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
			__fallthrough;
		default:
			len--;
			*buf = LL_I2C_ReceiveData8(i2c);
			buf++;
		}
	}
end:
	check_errors(dev, __func__);
	res = msg_end(dev, next_msg_flags, __func__);
	if (res < 0) {
		n32_i2c_reset(dev);
	}

	return res;
}
#endif /* CONFIG_I2C_N32_INTERRUPT */

int32_t n32_i2c_configure_timing(const struct device *dev, uint32_t clock)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	switch (I2C_SPEED_GET(data->dev_config)) {
	case I2C_SPEED_STANDARD:
		LL_I2C_ConfigSpeed(i2c, clock, 100000, LL_I2C_DUTYCYCLE_2);
		break;
	case I2C_SPEED_FAST:
		LL_I2C_ConfigSpeed(i2c, clock, 400000, LL_I2C_DUTYCYCLE_2);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

int n32_i2c_transaction(const struct device *dev, struct i2c_msg msg, uint8_t *next_msg_flags,
			uint16_t periph)
{
	int ret;

	if ((msg.flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE) {
		ret = n32_i2c_msg_write(dev, &msg, next_msg_flags, periph);
	} else {
		ret = n32_i2c_msg_read(dev, &msg, next_msg_flags, periph);
	}
	return ret;
}

int i2c_n32_get_config(const struct device *dev, uint32_t *config)
{
	struct i2c_n32_data *data = dev->data;

	if (!data->is_configured) {
		LOG_ERR("I2C controller not configured");
		return -EIO;
	}

	*config = data->dev_config;

	return 0;
}

int i2c_n32_runtime_configure(const struct device *dev, uint32_t config)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));

	I2C_TypeDef *i2c = cfg->i2c;
	uint32_t i2c_clock = 0U;
	int ret;

	if (clock_control_get_rate(clk, (clock_control_subsys_t)&cfg->clkid, &i2c_clock) < 0) {
		LOG_ERR("Failed call clock_control_get_rate(clkid)");
		return -EIO;
	}
	data->dev_config = config;

	k_sem_take(&data->bus_mutex, K_FOREVER);

#ifdef CONFIG_PM_DEVICE_RUNTIME
	ret = clock_control_on(clk, (clock_control_subsys_t)&cfg->clkid);
	if (ret < 0) {
		LOG_ERR("failure Enabling I2C clock");
		return ret;
	}
#endif

	LL_I2C_Disable(i2c);
	i2c_n32_set_smbus_mode(dev, data->mode);
	ret = n32_i2c_configure_timing(dev, i2c_clock);

	if (data->smbalert_active) {
		LL_I2C_Enable(i2c);
	}

#ifdef CONFIG_PM_DEVICE_RUNTIME
	ret = clock_control_off(clk, (clock_control_subsys_t)&cfg->clkid);
	if (ret < 0) {
		LOG_ERR("failure disabling I2C clock");
		return ret;
	}
#endif

	k_sem_give(&data->bus_mutex);

	return ret;
}

#define OPERATION(msg) (((struct i2c_msg *)msg)->flags & I2C_MSG_RW_MASK)

static int i2c_n32_transfer(const struct device *dev, struct i2c_msg *msg, uint8_t num_msgs,
			    uint16_t slave)
{
	struct i2c_n32_data *data = dev->data;
	struct i2c_msg *current, *next;
	int ret = 0;

	/* Check for validity of all messages, to prevent having to abort
	 * in the middle of a transfer
	 */
	current = msg;

	/*
	 * Set I2C_MSG_RESTART flag on first message in order to send start
	 * condition
	 */
	current->flags |= I2C_MSG_RESTART;

	for (uint8_t i = 1; i <= num_msgs; i++) {

		if (i < num_msgs) {
			next = current + 1;

			/*
			 * Restart condition between messages
			 * of different directions is required
			 */
			if (OPERATION(current) != OPERATION(next)) {
				if (!(next->flags & I2C_MSG_RESTART)) {
					ret = -EINVAL;
					break;
				}
			}

			/* Stop condition is only allowed on last message */
			if (current->flags & I2C_MSG_STOP) {
				ret = -EINVAL;
				break;
			}
		}

		current++;
	}

	if (ret) {
		return ret;
	}

	/* Send out messages */
	k_sem_take(&data->bus_mutex, K_FOREVER);

	/* Prevent driver from being suspended by PM until I2C transaction is complete */
#ifdef CONFIG_PM_DEVICE_RUNTIME
	(void)pm_device_runtime_get(dev);
#endif

	/* Prevent the clocks to be stopped during the i2c transaction */
	pm_policy_state_lock_get(PM_STATE_SUSPEND_TO_IDLE, PM_ALL_SUBSTATES);

	current = msg;

	while (num_msgs > 0) {
		uint8_t *next_msg_flags = NULL;

		if (num_msgs > 1) {
			next = current + 1;
			next_msg_flags = &(next->flags);
		}
		ret = n32_i2c_transaction(dev, *current, next_msg_flags, slave);
		if (ret < 0) {
			break;
		}
		current++;
		num_msgs--;
	}

	pm_policy_state_lock_put(PM_STATE_SUSPEND_TO_IDLE, PM_ALL_SUBSTATES);

#ifdef CONFIG_PM_DEVICE_RUNTIME
	(void)pm_device_runtime_put(dev);
#endif

	k_sem_give(&data->bus_mutex);

	return ret;
}

void i2c_disable_pins(const struct device *dev)
{
#ifdef CONFIG_I2C_N32_BUS_RECOVERY
	const struct i2c_n32_config *config = dev->config;

	int error = 0;
	error = gpio_pin_configure_dt(&config->scl, (GPIO_OPEN_DRAIN | GPIO_PULL_UP));
	if (error != 0) {
		LOG_ERR("failed to configure SCL GPIO (err %d)", error);
	}

	error = gpio_pin_configure_dt(&config->sda, (GPIO_OPEN_DRAIN | GPIO_PULL_UP));
	if (error != 0) {
		LOG_ERR("failed to configure SDA GPIO (err %d)", error);
	}
#endif
};

#if CONFIG_I2C_N32_BUS_RECOVERY
static void i2c_n32_bitbang_set_scl(void *io_context, int state)
{
	const struct i2c_n32_config *config = io_context;

	gpio_pin_set_dt(&config->scl, state);
}

static void i2c_n32_bitbang_set_sda(void *io_context, int state)
{
	const struct i2c_n32_config *config = io_context;

	gpio_pin_set_dt(&config->sda, state);
}

static int i2c_n32_bitbang_get_sda(void *io_context)
{
	const struct i2c_n32_config *config = io_context;

	return gpio_pin_get_dt(&config->sda) == 0 ? 0 : 1;
}

static int i2c_n32_recover_bus(const struct device *dev)
{
	const struct i2c_n32_config *config = dev->config;
	struct i2c_n32_data *data = dev->data;
	struct i2c_bitbang bitbang_ctx;
	struct i2c_bitbang_io bitbang_io = {
		.set_scl = i2c_n32_bitbang_set_scl,
		.set_sda = i2c_n32_bitbang_set_sda,
		.get_sda = i2c_n32_bitbang_get_sda,
	};
	uint32_t bitrate_cfg;
	int error = 0;

	LOG_WRN("attempting to recover bus");

	if (!gpio_is_ready_dt(&config->scl)) {
		LOG_ERR("SCL GPIO device not ready");
		return -EIO;
	}

	if (!gpio_is_ready_dt(&config->sda)) {
		LOG_ERR("SDA GPIO device not ready");
		return -EIO;
	}

	k_sem_take(&data->bus_mutex, K_FOREVER);

	error = gpio_pin_configure_dt(&config->scl, GPIO_OUTPUT_HIGH);
	if (error != 0) {
		LOG_ERR("failed to configure SCL GPIO (err %d)", error);
		goto restore;
	}

	error = gpio_pin_configure_dt(&config->sda, GPIO_OUTPUT_HIGH);
	if (error != 0) {
		LOG_ERR("failed to configure SDA GPIO (err %d)", error);
		goto restore;
	}

	i2c_bitbang_init(&bitbang_ctx, &bitbang_io, (void *)config);

	bitrate_cfg = i2c_map_dt_bitrate(config->bitrate) | I2C_MODE_CONTROLLER;
	error = i2c_bitbang_configure(&bitbang_ctx, bitrate_cfg);
	if (error != 0) {
		LOG_ERR("failed to configure I2C bitbang (err %d)", error);
		goto restore;
	}

	error = i2c_bitbang_recover_bus(&bitbang_ctx);
	if (error != 0) {
		LOG_ERR("failed to recover bus (err %d)", error);
	}

restore:
	(void)pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);

	k_sem_give(&data->bus_mutex);

	return error;
}
#endif /* CONFIG_I2C_N32_BUS_RECOVERY */

static const struct i2c_driver_api api_funcs = {
	.configure = i2c_n32_runtime_configure,
	.transfer = i2c_n32_transfer,
	.get_config = i2c_n32_get_config,
#if CONFIG_I2C_N32_BUS_RECOVERY
	.recover_bus = i2c_n32_recover_bus,
#endif /* CONFIG_I2C_N32_BUS_RECOVERY */
#if defined(CONFIG_I2C_TARGET)
	.target_register = i2c_n32_target_register,
	.target_unregister = i2c_n32_target_unregister,
#endif
};

#ifdef CONFIG_PM_DEVICE

static int i2c_n32_suspend(const struct device *dev)
{
	int ret;
	const struct i2c_n32_config *cfg = dev->config;
	const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));

	/* Disable device clock. */
	ret = clock_control_off(clk, (clock_control_subsys_t)&cfg->clkid);
	if (ret < 0) {
		LOG_ERR("failure disabling I2C clock");
		return ret;
	}

	/* Move pins to sleep state */
	ret = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_SLEEP);
	if (ret == -ENOENT) {
		/* Warn but don't block suspend */
		LOG_WRN("I2C pinctrl sleep state not available ");
	} else if (ret < 0) {
		return ret;
	}

	return 0;
}

#endif

static int i2c_n32_activate(const struct device *dev)
{
	int ret;
	const struct i2c_n32_config *cfg = dev->config;
	const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));

	/* Move pins to active/default state */
	ret = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_DEFAULT);
	if (ret < 0) {
		LOG_ERR("I2C pinctrl setup failed (%d)", ret);
		return ret;
	}

	/* Enable device clock. */
	N32_RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);

	if (clock_control_on(clk, (clock_control_subsys_t)&cfg->clkid) != 0) {
		LOG_ERR("i2c: failure enabling clock");
		return -EIO;
	}

	return 0;
}

#define I2Cx_SCL_PIN GPIO_PIN_8
#define I2Cx_SDA_PIN GPIO_PIN_9

static int i2c_n32_init(const struct device *dev)
{
	const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));
	const struct i2c_n32_config *cfg = dev->config;
	uint32_t bitrate_cfg;
	int ret;
	struct i2c_n32_data *data = dev->data;
#ifdef CONFIG_I2C_N32_INTERRUPT
	k_sem_init(&data->device_sync_sem, 0, K_SEM_MAX_LIMIT);
	cfg->irq_config_func(dev);
#endif

	data->is_configured = false;
	data->mode = I2CN32MODE_I2C;

	/*
	 * initialize mutex used when multiple transfers
	 * are taking place to guarantee that each one is
	 * atomic and has exclusive access to the I2C bus.
	 */
	k_sem_init(&data->bus_mutex, 1, 1);

	if (!device_is_ready(clk)) {
		LOG_ERR("clock control device not ready");
		return -ENODEV;
	}

	i2c_n32_activate(dev);

#if (defined(CONFIG_SOC_SERIES_N32G4FR) || defined(CONFIG_SOC_SERIES_N32G430))
	I2C_TypeDef *i2c = cfg->i2c;

	LL_I2C_EnableReset(i2c);
	LL_I2C_DisableReset(i2c);
#endif

	bitrate_cfg = i2c_map_dt_bitrate(cfg->bitrate);

	ret = i2c_n32_runtime_configure(dev, I2C_MODE_CONTROLLER | bitrate_cfg);
	if (ret < 0) {
		LOG_ERR("i2c: failure initializing");
		return ret;
	}

#ifdef CONFIG_PM_DEVICE_RUNTIME
	(void)pm_device_runtime_enable(dev);
#endif

	data->is_configured = true;

	return 0;
}

#ifdef CONFIG_PM_DEVICE

static int i2c_n32_pm_action(const struct device *dev, enum pm_device_action action)
{
	int err;

	switch (action) {
	case PM_DEVICE_ACTION_RESUME:
		err = i2c_n32_activate(dev);
		break;
	case PM_DEVICE_ACTION_SUSPEND:
		err = i2c_n32_suspend(dev);
		break;
	default:
		return -ENOTSUP;
	}

	return err;
}

#endif

#ifdef CONFIG_SMBUS_N32_SMBALERT
void i2c_n32_smbalert_set_callback(const struct device *dev, i2c_n32_smbalert_cb_func_t func,
				   const struct device *cb_dev)
{
	struct i2c_n32_data *data = dev->data;

	data->smbalert_cb_func = func;
	data->smbalert_cb_dev = cb_dev;
}
#endif /* CONFIG_SMBUS_N32_SMBALERT */

void i2c_n32_set_smbus_mode(const struct device *dev, enum i2c_n32_mode mode)
{
	const struct i2c_n32_config *cfg = dev->config;
	struct i2c_n32_data *data = dev->data;
	I2C_TypeDef *i2c = cfg->i2c;

	data->mode = mode;

	switch (mode) {
	case I2CN32MODE_I2C:
		LL_I2C_SetMode(i2c, LL_I2C_MODE_I2C);
		return;
#ifdef CONFIG_SMBUS_N32
	case I2CN32MODE_SMBUSHOST:
		LL_I2C_SetMode(i2c, LL_I2C_MODE_SMBUS_HOST);
		return;
	case I2CN32MODE_SMBUSDEVICE:
		LL_I2C_SetMode(i2c, LL_I2C_MODE_SMBUS_DEVICE);
		return;
	case I2CN32MODE_SMBUSDEVICEARP:
		LL_I2C_SetMode(i2c, LL_I2C_MODE_SMBUS_DEVICE_ARP);
		return;
#endif
	default:
		LOG_ERR("%s: invalid mode %i", dev->name, mode);
		return;
	}
}

#ifdef CONFIG_SMBUS_N32
void i2c_n32_smbalert_enable(const struct device *dev)
{
	struct i2c_n32_data *data = dev->data;
	const struct i2c_n32_config *cfg = dev->config;

	data->smbalert_active = true;
	LL_I2C_EnableSMBusAlert(cfg->i2c);
	LL_I2C_EnableIT_ERR(cfg->i2c);
	LL_I2C_Enable(cfg->i2c);
}

void i2c_n32_smbalert_disable(const struct device *dev)
{
	struct i2c_n32_data *data = dev->data;
	const struct i2c_n32_config *cfg = dev->config;

	data->smbalert_active = false;
	LL_I2C_DisableSMBusAlert(cfg->i2c);
	LL_I2C_DisableIT_ERR(cfg->i2c);
	LL_I2C_Disable(cfg->i2c);
}
#endif /* CONFIG_SMBUS_N32 */

/* Macros for I2C instance declaration */

#ifdef CONFIG_I2C_N32_INTERRUPT
#define N32_I2C_IRQ_CONNECT_AND_ENABLE(index)                                                      \
	do {                                                                                       \
		IRQ_CONNECT(DT_INST_IRQ_BY_NAME(index, event, irq),                                \
			    DT_INST_IRQ_BY_NAME(index, event, priority), n32_i2c_event_isr,        \
			    DEVICE_DT_INST_GET(index), 0);                                         \
		irq_enable(DT_INST_IRQ_BY_NAME(index, event, irq));                                \
                                                                                                   \
		IRQ_CONNECT(DT_INST_IRQ_BY_NAME(index, error, irq),                                \
			    DT_INST_IRQ_BY_NAME(index, error, priority), n32_i2c_error_isr,        \
			    DEVICE_DT_INST_GET(index), 0);                                         \
		irq_enable(DT_INST_IRQ_BY_NAME(index, error, irq));                                \
	} while (false)

#define N32_I2C_IRQ_HANDLER_DECL(index)                                                            \
	static void i2c_n32_irq_config_func_##index(const struct device *dev)
#define N32_I2C_IRQ_HANDLER_FUNCTION(index) .irq_config_func = i2c_n32_irq_config_func_##index,
#define N32_I2C_IRQ_HANDLER(index)                                                                 \
	static void i2c_n32_irq_config_func_##index(const struct device *dev)                      \
	{                                                                                          \
		N32_I2C_IRQ_CONNECT_AND_ENABLE(index);                                             \
	}
#else

#define N32_I2C_IRQ_HANDLER_DECL(index)
#define N32_I2C_IRQ_HANDLER_FUNCTION(index)
#define N32_I2C_IRQ_HANDLER(index)

#endif /* CONFIG_I2C_N32_INTERRUPT */

#define N32_I2C_INIT(index)                                                                        \
	N32_I2C_IRQ_HANDLER_DECL(index);                                                           \
                                                                                                   \
	PINCTRL_DT_INST_DEFINE(index);                                                             \
                                                                                                   \
	static const struct i2c_n32_config i2c_n32_cfg_##index = {                                 \
		.i2c = (I2C_TypeDef *)DT_INST_REG_ADDR(index),                                     \
		.clkid = DT_INST_CLOCKS_CELL(index, id),                                           \
		N32_I2C_IRQ_HANDLER_FUNCTION(index).bitrate =                                      \
			DT_INST_PROP(index, clock_frequency),                                      \
		.pcfg = PINCTRL_DT_INST_DEV_CONFIG_GET(index),                                     \
		IF_ENABLED(CONFIG_I2C_N32_BUS_RECOVERY,                                            \
			   (.scl = GPIO_DT_SPEC_INST_GET_OR(index, scl_gpios, {0}),                \
			    .sda = GPIO_DT_SPEC_INST_GET_OR(index, sda_gpios, {0}), ))};           \
                                                                                                   \
	static struct i2c_n32_data i2c_n32_dev_data_##index;                                       \
                                                                                                   \
	PM_DEVICE_DT_INST_DEFINE(index, i2c_n32_pm_action);                                        \
                                                                                                   \
	I2C_DEVICE_DT_INST_DEFINE(index, i2c_n32_init, PM_DEVICE_DT_INST_GET(index),               \
				  &i2c_n32_dev_data_##index, &i2c_n32_cfg_##index, POST_KERNEL,    \
				  CONFIG_I2C_INIT_PRIORITY, &api_funcs);                           \
                                                                                                   \
	N32_I2C_IRQ_HANDLER(index)

DT_INST_FOREACH_STATUS_OKAY(N32_I2C_INIT)
