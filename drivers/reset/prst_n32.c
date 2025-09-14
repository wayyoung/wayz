/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/reset.h>
#include <zephyr/drivers/clock_control/clock_control_n32.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(prst_n32);

#define DT_DRV_COMPAT nsing_n32_prst

#define GETSHIFT(x) (__builtin_ffsll(x) - 1)

struct prst_n32_config {
	uint32_t base;
};

static int prst_n32_status(const struct device *dev, uint32_t id,
			     uint8_t *status)
{
	(void)dev;
    uint32_t id_bit = 0;

	if(id == 0)
	{
		return  -EINVAL;	
	}

    id_bit = 1<< GET_CLOCK_ID_BIT(id);


	if (N32_AHBPCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        *status = (uint8_t)((RCC->AHBPRST & id_bit) >> GETSHIFT(id_bit));
    }
    else  if (N32_APB1PCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        *status = (uint8_t)((RCC->APB1PRST & id_bit) >> GETSHIFT(id_bit));
    }
    else if (N32_APB2PCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        *status = (uint8_t)((RCC->APB2PRST & id_bit) >> GETSHIFT(id_bit));
    }
    else
    {
    	LOG_ERR("Invalid RCC id: %d to do status", id);
		return  -EINVAL;
    }
	  
	return 0;
}

static int prst_n32_line_assert(const struct device *dev, uint32_t id)
{
	(void)dev;
    uint32_t id_bit = 0;

	if(id == 0)
	{
		return  -EINVAL;	
	}

    id_bit = 1<< GET_CLOCK_ID_BIT(id);

    if (N32_AHBPCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        RCC_AHB_Peripheral_Reset(id_bit);
    }
    else  if (N32_APB1PCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        RCC_APB1_Peripheral_Reset(id_bit);
    }
    else if (N32_APB2PCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        RCC_APB2_Peripheral_Reset(id_bit);
    }
    else
    {
    	LOG_ERR("Invalid RCC id: %d to do line_assert", id);
		return  -EINVAL;
    }

	return 0;
}

static int prst_n32_line_deassert(const struct device *dev, uint32_t id)
{
#ifdef CONFIG_SOC_SERIES_N32G4FR
	(void)dev;
	uint32_t id_bit = 0;

	if(id == 0)
	{
		return  -EINVAL;	
	}

    id_bit = 1<< GET_CLOCK_ID_BIT(id);

    if (N32_AHBPCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        RCC_EnableAHBPeriphReset(id_bit, DISABLE);
    }
    else  if (N32_APB1PCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        RCC_EnableAPB1PeriphReset(id_bit, DISABLE);
    }
    else if (N32_APB2PCLKEN_OFFSET == GET_CLOCK_ID_OFFSET(id))
    {
        RCC_EnableAPB2PeriphReset(id_bit, DISABLE);
    }
    else
    {
    	LOG_ERR("Invalid RCC id: %d to do line_deassert", id);
		return  -EINVAL;
    }
#endif
	return 0;;
}

static int prst_n32_line_toggle(const struct device *dev, uint32_t id)
{
	(void)prst_n32_line_assert(dev, id);
	(void)prst_n32_line_deassert(dev, id);

	return 0;
}

static const struct reset_driver_api prst_n32_driver_api = {
	.status = prst_n32_status,
	.line_assert = prst_n32_line_assert,
	.line_deassert = prst_n32_line_deassert,
	.line_toggle = prst_n32_line_toggle,
};

static const struct prst_n32_config config = {
	.base = DT_REG_ADDR(DT_INST_PARENT(0)),
};

DEVICE_DT_INST_DEFINE(0, NULL, NULL, NULL, &config, PRE_KERNEL_1,
		      CONFIG_RESET_INIT_PRIORITY, &prst_n32_driver_api);
