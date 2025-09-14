/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_INTERRUPT_CONTROLLER_EXTI_N32_H_
#define ZEPHYR_INCLUDE_DRIVERS_INTERRUPT_CONTROLLER_EXTI_N32_H_

#include <stdint.h>

#include <zephyr/sys/util_macro.h>

#ifdef CONFIG_SOC_SERIES_N32G430
#include <n32g430_exti.h>
#endif

typedef void (*exti_n32_cb_t)(uint8_t line, const struct device *user);

void exti_n32_enable(uint8_t line);

void exti_n32_disable(uint8_t line);

void exti_n32_trigger(uint8_t line, uint8_t trigger);

int  exti_n32_configure(uint8_t line, exti_n32_cb_t cb, const struct device *user);

void exti_n32_force(uint8_t line);

#endif /* ZEPHYR_INCLUDE_DRIVERS_INTERRUPT_CONTROLLER_EXTI_N32_H_ */
