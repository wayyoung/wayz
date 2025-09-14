#ifndef _N32G430_GPIO_REMAP_H_
#define _N32G430_GPIO_REMAP_H_

/* Copy from the n32g4fr_gpio.h and change type*/

#define N32_GPIO_RMP_SPI1              (0x00000001U) /*!< SPI1 Alternate Function mapping */
#define N32_GPIO_RMP_I2C1              (0x00000002U) /*!< I2C1 Alternate Function mapping */
#define N32_GPIO_RMP_USART1            (0x00000004U) /*!< USART1 Alternate Function mapping */
#define N32_GPIO_RMP_USART2            (0x00000008U) /*!< USART2 Alternate Function mapping */
#define N32_GPIO_PART_RMP_USART3       (0x00140010U) /*!< USART3 Partial Alternate Function mapping */
#define N32_GPIO_ALL_RMP_USART3        (0x00140030U) /*!< USART3 Full Alternate Function mapping */
#define N32_GPIO_PART1_RMP_TIM1        (0x00160040U) /*!< TIM1 Partial Alternate Function mapping */
#define N32_GPIO_PART2_RMP_TIM1        (0x00160080U) /*!< TIM1 Partial Alternate Function mapping */
#define N32_GPIO_ALL_RMP_TIM1          (0x001600C0U) /*!< TIM1 Full Alternate Function mapping */
#define N32_GPIO_PartialRemap1_TIM2    (0x00180100U) /*!< TIM2 Partial1 Alternate Function mapping */
#define N32_GPIO_PART2_RMP_TIM2        (0x00180200U) /*!< TIM2 Partial2 Alternate Function mapping */
#define N32_GPIO_ALL_RMP_TIM2          (0x00180300U) /*!< TIM2 Full Alternate Function mapping */
#define N32_GPIO_PART1_RMP_TIM3        (0x001A0800U) /*!< TIM3 Partial Alternate Function mapping */
#define N32_GPIO_ALL_RMP_TIM3          (0x001A0C00U) /*!< TIM3 Full Alternate Function mapping */
#define N32_GPIO_RMP_TIM4              (0x00001000U) /*!< TIM4 Alternate Function mapping */
#define N32_GPIO_RMP1_CAN1             (0x001D2000U) /*!< CAN1 Alternate Function mapping */
#define N32_GPIO_RMP2_CAN1             (0x001D4000U) /*!< CAN1 Alternate Function mapping */
#define N32_GPIO_RMP3_CAN1             (0x001D6000U) /*!< CAN1 Alternate Function mapping */
#define N32_GPIO_RMP_PD01              (0x00008000U) /*!< PD01 Alternate Function mapping */
#define N32_GPIO_RMP_TIM5CH4           (0x00200001U) /*!< LSI connected to TIM5 Channel4 input capture for calibration */
#define N32_GPIO_RMP_ADC1_ETRI         (0x00200002U) /*!< ADC1 External Trigger Injected Conversion remapping */
#define N32_GPIO_RMP_ADC1_ETRR         (0x00200004U) /*!< ADC1 External Trigger Regular Conversion remapping */
#define N32_GPIO_RMP_ADC2_ETRI         (0x00200008U) /*!< ADC2 External Trigger Injected Conversion remapping */
#define N32_GPIO_RMP_ADC2_ETRR         (0x00200010U) /*!< ADC2 External Trigger Regular Conversion remapping */
#define N32_GPIO_RMP_MII_RMII_SEL      (0x00200080U) /*!< MII_RMII_SEL remapping */
#define N32_GPIO_RMP_SW_JTAG_NO_NJTRST (0x00300100U) /*!< Full SWJ Enabled (JTAG-DP + SW-DPU) but without JTRST */
#define N32_GPIO_RMP_SW_JTAG_SW_ENABLE (0x00300200U) /*!< JTAG-DP Disabled and SW-DP Enabled */
#define N32_GPIO_RMP_SW_JTAG_DISABLE   (0x00300400U) /*!< Full SWJ Disabled (JTAG-DP + SW-DPU) */

