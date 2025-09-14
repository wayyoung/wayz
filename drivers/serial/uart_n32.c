/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <soc.h>
#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/clock_control_n32.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/sys_io.h>

#ifdef CONFIG_DCACHE
#include <zephyr/linker/linker-defs.h>
#include <zephyr/mem_mgmt/mem_attr.h>
#include <zephyr/dt-bindings/memory-attr/memory-attr-arm.h>
#endif /* CONFIG_DCACHE */

#define DT_DRV_COMPAT nsing_n32_uart

#define LOG_DBG(x...) 
#define LOG_WRN(x...)
#define LOG_ERR(x...)

#ifdef CONFIG_SOC_SERIES_N32G430
#define N32_USART_Data_Send(x,y) 						USART_Data_Send(x,y)
#define N32_USART_Data_Receive(x) 						USART_Data_Receive(x)
#define N32_USART_Initializes(x,y)						USART_Initializes(x,y)
#define N32_USART_Enable(x)								USART_Enable(x)
#define N32_USART_Disable(x)							USART_Disable(x)
#define N32_USART_Flag_Clear(x,y)						USART_Flag_Clear(x,y)
#define N32_USART_Flag_Status_Get(x,y)					USART_Flag_Status_Get(x,y)
#define N32_USART_Interrupt_Status_Get(x,y)				USART_Interrupt_Status_Get(x,y)
#define N32_USART_Interrupt_Status_Clear(x,y)			USART_Interrupt_Status_Clear(x,y)
#define N32_USART_Interrput_Enable(x,y)					USART_Interrput_Enable(x,y)
#define N32_USART_Interrput_Disable(x,y)				USART_Interrput_Disable(x,y)
#define N32_USART_DMA_Transfer_Enable(x,y)				USART_DMA_Transfer_Enable(x,y)
#define N32_USART_DMA_Transfer_Disable(x,y)				USART_DMA_Transfer_Disable(x,y)
#endif


#ifdef CONFIG_UART_ASYNC_API
#include <zephyr/drivers/dma/dma_n32.h>
#include <zephyr/drivers/dma.h>
struct uart_dma_stream {
	const struct device *dma_dev;
	uint32_t dma_channel;
	struct dma_config dma_cfg;
	uint8_t priority;
	bool src_addr_increment;
	bool dst_addr_increment;
	int fifo_threshold;
	struct dma_block_config blk_cfg;
	uint8_t *buffer;
	size_t buffer_length;
	size_t offset;
	volatile size_t counter;
	int32_t timeout;
	struct k_work_delayable timeout_work;
	bool enabled;
};
#endif


static void uart_n32_poll_out(const struct device *dev, unsigned char c);
struct uart_n32_config {
  struct uart_config uart_config;
  uint32_t base;
  uint32_t clkid;
  const struct pinctrl_dev_config *pcfg;
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
  uart_irq_config_func_t irq_config_func;
#endif /* CONFIG_UART_INTERRUPT_DRIVEN */
};

struct uart_n32_data {
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
  uart_irq_callback_user_data_t cb; /**< Callback function pointer */
  void *cb_data;                    /**< Callback function arg */
#endif                              /* CONFIG_UART_INTERRUPT_DRIVEN */
#ifdef CONFIG_UART_ASYNC_API
	const struct device *uart_dev;
	uart_callback_t async_cb;
	void *async_user_data;
	struct uart_dma_stream dma_rx;
	struct uart_dma_stream dma_tx;
	uint8_t *rx_next_buffer;
	size_t rx_next_buffer_len;
#endif
};

#define DEV_CFG(dev) ((const struct uart_n32_config *)(dev)->config)
#define DEV_DATA(dev) ((struct uart_n32_data *)(dev)->data)

#define UART_MODULE(config)  ((USART_Module *)(config->base))
#define UART_TYPEDEF(config) ((USART_TypeDef *)(config->base))

#ifdef CONFIG_UART_ASYNC_API

#pragma GCC push_options
#pragma GCC optimize ("O0")
static void uart_n32_clear_flag(USART_Module *uart_module)
{
	uart_module->STS;
	uart_module->DAT;
}
#pragma GCC pop_options


static inline void async_user_callback(struct uart_n32_data *data,
				struct uart_event *event)
{
	if (data->async_cb) {
		data->async_cb(data->uart_dev, event, data->async_user_data);
	}
}

static inline void async_evt_rx_rdy(struct uart_n32_data *data)
{
	LOG_DBG("rx_rdy: (%d %d)", data->dma_rx.offset, data->dma_rx.counter);

	struct uart_event event = {
		.type = UART_RX_RDY,
		.data.rx.buf = data->dma_rx.buffer,
		.data.rx.len = data->dma_rx.counter - data->dma_rx.offset,
		.data.rx.offset = data->dma_rx.offset
	};

	/* update the current pos for new data */
	data->dma_rx.offset = data->dma_rx.counter;

	/* send event only for new data */
	if (event.data.rx.len > 0) {
		async_user_callback(data, &event);
	}
}

