/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/clock_control_n32.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/pinctrl/pinctrl_n32.h>

#define DT_DRV_COMPAT nsing_n32_pinctrl

static int map_gpio_mode_from_pin_mode(uint8_t pin_mode) {
  switch (pin_mode) {
    case PIN_MODE_IN_FLOATING:
      return GPIO_MODE_INPUT;
    case PIN_MODE_Out_OD:
      return GPIO_MODE_OUT_OD;
    case PIN_MODE_Out_PP:
      return GPIO_MODE_OUT_PP;
    case PIN_MODE_AF_OD:
      return GPIO_MODE_AF_OD;
    case PIN_MODE_AF_PP:
      return GPIO_MODE_AF_PP;
    default:
      return GPIO_MODE_ANALOG;
  }
  return GPIO_MODE_ANALOG;
}

static int pinctrl_n32_set(const pinctrl_soc_pin_t *soc_pin) {

  GPIO_InitType GPIO_InitStructure;
  GPIO_Module *gpio = NULL;
  uint32_t port = N32_PINMUX_GET_PORT(soc_pin);
  uint32_t pin = N32_PINMUX_GET_PIN(soc_pin);
  uint32_t pin_func = N32_PINMUX_GET_FUNC(soc_pin);
  uint32_t pin_mode = N32_PINMUX_GET_PIN_MODE(soc_pin);
  

  GPIO_Structure_Initialize(&GPIO_InitStructure);

  switch (port) {
    case N32_PORT_A:
      gpio = GPIOA;
      break;

    case N32_PORT_B:
      gpio = GPIOB;
      break;

    case N32_PORT_C:
      gpio = GPIOC;
      break;

    case N32_PORT_D:
      gpio = GPIOD;
      break;

    default:
      return -1;
  }

  GPIO_InitStructure.Pin = (1 << pin);
  GPIO_InitStructure.GPIO_Mode = map_gpio_mode_from_pin_mode(pin_mode);
  GPIO_InitStructure.GPIO_Alternate = pin_func;
  
  if (pin_func != N32_GPIO_NO_AF) {
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);
    GPIO_Pin_Remap_Set(port, pin, pin_func);
  }

  GPIO_Peripheral_Initialize(gpio, &GPIO_InitStructure);

  return 0;
}

int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt,
                           uintptr_t reg) {
  for (int i = 0; i < pin_cnt; i++) {
    pinctrl_n32_set(&pins[i]);
  }

  return 0;
}

static int pinctrl_n32_init() {
  uint32_t clk_bit = N32_CLOCK_AFIO;
  const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(rcc));

  clock_control_on(clk, (clock_control_subsys_t *)&clk_bit);
  // @TODO: add a remap array to deal with the remap
  // GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_NO_NJTRST, ENABLE);

  return 0;
}

SYS_INIT(pinctrl_n32_init, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
