/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/clock_control_n32.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(clock_control_n32, CONFIG_CLOCK_CONTROL_LOG_LEVEL);

#define DT_DRV_COMPAT nsing_n32_rcc


struct n32_clock_control_config {
  uint32_t base;
};

static inline int check_adc_clock(uint32_t bits) {
#if defined(CONFIG_ADC_N32)
  switch(bits)
  {
    case N32_CLOCK_ADC1:
      /* RCC_ADCHCLK_DIV16*/
      ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB,RCC_ADCHCLK_DIV16);
      RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8);  //selsect HSE as RCC ADC1M CLK Source
      break;
    default:
      //do nothing
  } 
#endif
  return 0;
}

static inline int n32_cc_on(const struct device *dev,
                            clock_control_subsys_t sub_system) {
  const struct n32_clock_control_config *config = dev->config;
  uint32_t bits = *(uint32_t *)sub_system;

  sys_set_bit(config->base + GET_CLOCK_ID_OFFSET(bits), GET_CLOCK_ID_BIT(bits));

  check_adc_clock(bits);

  return 0;
}

static inline int n32_cc_off(const struct device *dev,
                             clock_control_subsys_t sub_system) {
  const struct n32_clock_control_config *config = dev->config;
  uint32_t bits = *(uint32_t *)sub_system;

  sys_clear_bit(config->base + GET_CLOCK_ID_OFFSET(bits),
                GET_CLOCK_ID_BIT(bits));

  return 0;
}

static int n32_cc_get_subsys_rate(const struct device *clock,
                                  clock_control_subsys_t sub_system,
                                  uint32_t *rate) {
  int rc = 0;
  uint32_t bits = *(uint32_t *)sub_system;

  RCC_ClocksType RCC_ClockFreq;
  RCC_Clocks_Frequencies_Value_Get(&RCC_ClockFreq);

  switch (GET_CLOCK_ID_OFFSET(bits)) {
    case N32_AHBPCLKEN_OFFSET:
      *rate = RCC_ClockFreq.HclkFreq;
      break;
    case N32_APB2PCLKEN_OFFSET:
      *rate = RCC_ClockFreq.Pclk2Freq;
      break;
    case N32_APB1PCLKEN_OFFSET:
      *rate = RCC_ClockFreq.Pclk1Freq;
      break;
    default:
      rc = -EINVAL;
      break;
  }

  return rc;
}

static const struct clock_control_driver_api n32_cc_api = {
    .on = n32_cc_on,
    .off = n32_cc_off,
    .get_rate = n32_cc_get_subsys_rate,
};

static int n32_cc_init(const struct device *dev) { return 0; }

static const struct n32_clock_control_config n32_cc_cfg = {
    .base = DT_INST_REG_ADDR(0),
};

/**
 * @brief Clock Control device init
 *
 */
DEVICE_DT_INST_DEFINE(0, &n32_cc_init, NULL, NULL, &n32_cc_cfg, PRE_KERNEL_1,
                      CONFIG_CLOCK_CONTROL_INIT_PRIORITY, &n32_cc_api);