static inline void async_evt_rx_err(struct uart_n32_data *data, int err_code)
{
	LOG_DBG("rx error: %d", err_code);

	struct uart_event event = {
		.type = UART_RX_STOPPED,
		.data.rx_stop.reason = err_code,
		.data.rx_stop.data.len = data->dma_rx.counter,
		.data.rx_stop.data.offset = 0,
		.data.rx_stop.data.buf = data->dma_rx.buffer
	};

	async_user_callback(data, &event);
}

static inline void async_evt_tx_done(struct uart_n32_data *data)
{
	LOG_DBG("tx done: %d", data->dma_tx.counter);

	struct uart_event event = {
		.type = UART_TX_DONE,
		.data.tx.buf = data->dma_tx.buffer,
		.data.tx.len = data->dma_tx.counter
	};

	/* Reset tx buffer */
	data->dma_tx.buffer_length = 0;
	data->dma_tx.counter = 0;

	async_user_callback(data, &event);
}

static inline void async_evt_tx_abort(struct uart_n32_data *data)
{
	LOG_DBG("tx abort: %d", data->dma_tx.counter);

	struct uart_event event = {
		.type = UART_TX_ABORTED,
		.data.tx.buf = data->dma_tx.buffer,
		.data.tx.len = data->dma_tx.counter
	};

	/* Reset tx buffer */
	data->dma_tx.buffer_length = 0;
	data->dma_tx.counter = 0;

	async_user_callback(data, &event);
}

static inline void async_evt_rx_buf_request(struct uart_n32_data *data)
{
	struct uart_event evt = {
		.type = UART_RX_BUF_REQUEST,
	};

	async_user_callback(data, &evt);
}

static inline void async_evt_rx_buf_release(struct uart_n32_data *data)
{
	struct uart_event evt = {
		.type = UART_RX_BUF_RELEASED,
		.data.rx_buf.buf = data->dma_rx.buffer,
	};

	async_user_callback(data, &evt);
}

static inline void async_timer_start(struct k_work_delayable *work,
				     int32_t timeout)
{
	if ((timeout != SYS_FOREVER_US) && (timeout != 0)) {
		/* start timer */
		LOG_DBG("async timer started for %d us", timeout);
		k_work_reschedule(work, K_USEC(timeout));
	}
}

static void uart_n32_dma_rx_flush(const struct device *dev)
{
	struct dma_status stat;
	struct uart_n32_data *data = dev->data;

	if (dma_get_status(data->dma_rx.dma_dev,
				data->dma_rx.dma_channel, &stat) == 0) {
		size_t rx_rcv_len = data->dma_rx.buffer_length -
					stat.pending_length;
		if (rx_rcv_len > data->dma_rx.offset) {
			data->dma_rx.counter = rx_rcv_len;

			async_evt_rx_rdy(data);
		}
	}
}



#ifdef CONFIG_DCACHE
static bool buf_in_nocache(uintptr_t buf, size_t len_bytes)
{
	bool buf_within_nocache = false;

#ifdef CONFIG_NOCACHE_MEMORY
	buf_within_nocache = (buf >= ((uintptr_t)_nocache_ram_start)) &&
		((buf + len_bytes - 1) <= ((uintptr_t)_nocache_ram_end));
	if (buf_within_nocache) {
		return true;
	}
#endif /* CONFIG_NOCACHE_MEMORY */

	buf_within_nocache = mem_attr_check_buf(
		(void *)buf, len_bytes, DT_MEM_ARM_MPU_RAM_NOCACHE) == 0;
	if (buf_within_nocache) {
		return true;
	}

	buf_within_nocache = (buf >= ((uintptr_t)__rodata_region_start)) &&
		((buf + len_bytes - 1) <= ((uintptr_t)__rodata_region_end));

	return buf_within_nocache;
}
#endif /* CONFIG_DCACHE */

static int uart_n32_async_callback_set(const struct device *dev,
					 uart_callback_t callback,
					 void *user_data)
{
	struct uart_n32_data *data = dev->data;

	data->async_cb = callback;
	data->async_user_data = user_data;

#if defined(CONFIG_UART_EXCLUSIVE_API_CALLBACKS)
	data->cb = NULL;
	data->cb_data = NULL;
#endif

	return 0;
}

static inline void uart_n32_dma_tx_enable(const struct device *dev)
{
	const struct uart_n32_config *config = dev->config;

	USART_EnableDMA(UART_MODULE(config),USART_DMAREQ_TX, ENABLE);
}

static inline void uart_n32_dma_tx_disable(const struct device *dev)
{

	const struct uart_n32_config *config = dev->config;

	USART_EnableDMA(UART_MODULE(config),USART_DMAREQ_TX, DISABLE);
}

static inline void uart_n32_dma_rx_enable(const struct device *dev)
{
	const struct uart_n32_config *config = dev->config;
	struct uart_n32_data *data = dev->data;

	USART_EnableDMA(UART_MODULE(config),USART_DMAREQ_RX, ENABLE);

	data->dma_rx.enabled = true;
}

static inline void uart_n32_dma_rx_disable(const struct device *dev)
{
	struct uart_n32_data *data = dev->data;

	data->dma_rx.enabled = false;
}