/* AFIO_RMP_CFG3 */
#define N32_GPIO_RMP_SDIO   (0x40000001U) /*!< SDIO Alternate Function mapping */
#define N32_GPIO_RMP1_CAN2  (0x40110002U) /*!< CAN2 Alternate Function mapping */
#define N32_GPIO_RMP3_CAN2  (0x40110006U) /*!< CAN2 Alternate Function mapping */
#define N32_GPIO_RMP1_QSPI  (0x40140020U) /*!< QSPI Alternate Function mapping */
#define N32_GPIO_RMP3_QSPI  (0x40140030U) /*!< QSPI Alternate Function mapping */
#define N32_GPIO_RMP1_I2C2  (0x40160040U) /*!< I2C2 Alternate Function mapping */
#define N32_GPIO_RMP3_I2C2  (0x401600C0U) /*!< I2C2 Alternate Function mapping */
#define N32_GPIO_RMP2_I2C3  (0x40180200U) /*!< I2C3 Alternate Function mapping */
#define N32_GPIO_RMP3_I2C3  (0x40180300U) /*!< I2C3 Alternate Function mapping */
#define N32_GPIO_RMP1_I2C4  (0x401A0400U) /*!< I2C4 Alternate Function mapping */
#define N32_GPIO_RMP3_I2C4  (0x401A0C00U) /*!< I2C4 Alternate Function mapping */
#define N32_GPIO_RMP1_SPI2  (0x401C1000U) /*!< SPI2 Alternate Function mapping */
#define N32_GPIO_RMP2_SPI2  (0x401C3000U) /*!< SPI2 Alternate Function mapping */
#define N32_GPIO_RMP1_SPI3  (0x401E4000U) /*!< SPI3 Alternate Function mapping */
#define N32_GPIO_RMP2_SPI3  (0x401E8000U) /*!< SPI3 Alternate Function mapping */
#define N32_GPIO_RMP3_SPI3  (0x401EC000U) /*!< SPI3 Alternate Function mapping */
#define N32_GPIO_RMP1_ETH   (0x40300001U) /*!< ETH Alternate Function mapping */
#define N32_GPIO_RMP2_ETH   (0x40300002U) /*!< ETH Alternate Function mapping */
#define N32_GPIO_RMP3_ETH   (0x40300003U) /*!< ETH Alternate Function mapping */
#define N32_GPIO_RMP1_SPI1  (0x41200000U) /*!< SPI1 Alternate Function mapping */
#define N32_GPIO_RMP2_SPI1  (0x41200004U) /*!< SPI1 Alternate Function mapping */
#define N32_GPIO_RMP3_SPI1  (0x43200004U) /*!< SPI1 Alternate Function mapping */
#define N32_GPIO_RMP1_USART2 (0x44200000U) /*!< USART2 Alternate Function mapping */
#define N32_GPIO_RMP2_USART2 (0x44200008U) /*!< USART2 Alternate Function mapping */
#define N32_GPIO_RMP3_USART2 (0x46200008U) /*!< USART2 Alternate Function mapping */
#define N32_GPIO_RMP1_UART4 (0x40340010U) /*!< UART4 Alternate Function mapping */
#define N32_GPIO_RMP2_UART4 (0x40340020U) /*!< UART4 Alternate Function mapping */
#define N32_GPIO_RMP3_UART4 (0x40340030U) /*!< UART4 Alternate Function mapping */
#define N32_GPIO_RMP1_UART5 (0x40360040U) /*!< UART5 Alternate Function mapping */
#define N32_GPIO_RMP2_UART5 (0x40360080U) /*!< UART5 Alternate Function mapping */
#define N32_GPIO_RMP3_UART5 (0x403600C0U) /*!< UART5 Alternate Function mapping */
#define N32_GPIO_RMP2_UART6 (0x40380200U) /*!< UART6 Alternate Function mapping */
#define N32_GPIO_RMP3_UART6 (0x40380300U) /*!< UART6 Alternate Function mapping */
#define N32_GPIO_RMP1_UART7 (0x403A0400U) /*!< UART7 Alternate Function mapping */
#define N32_GPIO_RMP3_UART7 (0x403A0C00U) /*!< UART7 Alternate Function mapping */
#define N32_GPIO_RMP1_TIM8  (0x403E4000U) /*!< TIM8 Alternate Function mapping */
#define N32_GPIO_RMP3_TIM8  (0x403EC000U) /*!< TIM8 Alternate Function mapping */

