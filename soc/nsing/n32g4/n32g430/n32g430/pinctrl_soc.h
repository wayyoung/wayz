/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _PINCTRL_SOC_H_
#define _PINCTRL_SOC_H_

#include <stdint.h>
#include <zephyr/devicetree.h>
#include <zephyr/types.h>

struct pinctrl_n32_pin {
  uint32_t config;
  uint32_t remap;
};

typedef struct pinctrl_n32_pin pinctrl_soc_pin_t;

#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx) \
  {                                                  \
      .config = DT_PROP_BY_IDX(node_id, prop, idx),  \
      .remap = DT_PROP(node_id, remap),          \
  },

/**
 * @brief Utility macro to initialize state pins contained in a given property.
 *
 * @param node_id Node identifier.
 * @param prop Property name describing state pins.
 */
#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                           \
  {DT_FOREACH_CHILD_VARGS(DT_PHANDLE(node_id, prop), DT_FOREACH_PROP_ELEM, \
                          pinmux, Z_PINCTRL_STATE_PIN_INIT)}

#endif