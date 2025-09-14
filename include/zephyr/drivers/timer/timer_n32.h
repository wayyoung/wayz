/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _TIMER_N32_H
#define _TIMER_N32_H

#include <zephyr/device.h>

#ifdef CONFIG_SOC_SERIES_N32G430
#include <n32g430_tim.h>
#endif

/* bit operations */
#ifndef REG32
#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#endif
#ifndef REG16
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#endif
#ifndef REG8
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#endif
#ifndef BIT
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#endif
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end)))) 
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))

// @TODO: N32G430
/* registers definitions */
#define TIMER_CTL0(timerx)               REG32((timerx) + 0x00U)           /*!< TIMER control register 0 */
#define TIMER_CTL1(timerx)               REG32((timerx) + 0x04U)           /*!< TIMER control register 1 */
#define TIMER_SMCFG(timerx)              REG32((timerx) + 0x08U)           /*!< TIMER slave mode configuration register */
#define TIMER_DMAINTEN(timerx)           REG32((timerx) + 0x0CU)           /*!< TIMER DMA and interrupt enable register */
#define TIMER_INTF(timerx)               REG32((timerx) + 0x10U)           /*!< TIMER interrupt flag register */
#define TIMER_SWEVG(timerx)              REG32((timerx) + 0x14U)           /*!< TIMER software event generation register */
#define TIMER_CHCTL0(timerx)             REG32((timerx) + 0x18U)           /*!< TIMER channel control register 0 */
#define TIMER_CHCTL1(timerx)             REG32((timerx) + 0x1CU)           /*!< TIMER channel control register 1 */
#define TIMER_CHCTL2(timerx)             REG32((timerx) + 0x20U)           /*!< TIMER channel control register 2 */
#define TIMER_CNT(timerx)                REG32((timerx) + 0x24U)           /*!< TIMER counter register */
#define TIMER_PSC(timerx)                REG32((timerx) + 0x28U)           /*!< TIMER prescaler register */
#define TIMER_CAR(timerx)                REG32((timerx) + 0x2CU)           /*!< TIMER counter auto reload register */
#define TIMER_CREP(timerx)               REG32((timerx) + 0x30U)           /*!< TIMER counter repetition register */
#define TIMER_CH1CV(timerx)              REG32((timerx) + 0x34U)           /*!< TIMER channel 1 capture/compare value register */
#define TIMER_CH2CV(timerx)              REG32((timerx) + 0x38U)           /*!< TIMER channel 2 capture/compare value register */
#define TIMER_CH3CV(timerx)              REG32((timerx) + 0x3CU)           /*!< TIMER channel 3 capture/compare value register */
#define TIMER_CH4CV(timerx)              REG32((timerx) + 0x40U)           /*!< TIMER channel 4 capture/compare value register */
#define TIMER_CCHP(timerx)               REG32((timerx) + 0x44U)           /*!< TIMER complementary channel protection register */
#define TIMER_DMACFG(timerx)             REG32((timerx) + 0x48U)           /*!< TIMER DMA configuration register */
#define TIMER_DMATB(timerx)              REG32((timerx) + 0x4CU)           /*!< TIMER DMA transfer buffer register */
#define TIMER_IRMP(timerx)               REG32((timerx) + 0x50U)           /*!< TIMER channel input remap register */
#define TIMER_CFG(timerx)                REG32((timerx) + 0xFCU)           /*!< TIMER configuration register */

/* bits definitions */
/* TIMER_CTL0 */
#define TIMER_CTL0_CEN                   BIT(0)              /*!< TIMER counter enable */
#define TIMER_CTL0_UPDIS                 BIT(1)              /*!< update disable */
#define TIMER_CTL0_UPS                   BIT(2)              /*!< update source */
#define TIMER_CTL0_SPM                   BIT(3)              /*!< single pulse mode */
#define TIMER_CTL0_DIR                   BIT(4)              /*!< timer counter direction */
#define TIMER_CTL0_CAM                   BITS(5,6)           /*!< center-aligned mode selection */
#define TIMER_CTL0_ARSE                  BIT(7)              /*!< auto-reload shadow enable */
#define TIMER_CTL0_CKDIV                 BITS(8,9)           /*!< clock division */