/* AFIO_RMP_CFG4 */
#define N32_GPIO_RMP1_COMP1       (0x20100001U) /*!< COMP1 Alternate Function mapping */
#define N32_GPIO_RMP2_COMP1       (0x20100002U) /*!< COMP1 Alternate Function mapping */
#define N32_GPIO_RMP3_COMP1       (0x20100003U) /*!< COMP1 Alternate Function mapping */
#define N32_GPIO_RMP1_COMP2       (0x20120004U) /*!< COMP2 Alternate Function mapping */
#define N32_GPIO_RMP2_COMP2       (0x20120008U) /*!< COMP2 Alternate Function mapping */
#define N32_GPIO_RMP3_COMP2       (0x2012000CU) /*!< COMP2 Alternate Function mapping */
#define N32_GPIO_RMP1_COMP3       (0x20140010U) /*!< COMP3 Alternate Function mapping */
#define N32_GPIO_RMP3_COMP3       (0x20140030U) /*!< COMP3 Alternate Function mapping */
#define N32_GPIO_RMP1_COMP4       (0x20160040U) /*!< COMP4 Alternate Function mapping */
#define N32_GPIO_RMP3_COMP4       (0x201600C0U) /*!< COMP4 Alternate Function mapping */
#define N32_GPIO_RMP1_COMP5       (0x20180100U) /*!< COMP5 Alternate Function mapping */
#define N32_GPIO_RMP2_COMP5       (0x20180200U) /*!< COMP5 Alternate Function mapping */
#define N32_GPIO_RMP3_COMP5       (0x20180300U) /*!< COMP5 Alternate Function mapping */
#define N32_GPIO_RMP1_COMP6       (0x201A0400U) /*!< COMP6 Alternate Function mapping */
#define N32_GPIO_RMP3_COMP6       (0x201A0C00U) /*!< COMP6 Alternate Function mapping */
#define N32_GPIO_RMP_COMP7        (0x20001000U) /*!< COMP7 Alternate Function mapping */
#define N32_GPIO_RMP_ADC3_ETRI    (0x20004000U) /*!< ADC3_ETRGINJ Alternate Function mapping */
#define N32_GPIO_RMP_ADC3_ETRR    (0x20008000U) /*!< ADC3_ETRGREG Alternate Function mapping */
#define N32_GPIO_RMP_ADC4_ETRI    (0x20200001U) /*!< ADC4_ETRGINJ Alternate Function mapping */
#define N32_GPIO_RMP_ADC4_ETRR    (0x20200002U) /*!< ADC4_ETRGREG Alternate Function mapping */
#define N32_GPIO_RMP_TSC_OUT_CTRL (0x20200004U) /*!< TSC_OUT_CTRL Alternate Function mapping */
#define N32_GPIO_RMP_QSPI_XIP_EN  (0x20200008U) /*!< QSPI_XIP_EN Alternate Function mapping */
#define N32_GPIO_RMP1_DVP         (0x20340010U) /*!< DVP Alternate Function mapping */
#define N32_GPIO_RMP3_DVP         (0x20340030U) /*!< DVP Alternate Function mapping */
#define N32_GPIO_Remap_SPI1_NSS   (0x20200040U) /*!< SPI1 NSS Alternate Function mapping */
#define N32_GPIO_Remap_SPI2_NSS   (0x20200080U) /*!< SPI2 NSS Alternate Function mapping */
#define N32_GPIO_Remap_SPI3_NSS   (0x20200100U) /*!< SPI3 NSS Alternate Function mapping */
#define N32_GPIO_Remap_QSPI_MISO  (0x20200200U) /*!< QSPI MISO Alternate Function mapping */

/* AFIO_RMP_CFG5 */
#define N32_GPIO_Remap_DET_EN_EGB4    (0x10200080U) /*!< EGB4 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGB3    (0x10200040U) /*!< EGB4 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGB2    (0x10200020U) /*!< EGB4 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGB1    (0x10200010U) /*!< EGB4 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGBN4   (0x10200008U) /*!< EGBN4 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGBN3   (0x10200004U) /*!< EGBN3 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGBN2   (0x10200002U) /*!< EGBN2 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_EGBN1   (0x10200001U) /*!< EGBN1 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_ECLAMP4 (0x10008000U) /*!< ECLAMP4 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_ECLAMP3 (0x10004000U) /*!< ECLAMP3 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_ECLAMP2 (0x10002000U) /*!< ECLAMP2 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_DET_EN_ECLAMP1 (0x10001000U) /*!< ECLAMP1 Detect Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGB4    (0x10000800U) /*!< EGB4 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGB3    (0x10000400U) /*!< EGB3 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGB2    (0x10000200U) /*!< EGB2 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGB1    (0x10000100U) /*!< EGB1 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGBN4   (0x10000080U) /*!< EGBN4 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGBN3   (0x10000040U) /*!< EGBN3 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGBN2   (0x10000020U) /*!< EGBN2 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_EGBN1   (0x10000010U) /*!< EGBN1 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_ECLAMP4 (0x10000008U) /*!< ECLAMP4 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_ECLAMP3 (0x10000004U) /*!< ECLAMP3 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_ECLAMP2 (0x10000002U) /*!< ECLAMP2 Reset Alternate Function mapping*/
#define N32_GPIO_Remap_RST_EN_ECLAMP1 (0x10000001U) /*!< ECLAMP1 Reset Alternate Function mapping*/

#endif