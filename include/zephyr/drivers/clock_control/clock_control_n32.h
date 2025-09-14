/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_CLOCK_CONTROL_CLOCK_CONTROL_N32_H_
#define ZEPHYR_DRIVERS_CLOCK_CONTROL_CLOCK_CONTROL_N32_H_


#define GET_CLOCK_ID_OFFSET(id) (((id) >> 6U) & 0xFFU)
#define GET_CLOCK_ID_BIT(id) ((id) & 0x1FU)

#ifdef CONFIG_SOC_SERIES_N32G430
#include <zephyr/dt-bindings/clock/n32g430-clock.h>

#define N32_RCC_AHB_Peripheral_Clock_Enable(x)          RCC_AHB_Peripheral_Clock_Enable(x)
#define N32_RCC_AHB_Peripheral_Clock_Disable(x)         RCC_AHB_Peripheral_Clock_Disable(x)
#define N32_RCC_APB2_Peripheral_Clock_Enable(x)         RCC_APB2_Peripheral_Clock_Enable(x)
#define N32_RCC_APB2_Peripheral_Clock_Disable(x)        RCC_APB2_Peripheral_Clock_Disable(x)
#define N32_RCC_APB1_Peripheral_Clock_Enable(x)         RCC_APB1_Peripheral_Clock_Enable(x)
#define N32_RCC_APB1_Peripheral_Clock_Disable(x)        RCC_APB1_Peripheral_Clock_Disable(x)
#endif

#endif /* ZEPHYR_INCLUDE_DRIVERS_CLOCK_CONTROL_N32_CLOCK_CONTROL_H_ */
