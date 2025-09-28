/*
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _N32G430_PINCTRL_H_
#define _N32G430_PINCTRL_H_


#define N32_PORT_A 0x0
#define N32_PORT_B 0x1
#define N32_PORT_C 0x2
#define N32_PORT_D 0x3


/* Remap the GPIO_ModeType from n32g430_gpio.h */
#define PIN_MODE_AIN  			0
#define	PIN_MODE_IN_FLOATING 	1
#define	PIN_MODE_IPD 			2
#define	PIN_MODE_IPU  			3
#define	PIN_MODE_Out_OD			4
#define	PIN_MODE_Out_PP  		5
#define	PIN_MODE_AF_OD  		6
#define	PIN_MODE_AF_PP 			7


/*
	Pinmux:
	  - bit[0:3] PIN
	  - bit[4:7] PORT
	  - bit[12:15] PIN MODE
	  - bit[16:19] PIN SPEED
 */

#define PIN_NUM_OFFSET 		0   //4
#define PORT_NUM_OFFSET 	4	//4
#define PIN_MODE_OFFSET 	12	//4
#define UNUSED_OFFSET 		20	//12



#define N32_PINMUX(port, pin, FUNC, pin_mode) (((port) << PORT_NUM_OFFSET) | ((pin) << PIN_NUM_OFFSET) | ((pin_mode) << PIN_MODE_OFFSET)) (FUNC)

#define N32_PINMUX_GET_PIN(pinmux) ((pinmux->config >> PIN_NUM_OFFSET) & 0xF)
#define N32_PINMUX_GET_PORT(pinmux) ((pinmux->config >> PORT_NUM_OFFSET) & 0xF)
#define N32_PINMUX_GET_FUNC(pinmux) (pinmux->func)
#define N32_PINMUX_GET_PIN_MODE(pinmux) ((pinmux->config >> PIN_MODE_OFFSET) & 0xF)
#define N32_PINMUX_GET_PORT_PIN(pinmux) ((pinmux->config >> PIN_NUM_OFFSET) & 0xFF)

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
	N32_PINMUX(N32_PORT_A, 4, N32_GPIO_AF2_USART1, PIN_MODE_AF_PP)

#define USART1_TX_PA9 \
	N32_PINMUX(N32_PORT_A, 9, N32_GPIO_AF5_USART1, PIN_MODE_AF_PP)

/* USART1_RX */
#define USART1_RX_PA5 \
	N32_PINMUX(N32_PORT_A, 5, N32_GPIO_AF5_USART1, PIN_MODE_IN_FLOATING)

#define USART1_RX_PA10 \
	N32_PINMUX(N32_PORT_A, 10, N32_GPIO_AF5_USART1, PIN_MODE_IN_FLOATING)

/*I2C1*/
#define I2C1_SCL_PB6 \
	N32_PINMUX(N32_PORT_B, 6, N32_GPIO_AF2_I2C1, PIN_MODE_AF_OD)

#define I2C1_SDA_PB7 \
	N32_PINMUX(N32_PORT_B, 7, N32_GPIO_AF2_I2C1, PIN_MODE_AF_OD)

/*I2C2*/
#define I2C2_SCL_PA9 \
	N32_PINMUX(N32_PORT_A, 9, N32_GPIO_AF7_I2C2, PIN_MODE_AF_OD)

#define I2C2_SDA_PA10 \
	N32_PINMUX(N32_PORT_A, 10, N32_GPIO_AF7_I2C2, PIN_MODE_AF_OD)

#endif /* ZEPHYR_N32_PINCTRL_COMMON_H_ */