/* TIMER_CTL1 */
#define TIMER_CTL1_CCSE                  BIT(0)              /*!< commutation control shadow enable */
#define TIMER_CTL1_CCUC                  BIT(2)              /*!< commutation control shadow register update control */
#define TIMER_CTL1_DMAS                  BIT(3)              /*!< DMA request source selection */
#define TIMER_CTL1_MMC                   BITS(4,6)           /*!< master mode control */
#define TIMER_CTL1_TI0S                  BIT(7)              /*!< channel 0 trigger input selection(hall mode selection) */
#define TIMER_CTL1_ISO0                  BIT(8)              /*!< idle state of channel 0 output */
#define TIMER_CTL1_ISO0N                 BIT(9)              /*!< idle state of channel 0 complementary output */
#define TIMER_CTL1_ISO1                  BIT(10)             /*!< idle state of channel 1 output */
#define TIMER_CTL1_ISO1N                 BIT(11)             /*!< idle state of channel 1 complementary output */
#define TIMER_CTL1_ISO2                  BIT(12)             /*!< idle state of channel 2 output */
#define TIMER_CTL1_ISO2N                 BIT(13)             /*!< idle state of channel 2 complementary output */
#define TIMER_CTL1_ISO3                  BIT(14)             /*!< idle state of channel 3 output */

/* TIMER_SMCFG */
#define TIMER_SMCFG_SMC                  BITS(0,2)           /*!< slave mode control */
#define TIMER_SMCFG_TRGS                 BITS(4,6)           /*!< trigger selection */
#define TIMER_SMCFG_MSM                  BIT(7)              /*!< master-slave mode */
#define TIMER_SMCFG_ETFC                 BITS(8,11)          /*!< external trigger filter control */
#define TIMER_SMCFG_ETPSC                BITS(12,13)         /*!< external trigger prescaler */
#define TIMER_SMCFG_SMC1                 BIT(14)             /*!< part of SMC for enable external clock mode 1 */
#define TIMER_SMCFG_ETP                  BIT(15)             /*!< external trigger polarity */
 
/* TIMER_DMAINTEN */
#define TIMER_DMAINTEN_UPIE              BIT(0)              /*!< update interrupt enable */
#define TIMER_DMAINTEN_CH1IE             BIT(1)              /*!< channel 1 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CH2IE             BIT(2)              /*!< channel 2 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CH3IE             BIT(3)              /*!< channel 3 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CH4IE             BIT(4)              /*!< channel 4 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CMTIE             BIT(5)              /*!< commutation interrupt request enable */
#define TIMER_DMAINTEN_TRGIE             BIT(6)              /*!< trigger interrupt enable */
#define TIMER_DMAINTEN_BRKIE             BIT(7)              /*!< break interrupt enable */
#define TIMER_DMAINTEN_UPDEN             BIT(8)              /*!< update DMA request enable */
#define TIMER_DMAINTEN_CH1DEN            BIT(9)              /*!< channel 1 DMA request enable */
#define TIMER_DMAINTEN_CH2DEN            BIT(10)             /*!< channel 2 DMA request enable */
#define TIMER_DMAINTEN_CH3DEN            BIT(11)             /*!< channel 3 DMA request enable */
#define TIMER_DMAINTEN_CH4DEN            BIT(12)             /*!< channel 4 DMA request enable */
#define TIMER_DMAINTEN_CMTDEN            BIT(13)             /*!< commutation DMA request enable */
#define TIMER_DMAINTEN_TRGDEN            BIT(14)             /*!< trigger DMA request enable */

/* TIMER_INTF */
#define TIMER_INTF_UPIF                  BIT(0)              /*!< update interrupt flag */
#define TIMER_INTF_CH1IF                 BIT(1)              /*!< channel 1 capture/compare interrupt flag */
#define TIMER_INTF_CH2IF                 BIT(2)              /*!< channel 2 capture/compare interrupt flag */
#define TIMER_INTF_CH3IF                 BIT(3)              /*!< channel 3 capture/compare interrupt flag */
#define TIMER_INTF_CH4IF                 BIT(4)              /*!< channel 4 capture/compare interrupt flag */
#define TIMER_INTF_CMTIF                 BIT(5)              /*!< channel commutation interrupt flag */
#define TIMER_INTF_TRGIF                 BIT(6)              /*!< trigger interrupt flag */
#define TIMER_INTF_BRKIF                 BIT(7)              /*!< break interrupt flag */
#define TIMER_INTF_CH1OF                 BIT(9)              /*!< channel 1 overcapture flag */
#define TIMER_INTF_CH2OF                 BIT(10)             /*!< channel 2 overcapture flag */
#define TIMER_INTF_CH3OF                 BIT(11)             /*!< channel 3 overcapture flag */
#define TIMER_INTF_CH4OF                 BIT(12)             /*!< channel 4 overcapture flag */