static int uart_n32_async_rx_disable(const struct device *dev)
{
	const struct uart_n32_config *config = dev->config;
	USART_Module *usart = UART_MODULE(config);
	struct uart_n32_data *data = dev->data;
	struct uart_event disabled_event = {
		.type = UART_RX_DISABLED
	};

	if (!data->dma_rx.enabled) {
		async_user_callback(data, &disabled_event);
		return -EFAULT;
	}

	USART_ConfigInt(usart, USART_INT_IDLEF, DISABLE);

	uart_n32_dma_rx_flush(dev);

	async_evt_rx_buf_release(data);

	uart_n32_dma_rx_disable(dev);

	(void)k_work_cancel_delayable(&data->dma_rx.timeout_work);

	dma_stop(data->dma_rx.dma_dev, data->dma_rx.dma_channel);

	if (data->rx_next_buffer) {
		struct uart_event rx_next_buf_release_evt = {
			.type = UART_RX_BUF_RELEASED,
			.data.rx_buf.buf = data->rx_next_buffer,
		};
		async_user_callback(data, &rx_next_buf_release_evt);
	}

	data->rx_next_buffer = NULL;
	data->rx_next_buffer_len = 0;

	/* When async rx is disabled, enable interruptible instance of uart to function normally */
	USART_ConfigInt(usart, USART_INT_RXDNE, ENABLE);

	LOG_DBG("rx: disabled");

	async_user_callback(data, &disabled_event);

	return 0;
}

void uart_n32_dma_tx_cb(const struct device *dma_dev, void *user_data,
			       uint32_t channel, int status)
{
	const struct device *uart_dev = user_data;
	struct uart_n32_data *data = uart_dev->data;
	struct dma_status stat;
	unsigned int key = irq_lock();

	/* Disable TX */
	uart_n32_dma_tx_disable(uart_dev);

	(void)k_work_cancel_delayable(&data->dma_tx.timeout_work);

	if (!dma_get_status(data->dma_tx.dma_dev,
				data->dma_tx.dma_channel, &stat)) {
		data->dma_tx.counter = data->dma_tx.buffer_length -
					stat.pending_length;
	}

	data->dma_tx.buffer_length = 0;

	irq_unlock(key);
}

static void uart_n32_dma_replace_buffer(const struct device *dev)
{
	const struct uart_n32_config *config = dev->config;
	USART_Module *usart = UART_MODULE(config);
	struct uart_n32_data *data = dev->data;

	/* Replace the buffer and reload the DMA */
	LOG_DBG("Replacing RX buffer: %d", data->rx_next_buffer_len);

	/* reload DMA */
	data->dma_rx.offset = 0;
	data->dma_rx.counter = 0;
	data->dma_rx.buffer = data->rx_next_buffer;
	data->dma_rx.buffer_length = data->rx_next_buffer_len;
	data->dma_rx.blk_cfg.block_size = data->dma_rx.buffer_length;
	data->dma_rx.blk_cfg.dest_address = (uint32_t)data->dma_rx.buffer;
	data->rx_next_buffer = NULL;
	data->rx_next_buffer_len = 0;

	dma_reload(data->dma_rx.dma_dev, data->dma_rx.dma_channel,
			data->dma_rx.blk_cfg.source_address,
			data->dma_rx.blk_cfg.dest_address,
			data->dma_rx.blk_cfg.block_size);

	dma_start(data->dma_rx.dma_dev, data->dma_rx.dma_channel);

	uart_n32_clear_flag(usart);

	/* Request next buffer */
	async_evt_rx_buf_request(data);
}

void uart_n32_dma_rx_cb(const struct device *dma_dev, void *user_data,
			       uint32_t channel, int status)
{
	const struct device *uart_dev = user_data;
	struct uart_n32_data *data = uart_dev->data;

	if (status < 0) {
		async_evt_rx_err(data, status);
		return;
	}

	(void)k_work_cancel_delayable(&data->dma_rx.timeout_work);

	/* true since this functions occurs when buffer if full */
	data->dma_rx.counter = data->dma_rx.buffer_length;

	async_evt_rx_rdy(data);

	if (data->rx_next_buffer != NULL) {
		async_evt_rx_buf_release(data);

		/* replace the buffer when the current
		 * is full and not the same as the next
		 * one.
		 */
		uart_n32_dma_replace_buffer(uart_dev);
	} else {
		/* Buffer full without valid next buffer,
		 * an UART_RX_DISABLED event must be generated,
		 * but uart_n32_async_rx_disable() cannot be
		 * called in ISR context. So force the RX timeout
		 * to minimum value and let the RX timeout to do the job.
		 */
		k_work_reschedule(&data->dma_rx.timeout_work, K_TICKS(1));
	}
}

