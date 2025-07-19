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
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NSING and hold NSING
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related
* to any customer's Insecure Usage.

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
 *\*\file n32g430_crc.h
 *\*\author NSING
 *\*\version v1.0.0
 *\*\copyright Copyright (c) 2022, NSING Technologies Pte. Ltd. All rights reserved.
 **/

#ifndef __N32G430_CRC_H__
#define __N32G430_CRC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "n32g430.h"

/*** CRC Structure Definition Start ***/

/*** CRC Structure Definition End ***/

/*** CRC Macro Definition Start ***/

/** CRC CRC32 reset definition **/
#define CRC_CRC32_RESET ((uint32_t)CRC32_CTRL_RESET) /* RESET bit */

/** CRC clear the CRC16 check value definition **/
#define CRC_CRC16_VALUE_CLEAR ((uint32_t)CRC16_CTRL_RESET)
#define CRC_CRC16_VALUE_NO_CLEAR ((uint32_t)CRC16_CTRL_NO_RESET)

/** CRC little-endian and big-endian definition **/
#define CRC_LITTLE_ENDIAN_ENABLE ((uint32_t)CRC16_CTRL_LITTLE) /* Set little-endian */
#define CRC_BIG_ENDIAN_ENABLE ((uint32_t)CRC16_CTRL_BIG)       /* Set big-endian */

    /*** CRC Macro Definition End ***/

    /*** CRC Driving Functions Declaration ***/
    void CRC32_Reset(void);
    uint32_t CRC32_Calculate(uint32_t data);
    uint32_t CRC32_Buffer_Calculate(uint32_t p_buf[], uint32_t buf_len);
    uint32_t CRC32_Get(void);
    void CRC32_Independent_Data_Save(uint8_t value);
    uint8_t CRC32_Independent_Data_Get(void);
    uint16_t CRC16_Calculate(uint8_t data);
    uint16_t CRC16_Buffer_Calculate(uint8_t p_buf[], uint32_t buf_len);
    void CRC16_Set(uint8_t data);
    uint16_t CRC16_Get(void);
    void CRC16_Little_Endian_Format_Set(void);
    void CRC16_Big_Endian_Format_Set(void);
    void CRC16_Value_Clean_Enable(void);
    void CRC16_Value_Clean_Disable(void);
    void CRC_LRC_Set(uint8_t data);
    uint8_t CRC_LRC_Get(void);

#ifdef __cplusplus
}
#endif

#endif /* __N32G430_CRC_H__ */
