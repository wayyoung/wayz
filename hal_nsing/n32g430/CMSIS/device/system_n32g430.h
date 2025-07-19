/**
*     Copyright (c) 2022, NSING Technologies Pte. Ltd.
*
*     All rights reserved.
*
*     This software is the exclusive property of NSING Technologies Pte. Ltd. (Hereinafter
* referred to as NSING). This software, and the product of NSING described herein
* (Hereinafter referred to as the Product) are owned by NSING under the laws and treaties
* of the Republic of Singapore and other applicable jurisdictions worldwide.
*
*     NSING does not grant any license under its patents, copyrights, trademarks, or other
* intellectual property rights. Names and brands of third party may be mentioned or referred
* thereto (if any) for identification purposes only.
*
*     NSING reserves the right to make changes, corrections, enhancements, modifications, and
* improvements to this software at any time without notice. Please contact NSING and obtain
* the latest version of this software before placing orders.

*     Although NSING has attempted to provide accurate and reliable information, NSING assumes
* no responsibility for the accuracy and reliability of this software.
*
*     It is the responsibility of the user of this software to properly design, program, and test
* the functionality and safety of any application made of this information and any resulting product.
* In no event shall NSING be liable for any direct, indirect, incidental, special,exemplary, or
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NSING Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property
* damage. Such applications are deemed, �Insecure Usage�.
*
*     All Insecure Usage shall be made at user�s risk. User shall indemnify NSING and hold NSING
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related
* to any customer�s Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NSING, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NSING products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations.
* User shall comply with any applicable export control laws and regulations promulgated and administered by
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
 *\*\file system_n32g430.h
 *\*\author NSING
 *\*\version v1.0.1
 *\*\copyright Copyright (c) 2022, NSING Technologies Pte. Ltd. All rights reserved.
 */
#ifndef __SYSTEM_N32G430_H__
#define __SYSTEM_N32G430_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        SYSCLOCK_SRC_HSI = 0x00,
        SYSCLOCK_SRC_HSE = 0x04,
        SYSCLOCK_SRC_PLL = 0x08
    } SysclkSrc;

/** RCC REG_BIT_MASK **/
#define REG_BIT_MASK ((uint32_t)0x00000000)

#define SYSCLK_FREQ_128M (128000000)
#define SYSCLK_FREQ_64M (64000000)
#define SYSCLK_FREQ_32M (32000000)
#define SYSCLK_FREQ_39M (39000000)

#define PLL_MUL_FACTOR_16 (16)
#define RCC_PLLMULFCT_OFFSET (REG_BIT18_OFFSET)

    extern uint32_t SystemClockFrequency; /* System Clock Frequency */

    extern void System_Initializes(void);
    extern void System_Clock_Frequency_Update(void);

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_N32G430_H__ */
