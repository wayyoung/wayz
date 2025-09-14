#ifndef _N32G430_GPIO_REMAP_H_
#define _N32G430_GPIO_REMAP_H_

/* Copy from the n32g430_gpio.h and change type*/

/** Alternate function AF0 **/
#define N32_ALTERNATE_FUN_0 (0x00U)
#define N32_GPIO_AF0_SWDIO_JTMS (N32_ALTERNATE_FUN_0) /* SWDIO Alternate Function mapping    */
#define N32_GPIO_AF0_SWCLK_JTCK (N32_ALTERNATE_FUN_0) /* SWCLK Alternate Function mapping    */
#define N32_GPIO_AF0_JTDI (N32_ALTERNATE_FUN_0)       /* JTDI  Alternate Function mapping    */
#define N32_GPIO_AF0_JTDO (N32_ALTERNATE_FUN_0)       /* JTDO  Alternate Function mapping    */
#define N32_GPIO_AF0_NJRST (N32_ALTERNATE_FUN_0)      /* NJRST Alternate Function mapping    */

/** Alternate function AF1 **/
#define N32_ALTERNATE_FUN_1 (0x01U)
#define N32_GPIO_AF1_SPI1 (N32_ALTERNATE_FUN_1)   /* SPI1 Alternate Function mapping     */
#define N32_GPIO_AF1_SPI2 (N32_ALTERNATE_FUN_1)   /* SPI2 Alternate Function mapping     */
#define N32_GPIO_AF1_TIM8 (N32_ALTERNATE_FUN_1)   /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF1_USART1 (N32_ALTERNATE_FUN_1) /* USART1 Alternate Function mapping   */

/** Alternate function AF2 **/
#define N32_ALTERNATE_FUN_2 (0x02U)
#define N32_GPIO_AF2_TIM5 (N32_ALTERNATE_FUN_2)   /* TIM5 Alternate Function mapping     */
#define N32_GPIO_AF2_TIM1 (N32_ALTERNATE_FUN_2)   /* TIM1 Alternate Function mapping     */
#define N32_GPIO_AF2_USART1 (N32_ALTERNATE_FUN_2) /* USART1 Alternate Function mapping   */
#define N32_GPIO_AF2_I2C2 (N32_ALTERNATE_FUN_2)   /* I2C2 Alternate Function mapping     */
#define N32_GPIO_AF2_CAN (N32_ALTERNATE_FUN_2)    /* CAN Alternate Function mapping      */
#define N32_GPIO_AF2_SPI2 (N32_ALTERNATE_FUN_2)   /* SPI2 Alternate Function mapping     */
#define N32_GPIO_AF2_SPI1 (N32_ALTERNATE_FUN_2)   /* SPI1 Alternate Function mapping     */
#define N32_GPIO_AF2_I2C1 (N32_ALTERNATE_FUN_2)   /* I2C1 Alternate Function mapping     */

/** Alternate function AF3 **/
#define N32_ALTERNATE_FUN_3 (0x03U)
#define N32_GPIO_AF3_TIM2 (N32_ALTERNATE_FUN_3)  /* TIM2 Alternate Function mapping     */
#define N32_GPIO_AF3_SPI2 (N32_ALTERNATE_FUN_3)  /* SPI2 Alternate Function mapping     */
#define N32_GPIO_AF3_TIM3 (N32_ALTERNATE_FUN_3)  /* TIM3 Alternate Function mapping     */
#define N32_GPIO_AF3_TIM1 (N32_ALTERNATE_FUN_3)  /* TIM1 Alternate Function mapping     */
#define N32_GPIO_AF3_LPTIM (N32_ALTERNATE_FUN_3) /* LPTIM Alternate Function mapping    */
#define N32_GPIO_AF3_TIM4 (N32_ALTERNATE_FUN_3)  /* TIM4 Alternate Function mapping     */

/** Alternate function AF4 **/
#define N32_ALTERNATE_FUN_4 (0x04U)
#define N32_GPIO_AF4_EVENTOUT (N32_ALTERNATE_FUN_4) /* EVENTOUT Alternate Function mapping */
#define N32_GPIO_AF4_USART2 (N32_ALTERNATE_FUN_4)   /* USART2 Alternate Function mapping   */