static int uart_n32_async_tx(const struct device *dev,
		const uint8_t *tx_data, size_t buf_size, int32_t timeout)
{
	const struct uart_n32_config *config = dev->config;
	USART_Module *usart = UART_MODULE(config);
	struct uart_n32_data *data = dev->data;
	int ret;

	if (data->dma_tx.dma_dev == NULL) {
		return -ENODEV;
	}

	if (data->dma_tx.buffer_length != 0) {
		return -EBUSY;
	}

#ifdef CONFIG_DCACHE
	if (!buf_in_nocache((uintptr_t)tx_data, buf_size)) {
		LOG_ERR("Tx buffer should be placed in a nocache memory region");
		return -EFAULT;
	}
#endif /* CONFIG_DCACHE */

	data->dma_tx.buffer = (uint8_t *)tx_data;
	data->dma_tx.buffer_length = buf_size;
	data->dma_tx.timeout = timeout;

	LOG_DBG("tx: l=%d", data->dma_tx.buffer_length);

	/* Clear TC flag */
	USART_ClrFlag(usart, USART_FLAG_TXC);

	/* Enable TC interrupt so we can signal correct TX done */
	USART_ConfigInt(usart, USART_INT_TXC, ENABLE);

	/* set source address */
	data->dma_tx.blk_cfg.source_address = (uint32_t)data->dma_tx.buffer;
	data->dma_tx.blk_cfg.block_size = data->dma_tx.buffer_length;

	ret = dma_config(data->dma_tx.dma_dev, data->dma_tx.dma_channel,
				&data->dma_tx.dma_cfg);

	if (ret != 0) {
		LOG_ERR("dma tx config error!");
		return -EINVAL;
	}

	if (dma_start(data->dma_tx.dma_dev, data->dma_tx.dma_channel)) {
		LOG_ERR("UART err: TX DMA start failed!");
		return -EFAULT;
	}

	/* Start TX timer */
	async_timer_start(&data->dma_tx.timeout_work, data->dma_tx.timeout);


	/* Enable TX DMA requests */
	uart_n32_dma_tx_enable(dev);

	return 0;
}

static int uart_n32_async_rx_enable(const struct device *dev,
		uint8_t *rx_buf, size_t buf_size, int32_t timeout)
{
	const struct uart_n32_config *config = dev->config;
	USART_Module *usart = UART_MODULE(config);
	struct uart_n32_data *data = dev->data;
	int ret;

	if (data->dma_rx.dma_dev == NULL) {
		return -ENODEV;
	}

	if (data->dma_rx.enabled) {
		LOG_WRN("RX was already enabled");
		return -EBUSY;
	}

#ifdef CONFIG_DCACHE
	if (!buf_in_nocache((uintptr_t)rx_buf, buf_size)) {
		LOG_ERR("Rx buffer should be placed in a nocache memory region");
		return -EFAULT;
	}
#endif /* CONFIG_DCACHE */

	data->dma_rx.offset = 0;
	data->dma_rx.buffer = rx_buf;
	data->dma_rx.buffer_length = buf_size;
	data->dma_rx.counter = 0;
	data->dma_rx.timeout = timeout;

	/* Disable RX interrupts to let DMA to handle it */
	USART_ConfigInt(usart, USART_INT_RXDNE, DISABLE);

	data->dma_rx.blk_cfg.block_size = buf_size;
	data->dma_rx.blk_cfg.dest_address = (uint32_t)data->dma_rx.buffer;

	ret = dma_config(data->dma_rx.dma_dev, data->dma_rx.dma_channel,
				&data->dma_rx.dma_cfg);

	if (ret != 0) {
		LOG_ERR("UART ERR: RX DMA config failed!");
		return -EINVAL;
	}

	if (dma_start(data->dma_rx.dma_dev, data->dma_rx.dma_channel)) {
		LOG_ERR("UART ERR: RX DMA start failed!");
		return -EFAULT;
	}

	/* Flush RX data buffer */
	USART_ClrFlag(usart, USART_FLAG_RXDNE);


	/* Enable RX DMA requests */
	uart_n32_dma_rx_enable(dev);

	/* Enable IRQ IDLE to define the end of a
	 * RX DMA transaction.
	 */
	uart_n32_clear_flag(usart);

	USART_ConfigInt(usart, USART_INT_IDLEF, ENABLE);
	USART_ConfigInt(usart, USART_INT_ERRF, ENABLE);

	/* Request next buffer */
	async_evt_rx_buf_request(data);

	LOG_DBG("async rx enabled");

	return ret;
}

static int uart_n32_async_tx_abort(const struct device *dev)
{
	struct uart_n32_data *data = dev->data;
	size_t tx_buffer_length = data->dma_tx.buffer_length;
	struct dma_status stat;

	if (tx_buffer_length == 0) {
		return -EFAULT;
	}

	(void)k_work_cancel_delayable(&data->dma_tx.timeout_work);
	if (!dma_get_status(data->dma_tx.dma_dev,
				data->dma_tx.dma_channel, &stat)) {
		data->dma_tx.counter = tx_buffer_length - stat.pending_length;
	}

	dma_stop(data->dma_tx.dma_dev, data->dma_tx.dma_channel);
	async_evt_tx_abort(data);

	return 0;
}