/* TIMER_SWEVG */
#define TIMER_SWEVG_UPG                  BIT(0)              /*!< update event generate */
#define TIMER_SWEVG_CH1G                 BIT(1)              /*!< channel 1 capture or compare event generation */
#define TIMER_SWEVG_CH2G                 BIT(2)              /*!< channel 2 capture or compare event generation */
#define TIMER_SWEVG_CH3G                 BIT(3)              /*!< channel 3 capture or compare event generation */
#define TIMER_SWEVG_CH4G                 BIT(4)              /*!< channel 4 capture or compare event generation */
#define TIMER_SWEVG_CMTG                 BIT(5)              /*!< channel commutation event generation */
#define TIMER_SWEVG_TRGG                 BIT(6)              /*!< trigger event generation */
#define TIMER_SWEVG_BRKG                 BIT(7)              /*!< break event generation */

/* TIMER_CCHP */
#define TIMER_CCHP_DTCFG                 BITS(0,7)           /*!< dead time configure */
#define TIMER_CCHP_PROT                  BITS(8,9)           /*!< complementary register protect control */
#define TIMER_CCHP_IOS                   BIT(10)             /*!< idle mode off-state configure */
#define TIMER_CCHP_ROS                   BIT(11)             /*!< run mode off-state configure */
#define TIMER_CCHP_BRKEN                 BIT(12)             /*!< break enable */
#define TIMER_CCHP_BRKP                  BIT(13)             /*!< break polarity */
#define TIMER_CCHP_OAEN                  BIT(14)             /*!< output automatic enable */
#define TIMER_CCHP_POEN                  BIT(15)             /*!< primary output enable */

// /* TIMER interrupt enable or disable */
#define TIMER_INT_UP                        TIMER_DMAINTEN_UPIE                     /*!< update interrupt */
#define TIMER_INT_CH1                       TIMER_DMAINTEN_CH1IE                    /*!< channel 1 interrupt */
#define TIMER_INT_CH2                       TIMER_DMAINTEN_CH2IE                    /*!< channel 2 interrupt */
#define TIMER_INT_CH3                       TIMER_DMAINTEN_CH3IE                    /*!< channel 3 interrupt */
#define TIMER_INT_CH4                       TIMER_DMAINTEN_CH4IE                    /*!< channel 4 interrupt */
#define TIMER_INT_CMT                       TIMER_DMAINTEN_CMTIE                    /*!< channel commutation interrupt flag */
#define TIMER_INT_TRG                       TIMER_DMAINTEN_TRGIE                    /*!< trigger interrupt */
#define TIMER_INT_BRK                       TIMER_DMAINTEN_BRKIE                    /*!< break interrupt */

/* TIMER flag */
#define TIMER_FLAG_UP                       TIMER_INTF_UPIF                         /*!< update flag */
#define TIMER_FLAG_CH1                      TIMER_INTF_CH1IF                        /*!< channel 1 flag */
#define TIMER_FLAG_CH2                      TIMER_INTF_CH2IF                        /*!< channel 2 flag */
#define TIMER_FLAG_CH3                      TIMER_INTF_CH3IF                        /*!< channel 3 flag */
#define TIMER_FLAG_CH4                      TIMER_INTF_CH4IF                        /*!< channel 4 flag */
#define TIMER_FLAG_CMT                      TIMER_INTF_CMTIF                        /*!< channel commutation flag */
#define TIMER_FLAG_TRG                      TIMER_INTF_TRGIF                        /*!< trigger flag */
#define TIMER_FLAG_BRK                      TIMER_INTF_BRKIF                        /*!< break flag */
#define TIMER_FLAG_CH1O                     TIMER_INTF_CH1OF                        /*!< channel 1 overcapture flag */
#define TIMER_FLAG_CH2O                     TIMER_INTF_CH2OF                        /*!< channel 2 overcapture flag */
#define TIMER_FLAG_CH3O                     TIMER_INTF_CH3OF                        /*!< channel 3 overcapture flag */
#define TIMER_FLAG_CH4O                     TIMER_INTF_CH4OF                        /*!< channel 4 overcapture flag */