/** Alternate function AF5 **/
#define N32_ALTERNATE_FUN_5 (0x05U)
#define N32_GPIO_AF5_USART2 (N32_ALTERNATE_FUN_5) /* USART2 Alternate Function mapping   */
#define N32_GPIO_AF5_USART1 (N32_ALTERNATE_FUN_5) /* USART1 Alternate Function mapping   */
#define N32_GPIO_AF5_SPI1 (N32_ALTERNATE_FUN_5)   /* SPI1 Alternate Function mapping     */
#define N32_GPIO_AF5_TIM3 (N32_ALTERNATE_FUN_5)   /* TIM3 Alternate Function mapping     */
#define N32_GPIO_AF5_SPI2 (N32_ALTERNATE_FUN_5)   /* SPI2 Alternate Function mapping     */
#define N32_GPIO_AF5_I2C1 (N32_ALTERNATE_FUN_5)   /* I2C1 Alternate Function mapping     */

/** Alternate function AF6 **/
#define N32_ALTERNATE_FUN_6 (0x06U)
#define N32_GPIO_AF6_TIM2 (N32_ALTERNATE_FUN_6)  /* TIM2 Alternate Function mapping     */
#define N32_GPIO_AF6_I2C2 (N32_ALTERNATE_FUN_6)  /* I2C2 Alternate Function mapping     */
#define N32_GPIO_AF6_TIM4 (N32_ALTERNATE_FUN_6)  /* TIM4 Alternate Function mapping     */
#define N32_GPIO_AF6_TIM1 (N32_ALTERNATE_FUN_6)  /* TIM1 Alternate Function mapping     */
#define N32_GPIO_AF6_SPI1 (N32_ALTERNATE_FUN_6)  /* SPI1 Alternate Function mapping     */
#define N32_GPIO_AF6_SPI2 (N32_ALTERNATE_FUN_6)  /* SPI2 Alternate Function mapping     */
#define N32_GPIO_AF6_LPTIM (N32_ALTERNATE_FUN_6) /* LPTIM Alternate Function mapping    */
#define N32_GPIO_AF6_CAN (N32_ALTERNATE_FUN_6)   /* CAN Alternate Function mapping      */
#define N32_GPIO_AF6_UART4 (N32_ALTERNATE_FUN_6) /* UART4 Alternate Function mapping    */

/** Alternate function AF7 **/
#define N32_ALTERNATE_FUN_7 (0x07U)
#define N32_GPIO_AF7_TIM5 (N32_ALTERNATE_FUN_7)   /* TIM5 Alternate Function mapping     */
#define N32_GPIO_AF7_TIM8 (N32_ALTERNATE_FUN_7)   /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF7_I2C2 (N32_ALTERNATE_FUN_7)   /* I2C2 Alternate Function mapping     */
#define N32_GPIO_AF7_USART2 (N32_ALTERNATE_FUN_7) /* USART2 Alternate Function mapping   */
#define N32_GPIO_AF7_UART4 (N32_ALTERNATE_FUN_7)  /* UART4 Alternate Function mapping    */
#define N32_GPIO_AF7_UART3 (N32_ALTERNATE_FUN_7)  /* UART3 Alternate Function mapping    */
#define N32_GPIO_AF7_TIM1 (N32_ALTERNATE_FUN_7)   /* TIM1 Alternate Function mapping     */
#define N32_GPIO_AF7_I2C1 (N32_ALTERNATE_FUN_7)   /* I2C1 Alternate Function mapping     */

/** Alternate function AF8 **/
#define N32_ALTERNATE_FUN_8 (0x08U)
#define N32_GPIO_AF8_TIM8 (N32_ALTERNATE_FUN_8)  /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF8_TIM5 (N32_ALTERNATE_FUN_8)  /* TIM5 Alternate Function mapping     */
#define N32_GPIO_AF8_COMP2 (N32_ALTERNATE_FUN_8) /* COMP2 Alternate Function mapping    */
#define N32_GPIO_AF8_I2C1 (N32_ALTERNATE_FUN_8)  /* I2C1 Alternate Function mapping     */
#define N32_GPIO_AF8_TIM1 (N32_ALTERNATE_FUN_8)  /* TIM1 Alternate Function mapping     */
#define N32_GPIO_AF8_COMP1 (N32_ALTERNATE_FUN_8) /* COMP1 Alternate Function mapping    */
#define N32_GPIO_AF8_UART3 (N32_ALTERNATE_FUN_8) /* UART3 Alternate Function mapping    */

