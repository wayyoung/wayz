/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADC_N32_H
#define _ADC_N32_H

#include <zephyr/device.h>

#ifdef CONFIG_SOC_SERIES_N32G430
#include <n32g430_adc.h>
#define ADC_VREF_INTERNAL_3V3 3300
#endif

#endif