/* TIMER interrupt flag */
#define TIMER_INT_FLAG_UP                   TIMER_INTF_UPIF                         /*!< update interrupt flag */
#define TIMER_INT_FLAG_CH1                  TIMER_INTF_CH1IF                        /*!< channel 1 interrupt flag */
#define TIMER_INT_FLAG_CH2                  TIMER_INTF_CH2IF                        /*!< channel 2 interrupt flag */
#define TIMER_INT_FLAG_CH3                  TIMER_INTF_CH3IF                        /*!< channel 3 interrupt flag */
#define TIMER_INT_FLAG_CH4                  TIMER_INTF_CH4IF                        /*!< channel 4 interrupt flag */
#define TIMER_INT_FLAG_CMT                  TIMER_INTF_CMTIF                        /*!< channel commutation interrupt flag */
#define TIMER_INT_FLAG_TRG                  TIMER_INTF_TRGIF                        /*!< trigger interrupt flag */
#define TIMER_INT_FLAG_BRK                  TIMER_INTF_BRKIF

/* center-aligned mode selection */
#define CTL0_CAM(regval)                    ((uint16_t)(BITS(5, 6) & ((uint32_t)(regval) << 5U)))
#define TIMER_COUNTER_EDGE                  CTL0_CAM(0)                             /*!< edge-aligned mode */
#define TIMER_COUNTER_CENTER_DOWN           CTL0_CAM(1)                             /*!< center-aligned and counting down assert mode */
#define TIMER_COUNTER_CENTER_UP             CTL0_CAM(2)                             /*!< center-aligned and counting up assert mode */
#define TIMER_COUNTER_CENTER_BOTH           CTL0_CAM(3)                             /*!< center-aligned and counting up/down assert mode */

/* TIMER prescaler reload mode */
#define TIMER_PSC_RELOAD_NOW                ((uint32_t)0x00000000U)                        /*!< the prescaler is loaded right now */
#define TIMER_PSC_RELOAD_UPDATE             ((uint32_t)0x00000001U)                        /*!< the prescaler is loaded at the next update event */

/* count direction */
#define TIMER_COUNTER_UP                    ((uint16_t)0x0000U)                     /*!< counter up direction */
#define TIMER_COUNTER_DOWN                  ((uint16_t)TIMER_CTL0_DIR)              /*!< counter down direction */

/* specify division ratio between TIMER clock and dead-time and sampling clock */
#define CTL0_CKDIV(regval)                  ((uint16_t)(BITS(8, 9) & ((uint32_t)(regval) << 8U)))
#define TIMER_CKDIV_DIV1                    CTL0_CKDIV(0)                           /*!< clock division value is 1,fDTS=fTIMER_CK */
#define TIMER_CKDIV_DIV2                    CTL0_CKDIV(1)                           /*!< clock division value is 2,fDTS= fTIMER_CK/2 */
#define TIMER_CKDIV_DIV4                    CTL0_CKDIV(2)                           /*!< clock division value is 4, fDTS= fTIMER_CK/4 */

/* channel output compare mode */
#define TIMER_OC_MODE_TIMING                ((uint16_t)0x0000U)                     /*!< timing mode */
#define TIMER_OC_MODE_ACTIVE                ((uint16_t)0x0010U)                     /*!< active mode */
#define TIMER_OC_MODE_INACTIVE              ((uint16_t)0x0020U)                     /*!< inactive mode */
#define TIMER_OC_MODE_TOGGLE                ((uint16_t)0x0030U)                     /*!< toggle mode */
#define TIMER_OC_MODE_LOW                   ((uint16_t)0x0040U)                     /*!< force low mode */
#define TIMER_OC_MODE_HIGH                  ((uint16_t)0x0050U)                     /*!< force high mode */
#define TIMER_OC_MODE_PWM0                  ((uint16_t)0x0060U)                     /*!< PWM0 mode */
#define TIMER_OC_MODE_PWM1                  ((uint16_t)0x0070U)                     /*!< PWM1 mode*/

/* channel output compare shadow enable */
#define TIMER_OC_SHADOW_ENABLE              ((uint16_t)0x0008U)                     /*!< channel output shadow state enable */
#define TIMER_OC_SHADOW_DISABLE             ((uint16_t)0x0000U)                     /*!< channel output shadow state disable */

/* channel output compare fast enable */
#define TIMER_OC_FAST_ENABLE                ((uint16_t)0x0004)                      /*!< channel output fast function enable */
#define TIMER_OC_FAST_DISABLE               ((uint16_t)0x0000)                      /*!< channel output fast function disable */

/* channel output compare clear enable */
#define TIMER_OC_CLEAR_ENABLE               ((uint16_t)0x0080U)                     /*!< channel output clear function enable */
#define TIMER_OC_CLEAR_DISABLE              ((uint16_t)0x0000U)                     /*!< channel output clear function disable */


#endif