/** Alternate function AF9 **/
#define N32_ALTERNATE_FUN_9 (0x09U)
#define N32_GPIO_AF9_COMP1 (N32_ALTERNATE_FUN_9)     /* COMP1 Alternate Function mapping    */
#define N32_GPIO_AF9_TIM8 (N32_ALTERNATE_FUN_9)      /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF9_TIM4 (N32_ALTERNATE_FUN_9)      /* TIM4 Alternate Function mapping     */
#define N32_GPIO_AF9_MCO (N32_ALTERNATE_FUN_9)       /* MCO Alternate Function mapping      */
#define N32_GPIO_AF9_RTC (N32_ALTERNATE_FUN_9)       /* RTC Alternate Function mapping      */
#define N32_GPIO_AF9_COMP2 (N32_ALTERNATE_FUN_9)     /* COMP2 Alternate Function mapping    */
#define N32_GPIO_AF9_LPTIM (N32_ALTERNATE_FUN_9)     /* LPTIM Alternate Function mapping    */
#define N32_GPIO_AF9_I2C2 (N32_ALTERNATE_FUN_9)      /* I2C2 Alternate Function mapping     */
#define N32_GPIO_AF9_TIMESTAMP (N32_ALTERNATE_FUN_9) /* TIMESTAMP Alternate Function mapping */

/** Alternate function AF10 **/
#define N32_ALTERNATE_FUN_10 (0x0AU)
#define N32_GPIO_AF10_TIM1 (N32_ALTERNATE_FUN_10)  /* TIM1 Alternate Function mapping     */
#define N32_GPIO_AF10_TIM8 (N32_ALTERNATE_FUN_10)  /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF10_COMP3 (N32_ALTERNATE_FUN_10) /* COMP3 Alternate Function mapping    */
#define N32_GPIO_AF10_TIM4 (N32_ALTERNATE_FUN_10)  /* TIM4 Alternate Function mapping     */
#define N32_GPIO_AF10_COMP1 (N32_ALTERNATE_FUN_10) /* COMP1 Alternate Function mapping    */
#define N32_GPIO_AF10_UART3 (N32_ALTERNATE_FUN_10) /* UART3 Alternate Function mapping    */

/** Alternate function AF11 **/
#define N32_ALTERNATE_FUN_11 (0x0BU)
#define N32_GPIO_AF11_UART4 (N32_ALTERNATE_FUN_11)  /* UART4 Alternate Function mapping    */
#define N32_GPIO_AF11_CAN (N32_ALTERNATE_FUN_11)    /* CAN Alternate Function mapping      */
#define N32_GPIO_AF11_USART2 (N32_ALTERNATE_FUN_11) /* USART2 Alternate Function mapping   */
#define N32_GPIO_AF11_TIM8 (N32_ALTERNATE_FUN_11)   /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF11_COMP3 (N32_ALTERNATE_FUN_11)  /* COMP3 Alternate Function mapping    */

/** Alternate function AF12 **/
#define N32_ALTERNATE_FUN_12 (0x0CU)
#define N32_GPIO_AF12_LPTIM (N32_ALTERNATE_FUN_12)  /* LPTIM Alternate Function mapping    */
#define N32_GPIO_AF12_BEEPER (N32_ALTERNATE_FUN_12) /* BEEPER Alternate Function mapping   */

/** Alternate function AF13 **/
#define N32_ALTERNATE_FUN_13 (0x0DU)
#define N32_GPIO_AF13_TIM3 (N32_ALTERNATE_FUN_13) /* TIM3 Alternate Function mapping     */
#define N32_GPIO_AF13_TIM8 (N32_ALTERNATE_FUN_13) /* TIM8 Alternate Function mapping     */
#define N32_GPIO_AF13_TIM4 (N32_ALTERNATE_FUN_13) /* TIM4 Alternate Function mapping     */
#define N32_GPIO_AF13_TIM5 (N32_ALTERNATE_FUN_13) /* TIM5 Alternate Function mapping     */

/** Alternate function AF15 **/
#define N32_ALTERNATE_FUN_15 (0x0FU) /* NON Alternate Function mapping     */

#define N32_GPIO_NO_AF (N32_ALTERNATE_FUN_15)
#endif