static void uart_n32_async_rx_timeout(struct k_work *work)
{
	struct k_work_delayable *dwork = k_work_delayable_from_work(work);
	struct uart_dma_stream *rx_stream = CONTAINER_OF(dwork,
			struct uart_dma_stream, timeout_work);
	struct uart_n32_data *data = CONTAINER_OF(rx_stream,
			struct uart_n32_data, dma_rx);
	const struct device *dev = data->uart_dev;

	LOG_DBG("rx timeout");

	if (data->dma_rx.counter == data->dma_rx.buffer_length) {
		uart_n32_async_rx_disable(dev);
	} else {
		uart_n32_dma_rx_flush(dev);
	}
}

static void uart_n32_async_tx_timeout(struct k_work *work)
{
	struct k_work_delayable *dwork = k_work_delayable_from_work(work);
	struct uart_dma_stream *tx_stream = CONTAINER_OF(dwork,
			struct uart_dma_stream, timeout_work);
	struct uart_n32_data *data = CONTAINER_OF(tx_stream,
			struct uart_n32_data, dma_tx);
	const struct device *dev = data->uart_dev;

	uart_n32_async_tx_abort(dev);

	LOG_DBG("tx: async timeout");
}

static int uart_n32_async_rx_buf_rsp(const struct device *dev, uint8_t *buf,
				       size_t len)
{
	struct uart_n32_data *data = dev->data;
	unsigned int key;
	int err = 0;

	LOG_DBG("replace buffer (%d)", len);

	key = irq_lock();

	if (data->rx_next_buffer != NULL) {
		err = -EBUSY;
	} else if (!data->dma_rx.enabled) {
		err = -EACCES;
	} else {
#ifdef CONFIG_DCACHE
		if (!buf_in_nocache((uintptr_t)buf, len)) {
			LOG_ERR("Rx buffer should be placed in a nocache memory region");
			return -EFAULT;
		}
#endif /* CONFIG_DCACHE */
		data->rx_next_buffer = buf;
		data->rx_next_buffer_len = len;
	}

	irq_unlock(key);

	return err;
}

static int uart_n32_async_init(const struct device *dev)
{
	const struct uart_n32_config *config = dev->config;
	USART_Module *usart = UART_MODULE(config);
	struct uart_n32_data *data = dev->data;

	data->uart_dev = dev;

	if (data->dma_rx.dma_dev != NULL) {
		if (!device_is_ready(data->dma_rx.dma_dev)) {
			return -ENODEV;
		}
	}

	if (data->dma_tx.dma_dev != NULL) {
		if (!device_is_ready(data->dma_tx.dma_dev)) {
			return -ENODEV;
		}
	}

	/* Disable both TX and RX DMA requests */
	uart_n32_dma_rx_disable(dev);
	uart_n32_dma_tx_disable(dev);

	k_work_init_delayable(&data->dma_rx.timeout_work,
			    uart_n32_async_rx_timeout);
	k_work_init_delayable(&data->dma_tx.timeout_work,
			    uart_n32_async_tx_timeout);

	/* Configure dma rx config */
	memset(&data->dma_rx.blk_cfg, 0, sizeof(data->dma_rx.blk_cfg));

	data->dma_rx.blk_cfg.source_address = ((uint32_t)&(usart->DAT));


	data->dma_rx.blk_cfg.dest_address = 0; /* dest not ready */

	if (data->dma_rx.src_addr_increment) {
		data->dma_rx.blk_cfg.source_addr_adj = DMA_ADDR_ADJ_INCREMENT;
	} else {
		data->dma_rx.blk_cfg.source_addr_adj = DMA_ADDR_ADJ_NO_CHANGE;
	}

	if (data->dma_rx.dst_addr_increment) {
		data->dma_rx.blk_cfg.dest_addr_adj = DMA_ADDR_ADJ_INCREMENT;
	} else {
		data->dma_rx.blk_cfg.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE;
	}

	/* RX disable circular buffer */
	data->dma_rx.blk_cfg.source_reload_en  = 0;
	data->dma_rx.blk_cfg.dest_reload_en = 0;
	data->dma_rx.blk_cfg.fifo_mode_control = data->dma_rx.fifo_threshold;

	data->dma_rx.dma_cfg.head_block = &data->dma_rx.blk_cfg;
	data->dma_rx.dma_cfg.user_data = (void *)dev;
	data->rx_next_buffer = NULL;
	data->rx_next_buffer_len = 0;

	/* Configure dma tx config */
	memset(&data->dma_tx.blk_cfg, 0, sizeof(data->dma_tx.blk_cfg));

	data->dma_tx.blk_cfg.dest_address = ((uint32_t)&(usart->DAT));


	data->dma_tx.blk_cfg.source_address = 0; /* not ready */

	if (data->dma_tx.src_addr_increment) {
		data->dma_tx.blk_cfg.source_addr_adj = DMA_ADDR_ADJ_INCREMENT;
	} else {
		data->dma_tx.blk_cfg.source_addr_adj = DMA_ADDR_ADJ_NO_CHANGE;
	}

	if (data->dma_tx.dst_addr_increment) {
		data->dma_tx.blk_cfg.dest_addr_adj = DMA_ADDR_ADJ_INCREMENT;
	} else {
		data->dma_tx.blk_cfg.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE;
	}

	data->dma_tx.blk_cfg.fifo_mode_control = data->dma_tx.fifo_threshold;

	data->dma_tx.dma_cfg.head_block = &data->dma_tx.blk_cfg;
	data->dma_tx.dma_cfg.user_data = (void *)dev;

	return 0;
}

