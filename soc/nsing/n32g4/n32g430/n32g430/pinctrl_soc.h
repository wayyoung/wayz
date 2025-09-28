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
  uint32_t func;
  uint32_t remap;
};

typedef struct pinctrl_n32_pin pinctrl_soc_pin_t;

#define DT_PINMUXS_CONFIG_BY_IDX(node_id, pinmuxs_prop, idx) \
	DT_PHA_BY_IDX(node_id, pinmuxs_prop, idx, config)

#define DT_PINMUXS_FUNC_BY_IDX(node_id, pinmuxs_prop, idx) \
	DT_PHA_BY_IDX(node_id, pinmuxs_prop, idx, func)


#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx)                 \
  {                                                                  \
      .config = DT_PINMUXS_CONFIG_BY_IDX(node_id, prop, idx),              \
      .func = DT_PINMUXS_FUNC_BY_IDX(node_id, prop, idx),              \
      .remap = DT_PROP(node_id, remap),                   \
  },

#define Z_PINCTRL_GROUP_PINS_INIT(child_id) \
	DT_FOREACH_PROP_ELEM(child_id, pinmuxs, Z_PINCTRL_STATE_PIN_INIT)

#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                         \
  {                                                                      \
    DT_FOREACH_CHILD(DT_PHANDLE(node_id, prop), Z_PINCTRL_GROUP_PINS_INIT) \
  }

#endif
