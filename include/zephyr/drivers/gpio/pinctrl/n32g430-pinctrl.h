/*
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _N32G430_PINCTRL_H_
#define _N32G430_PINCTRL_H_


#define N32_PORT_A 0x0
#define N32_PORT_B 0x1
#define N32_PORT_C 0x2
#define N32_PORT_D 0x3


#define N32_ALT_FUNC_0 0x0
#define N32_ALT_FUNC_1 0x1 //USART
#define N32_ALT_FUNC_2 0x2 //USB
#define N32_ALT_FUNC_3 0x3
#define N32_ALT_FUNC_4 0x4
#define N32_ALT_FUNC_5 0x5
#define N32_ALT_FUNC_6 0x6
#define N32_ALT_FUNC_8 0x8
#define N32_ALT_FUNC_9 0x9
#define N32_ALT_FUNC_A 0xA

// ==== ADC
// ==== TIMx
// ==== CAN
// ==== DVP
// ==== USART
// ==== I2C
// ==== SPI


/* Remap the GPIO_ModeType from n32g430_gpio.h */
#define PIN_MODE_AIN  			0
#define	PIN_MODE_IN_FLOATING 	1
// #define	PIN_MODE_IPD 		2
// #define	PIN_MODE_IPU  		3
#define	PIN_MODE_Out_OD			4
#define	PIN_MODE_Out_PP  		5
#define	PIN_MODE_AF_OD  		6
#define	PIN_MODE_AF_PP 			7


/*
	Pinmux:
	  - bit[0:3] PIN
	  - bit[4:7] PORT
	  - bit[8:11] FUNC
	  - bit[12:15] PIN MODE
	  - bit[16:19] PIN SPEED
 */


#define N32_PINMUX(port, pin, func, pin_mode, pin_speed) (((func) << FUNC_NUM_OFFSET) | ((port) << PORT_NUM_OFFSET) | ((pin) << PIN_NUM_OFFSET) | ((pin_mode) << PIN_MODE_OFFSET) | ((pin_speed) << PIN_SPEED_OFFSET))

#define N32_PINMUX_GET_PIN(pinmux) ((pinmux >> PIN_NUM_OFFSET) & 0xF)
#define N32_PINMUX_GET_PORT(pinmux) ((pinmux >> PORT_NUM_OFFSET) & 0xF)
#define N32_PINMUX_GET_FUNC(pinmux) ((pinmux >> FUNC_NUM_OFFSET) & 0xF)
#define N32_PINMUX_GET_PIN_MODE(pinmux) ((pinmux >> PIN_MODE_OFFSET) & 0xF)
#define N32_PINMUX_GET_PIN_SPEED(pinmux) ((pinmux >> PIN_SPEED_OFFSET) & 0xF)
#define N32_PINMUX_GET_PORT_PIN(pinmux) ((pinmux >> PIN_NUM_OFFSET) & 0xFF)

#define N32_PINMUX_DT_INST_GET_ELEM(idx, x, inst) \
	DT_PROP(DT_INST_PINCTRL_BY_IDX(inst, x, idx), pinmux),

#define N32_PINMUX_DT_INST_GET_ARRAY(inst, x)                        \
	{COND_CODE_1(DT_INST_PINCTRL_HAS_IDX(inst, x),                   \
				 (UTIL_LISTIFY(DT_INST_NUM_PINCTRLS_BY_IDX(inst, x), \
							   N32_PINMUX_DT_INST_GET_ELEM,          \
							   x,                                    \
							   inst)),                               \
				 ())}

/* USART1_TX */
#define USART1_TX_PA4 \
	N32_PINMUX(N32_PORT_A, 4, N32_ALT_FUNC_0, PIN_MODE_AF_PP)

/* USART1_RX */
#define USART1_RX_PA5 \
	N32_PINMUX(N32_PORT_A, 5, N32_ALT_FUNC_0, PIN_MODE_IN_FLOATING)

/* I2C1_SCL */
#define I2C1_SCL_PB6 \
	N32_PINMUX(N32_PORT_B, 6, N32_ALT_FUNC_0, PIN_MODE_AF_OD)

/* I2C1_SDA */
#define I2C1_SDA_PB7 \
	N32_PINMUX(N32_PORT_B, 7, N32_ALT_FUNC_0, PIN_MODE_AF_OD)


#endif /* ZEPHYR_N32_PINCTRL_COMMON_H_ */