#ifdef CONFIG_UART_WIDE_DATA

static int uart_n32_async_tx_u16(const struct device *dev, const uint16_t *tx_data,
				   size_t buf_size, int32_t timeout)
{
	return uart_n32_async_tx(dev, (const uint8_t *)tx_data, buf_size * 2, timeout);
}

static int uart_n32_async_rx_enable_u16(const struct device *dev, uint16_t *buf, size_t len,
					  int32_t timeout)
{
	return uart_n32_async_rx_enable(dev, (uint8_t *)buf, len * 2, timeout);
}

static int uart_n32_async_rx_buf_rsp_u16(const struct device *dev, uint16_t *buf, size_t len)
{
	return uart_n32_async_rx_buf_rsp(dev, (uint8_t *)buf, len * 2);
}

#endif


#endif /* CONFIG_UART_ASYNC_API */


#ifdef CONFIG_UART_INTERRUPT_DRIVEN
static void uart_n32_isr(const struct device *dev);
#endif



static int uart_n32_init(const struct device *dev) {
  const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));
  const struct uart_n32_config *config = DEV_CFG(dev);

  USART_InitType USART_InitStructure;
  pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);

  clock_control_on(clk, (clock_control_subsys_t *)&config->clkid);

  USART_InitStructure.BaudRate = config->uart_config.baudrate;
  USART_InitStructure.WordLength = USART_WL_8B;
  USART_InitStructure.StopBits = USART_STPB_1;
  USART_InitStructure.Parity = USART_PE_NO;
  USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
  USART_InitStructure.Mode = USART_MODE_RX | USART_MODE_TX;

  N32_USART_Initializes(UART_MODULE(config), &USART_InitStructure);
  N32_USART_Enable(UART_MODULE(config));

#if defined(CONFIG_UART_INTERRUPT_DRIVEN) || defined(CONFIG_UART_ASYNC_API)
  config->irq_config_func(dev);
#endif

#ifdef CONFIG_UART_ASYNC_API
	return uart_n32_async_init(dev);
#else
	return 0;
#endif

}

static int uart_n32_poll_in(const struct device *dev, unsigned char *c) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  if (N32_USART_Flag_Status_Get(uart, USART_FLAG_RXDNE) == RESET) {
    return -1;
  }

  *c = N32_USART_Data_Receive(uart) & 0xff;

  return 0;
}

static void uart_n32_poll_out(const struct device *dev, unsigned char c) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Data_Send(uart, c);
  while (N32_USART_Flag_Status_Get(uart, USART_FLAG_TXDE) == RESET) {
    ;
  }
}

static int uart_n32_err_check(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);
  uint32_t reg_val = uart->STS;
  int z_err = 0U;

  z_err = ((reg_val & USART_FLAG_OREF) ? UART_ERROR_OVERRUN : 0) |
          ((reg_val & USART_FLAG_PEF) ? UART_ERROR_PARITY : 0) |
          ((reg_val & USART_FLAG_FEF) ? UART_ERROR_FRAMING : 0);

  N32_USART_Flag_Clear(uart, reg_val & (USART_FLAG_OREF | USART_FLAG_NEF |
                                 USART_FLAG_PEF | USART_FLAG_FEF));

  return (int)z_err;
}

#if  defined(CONFIG_UART_INTERRUPT_DRIVEN) || defined(CONFIG_UART_ASYNC_API)

static void uart_n32_irq_callback_set(const struct device *dev,
                                      uart_irq_callback_user_data_t cb,
                                      void *cb_data) {
  struct uart_n32_data *const dev_data = DEV_DATA(dev);

  dev_data->cb = cb;
  dev_data->cb_data = cb_data;
}

static void uart_n32_isr(const struct device *dev) {
  struct uart_n32_data *const data = DEV_DATA(dev);
#ifdef CONFIG_UART_ASYNC_API
  const struct uart_n32_config *config = dev->config;
  USART_Module *usart = UART_MODULE(config);
#endif
  
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
  if (data->cb) {
    data->cb(dev, data->cb_data);    
  }
#endif /* CONFIG_UART_INTERRUPT_DRIVEN */

#ifdef CONFIG_UART_ASYNC_API
	if (N32_USART_Interrupt_Status_Get(usart, USART_INT_IDLEF) &&
			N32_USART_Flag_Status_Get(usart, USART_FLAG_IDLEF)) {

		uart_n32_clear_flag(usart)

		LOG_DBG("idle interrupt occurred");

		if (data->dma_rx.timeout == 0) {
			uart_n32_dma_rx_flush(dev);
		} else {
			/* Start the RX timer not null */
			async_timer_start(&data->dma_rx.timeout_work,
								data->dma_rx.timeout);
		}
	} else if (N32_USART_Interrupt_Status_Get(usart, USART_INT_TXC) &&
			N32_USART_Flag_Status_Get(usart, USART_FLAG_TXC)) {

		N32_USART_Interrput_Disable(usart, USART_INT_TXC);
		/* Generate TX_DONE event when transmission is done */
		async_evt_tx_done(data);

	} else if (N32_USART_Interrupt_Status_Get(usart, USART_INT_RXDNE) &&
			N32_USART_Flag_Status_Get(usart, USART_FLAG_RXDNE)) {

		/* clear the RXNE flag, because Rx data was not read */
		N32_USART_Flag_Status_Clear(usart, USART_FLAG_RXDNE);

	}

	/* Clear errors */
	uart_n32_err_check(dev);
#endif /* CONFIG_UART_ASYNC_API */

}

