/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_H_
#define _SOC_H_

#include <zephyr/devicetree.h>

#include "n32g430.h"

#ifdef CONFIG_N32_HAL_ADC
#include "n32g430_adc.h"
#endif


#ifdef CONFIG_N32_HAL_CAN
#include "n32g430_can.h"
#endif

#ifdef CONFIG_N32_HAL_CRC
#include "n32g430_crc.h"
#endif

#ifdef CONFIG_N32_HAL_DMA
#include "n32g430_dma.h"
#endif

#ifdef CONFIG_N32_HAL_EXTI
#include "n32g430_exti.h"
#endif

#ifdef CONFIG_N32_HAL_FLASH
#include "n32g430_flash.h"
#endif

#ifdef CONFIG_N32_HAL_GPIO
#include "n32g430_gpio.h"
#endif

#ifdef CONFIG_N32_HAL_I2C
#include "n32g430_i2c.h"
#endif

#ifdef CONFIG_N32_HAL_IWDG
#include "n32g430_iwdg.h"
#endif

#ifdef CONFIG_N32_HAL_PWR
#include "n32g430_pwr.h"
#endif

#ifdef CONFIG_N32_HAL_RCC
#include "n32g430_rcc.h"
#endif

#ifdef CONFIG_N32_HAL_RTC
#include "n32g430_rtc.h"
#endif

#ifdef CONFIG_N32_HAL_SPI
#include "n32g430_spi.h"
#endif

#ifdef CONFIG_N32_HAL_TIM
#include "n32g430_tim.h"
#endif

#ifdef CONFIG_N32_HAL_USART
#include "n32g430_usart.h"
#endif

#ifdef CONFIG_N32_HAL_WWDG
#include "n32g430_wwdg.h"
#endif

#define __ASM __asm     /*!< asm keyword for GNU Compiler */
#define __INLINE inline /*!< inline keyword for GNU Compiler */
#define __STATIC_INLINE static inline

/**
\brief Get Link Register
\details Returns the current value of the Link Register (LR).
\return LR Register value
*/
__attribute__((always_inline)) __STATIC_INLINE unsigned int __GET_LR(void) {
  register unsigned int result;

  __ASM volatile("MOV %0, LR\n" : "=r"(result));
  return (result);
}

#endif /* _SOC_H_ */