static int uart_n32_fifo_fill(const struct device *dev, const uint8_t *tx_data,
                              int size) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);
  uint8_t num_tx = 0U;

  while ((size - num_tx > 0) &&
         (N32_USART_Flag_Status_Get(uart, USART_FLAG_TXDE) != RESET)) {
    N32_USART_Data_Send(uart, tx_data[num_tx++]);
  }

  return num_tx;
}

static int uart_n32_fifo_read(const struct device *dev, uint8_t *rx_data,
                              const int size) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);
  uint8_t num_rx = 0U;

  while ((size - num_rx > 0) && N32_USART_Flag_Status_Get(uart, USART_FLAG_RXDNE)) {
    rx_data[num_rx++] = N32_USART_Data_Receive(uart);
  }

  return num_rx;
}

static void uart_n32_irq_tx_enable(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Interrput_Enable(uart, USART_INT_TXC);
}

static void uart_n32_irq_tx_disable(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Interrput_Disable(uart, USART_INT_TXC);
}

static int uart_n32_irq_tx_ready(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  return (N32_USART_Flag_Status_Get(uart, USART_FLAG_TXDE) != RESET) &&
         (N32_USART_Interrupt_Status_Get(uart, USART_INT_TXC) != RESET);
}

static int uart_n32_irq_tx_complete(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  return (N32_USART_Flag_Status_Get(uart, USART_FLAG_TXC) != RESET);
}

static void uart_n32_irq_rx_enable(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Interrput_Enable(uart, USART_INT_RXDNE);
}

static void uart_n32_irq_rx_disable(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Interrput_Disable(uart, USART_INT_RXDNE);
}

static int uart_n32_irq_rx_ready(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  return (N32_USART_Flag_Status_Get(uart, USART_FLAG_RXDNE) != RESET);
}

static void uart_n32_irq_err_enable(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Interrput_Enable(uart, USART_INT_ERRF);
  N32_USART_Interrput_Enable(uart, USART_INT_PEF);
}

static void uart_n32_irq_err_disable(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  N32_USART_Interrput_Disable(uart, USART_INT_ERRF);
  N32_USART_Interrput_Enable(uart, USART_INT_PEF);
}

static int uart_n32_irq_is_pending(const struct device *dev) {
  const struct uart_n32_config *config = DEV_CFG(dev);
  USART_Module *uart = UART_MODULE(config);

  return (((N32_USART_Interrupt_Status_Get(uart, USART_INT_RXDNE) != RESET) &&
           (N32_USART_Flag_Status_Get(uart, USART_FLAG_RXDNE) != RESET)) ||
          ((N32_USART_Interrupt_Status_Get(uart, USART_INT_TXC) != RESET) &&
           (N32_USART_Flag_Status_Get(uart, USART_FLAG_TXC) != RESET)));
}

#endif /* CONFIG_UART_INTERRUPT_DRIVEN */

static const struct uart_driver_api uart_n32_driver_api = {
    .poll_in = uart_n32_poll_in,
    .poll_out = uart_n32_poll_out,
    .err_check = uart_n32_err_check,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .fifo_fill = uart_n32_fifo_fill,
    .fifo_read = uart_n32_fifo_read,
    .irq_tx_enable = uart_n32_irq_tx_enable,
    .irq_tx_disable = uart_n32_irq_tx_disable,
    .irq_tx_ready = uart_n32_irq_tx_ready,
    .irq_rx_enable = uart_n32_irq_rx_enable,
    .irq_rx_disable = uart_n32_irq_rx_disable,
    .irq_tx_complete = uart_n32_irq_tx_complete,
    .irq_rx_ready = uart_n32_irq_rx_ready,
    .irq_err_enable = uart_n32_irq_err_enable,
    .irq_err_disable = uart_n32_irq_err_disable,
    .irq_is_pending = uart_n32_irq_is_pending,
    .irq_callback_set = uart_n32_irq_callback_set,
#endif /* CONFIG_UART_INTERRUPT_DRIVEN */
#ifdef CONFIG_UART_ASYNC_API
	.callback_set = uart_n32_async_callback_set,
	.tx = uart_n32_async_tx,
	.tx_abort = uart_n32_async_tx_abort,
	.rx_enable = uart_n32_async_rx_enable,
	.rx_disable = uart_n32_async_rx_disable,
	.rx_buf_rsp = uart_n32_async_rx_buf_rsp,
#endif /* CONFIG_UART_ASYNC_API */
};


#ifdef CONFIG_UART_ASYNC_API

/* src_dev and dest_dev should be 'MEMORY' or 'PERIPHERAL'. */
#define UART_DMA_CHANNEL_INIT(index, dir, dir_cap, src_dev, dest_dev)	\
	.dma_dev = DEVICE_DT_GET(N32_DMA_CTLR(index, dir)),			\
	.dma_channel = DT_INST_DMAS_CELL_BY_NAME(index, dir, channel),	\
	.dma_cfg = {							\
		.dma_slot = N32_DMA_SLOT(index, dir, slot),\
		.channel_direction = N32_DMA_CONFIG_DIRECTION(	\
					N32_DMA_CHANNEL_CONFIG(index, dir)),\
		.channel_priority = N32_DMA_CONFIG_PRIORITY(		\
				N32_DMA_CHANNEL_CONFIG(index, dir)),	\
		.source_data_size = N32_DMA_CONFIG_##src_dev##_DATA_SIZE(\
					N32_DMA_CHANNEL_CONFIG(index, dir)),\
		.dest_data_size = N32_DMA_CONFIG_##dest_dev##_DATA_SIZE(\
				N32_DMA_CHANNEL_CONFIG(index, dir)),\
		.source_burst_length = 1, /* SINGLE transfer */		\
		.dest_burst_length = 1,					\
		.block_count = 1,					\
		.dma_callback = uart_n32_dma_##dir##_cb,		\
	},								\
	.src_addr_increment = N32_DMA_CONFIG_##src_dev##_ADDR_INC(	\
				N32_DMA_CHANNEL_CONFIG(index, dir)),	\
	.dst_addr_increment = N32_DMA_CONFIG_##dest_dev##_ADDR_INC(	\
				N32_DMA_CHANNEL_CONFIG(index, dir)),	\
	.fifo_threshold = N32_DMA_FEATURES_FIFO_THRESHOLD(		\
				N32_DMA_FEATURES(index, dir)),		\


#define UART_DMA_CHANNEL(index, dir, DIR, src, dest)			\
.dma_##dir = {								\
	COND_CODE_1(DT_INST_DMAS_HAS_NAME(index, dir),			\
		 (UART_DMA_CHANNEL_INIT(index, dir, DIR, src, dest)),	\
		 (NULL))						\
	},

#else
#define UART_DMA_CHANNEL(index, dir, DIR, src, dest)
#endif

#define UART_N32_DEVICE(idx)                                                   \
  PINCTRL_DT_INST_DEFINE(idx);                                                 \
  IF_ENABLED(CONFIG_UART_INTERRUPT_DRIVEN,                                     \
             (static void uart_n32_cfg_func_##idx(const struct device *dev) {  \
               IF_ENABLED(                                                     \
                   CONFIG_UART_INTERRUPT_DRIVEN,                               \
                   (IRQ_CONNECT(DT_INST_IRQN(idx), DT_INST_IRQ(idx, priority), \
                                uart_n32_isr, DEVICE_DT_INST_GET(idx), 0);     \
                    irq_enable(DT_INST_IRQN(idx))));                           \
             }));                                                              \
  static const struct uart_n32_config uart_n32_dev_cfg_##idx = {               \
      .uart_config =                                                           \
          {                                                                    \
              .baudrate = DT_INST_PROP(idx, current_speed),                    \
              .parity = UART_CFG_PARITY_NONE,                                  \
              .stop_bits = UART_CFG_STOP_BITS_1,                               \
              .data_bits = UART_CFG_DATA_BITS_8,                               \
              .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,                            \
          },                                                                   \
      .base = DT_INST_REG_ADDR(idx),                                           \
      .clkid = DT_INST_CLOCKS_CELL(idx, id),                                   \
      .pcfg = PINCTRL_DT_INST_DEV_CONFIG_GET(idx),                             \
      IF_ENABLED(CONFIG_UART_INTERRUPT_DRIVEN,                                 \
                 (.irq_config_func = uart_n32_cfg_func_##idx, ))};             \
  static struct uart_n32_data uart_n32_dev_data_##idx = {                      \
      IF_ENABLED(CONFIG_UART_INTERRUPT_DRIVEN, (.cb = NULL, ))                 \
        UART_DMA_CHANNEL(idx, rx, RX, PERIPHERAL, MEMORY) 		               \
	    UART_DMA_CHANNEL(idx, tx, TX, MEMORY, PERIPHERAL)		               \
      };                                                                       \
  DEVICE_DT_INST_DEFINE(idx, uart_n32_init, NULL, &uart_n32_dev_data_##idx,    \
                        &uart_n32_dev_cfg_##idx, PRE_KERNEL_1,                 \
                        CONFIG_SERIAL_INIT_PRIORITY,                           \
                        (void *)&uart_n32_driver_api);

DT_INST_FOREACH_STATUS_OKAY(UART_N32_DEVICE);
