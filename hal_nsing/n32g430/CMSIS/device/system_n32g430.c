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
 *\*\file system_n32g430.c
 *\*\author NSING
 *\*\version v1.0.3
 *\*\copyright Copyright (c) 2022, NSING Technologies Pte. Ltd. All rights reserved.
 */
#include "n32g430.h"

/* Uncomment the line corresponding to the desired System clock (SYSCLK)
   frequency (after reset the HSI is used as SYSCLK source)

   IMPORTANT NOTE:
   ==============
   1. After each device reset the HSI is used as System clock source.

   2. Please make sure that the selected System clock doesn't exceed your
   device's maximum frequency.

   3. If none of the define below is enabled, the HSI is used as System clock
    source.

   4. The System clock configuration functions provided within this file assume
   that:
        - For Low, Medium and High density Value line devices an external 8MHz
          crystal is used to drive the System clock.
        - For Low, Medium and High density devices an external 8MHz crystal is
          used to drive the System clock.
        - For Connectivity line devices an external 25MHz crystal is used to
   drive the System clock. If you are using different crystal you have to adapt
   those functions accordingly.
    */

#define SYSCLK_USE_HSI 0
#define SYSCLK_USE_HSE 1
#define SYSCLK_USE_HSI_PLL 2
#define SYSCLK_USE_HSE_PLL 3

#ifndef SYSCLK_FREQ
#define SYSCLK_FREQ CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC
#endif

#ifdef CONFIG_N32_SYSCLK_SRC
#define SYSCLK_SRC CONFIG_N32_SYSCLK_SRC
#else
#ifndef SYSCLK_SRC
#define SYSCLK_SRC SYSCLK_USE_HSE_PLL
#endif
#endif

#if SYSCLK_SRC == SYSCLK_USE_HSI

#if SYSCLK_FREQ != HSI_VALUE
#error SYSCL_FREQ must be set to HSI_VALUE
#endif

#elif SYSCLK_SRC == SYSCLK_USE_HSE

#ifndef HSE_VALUE
#error HSE_VALUE must be defined!
#endif

#if SYSCLK_FREQ != HSE_VALUE
#error SYSCL_FREQ must be set to HSE_VALUE
#endif

#elif SYSCLK_SRC == SYSCLK_USE_HSI_PLL

#if SYSCLK_FREQ < 32000000
#error When SYSCLK_SRC is PLL, SYSCLK_FREQ must be greater than or equal to 32MHz
#endif

#if (SYSCLK_FREQ % (HSI_VALUE / 2) == 0) && (SYSCLK_FREQ / (HSI_VALUE / 2) >= 2) && (SYSCLK_FREQ / (HSI_VALUE / 2) <= 32)

#define PLLSRC_DIV 2
#define PLL_MUL (SYSCLK_FREQ / (HSI_VALUE / 2))

#elif (SYSCLK_FREQ % HSI_VALUE == 0) && (SYSCLK_FREQ / HSI_VALUE >= 2)
&&(SYSCLK_FREQ / HSI_VALUE <= 32)

#define PLLSRC_DIV 1
#define PLL_MUL (SYSCLK_FREQ / HSI_VALUE)

#else
#error Cannot make a PLL multiply factor to SYSCLK_FREQ.
#endif

#elif SYSCLK_SRC == SYSCLK_USE_HSE_PLL

#if SYSCLK_FREQ < 32000000
#error When SYSCLK_SRC is PLL, SYSCLK_FREQ must be greater than or equal to 32MHz
#endif

#ifndef HSE_VALUE
#error HSE_VALUE must be defined!
#endif

#if ((SYSCLK_FREQ % (HSE_VALUE / 2)) == 0) && (SYSCLK_FREQ / (HSE_VALUE / 2) >= 2) && (SYSCLK_FREQ / (HSE_VALUE / 2) <= 32)

#define PLLSRC_DIV 2
#define PLL_MUL (SYSCLK_FREQ / (HSE_VALUE / 2))

#elif (SYSCLK_FREQ % HSE_VALUE == 0) && (SYSCLK_FREQ / HSE_VALUE >= 2)
&&(SYSCLK_FREQ / HSE_VALUE <= 32)

#define PLLSRC_DIV 1
#define PLL_MUL (SYSCLK_FREQ / HSE_VALUE)

#else
#error Cannot make a PLL multiply factor to SYSCLK_FREQ.
#endif

#else
#error wrong value for SYSCLK_SRC
#endif

/* If you want to use VECT_TAB_SRAM, release the comment*/
/* #define VECT_TAB_SRAM  */

/* Vector Table base offset field. This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET 0x0

/** Clock Definitions **/

/* System Clock Frequency (Core Clock) */
uint32_t SystemClockFrequency = SYSCLK_FREQ;

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

static void System_Clock_Set(void);

#ifdef DATA_IN_ExtSRAM
static void SystemInit_ExtMemCtl(void);
#endif /* DATA_IN_ExtSRAM */

typedef void (*pFunction)(uint32_t, uint32_t *);

/**
 *\*\name    PLL_TrimValueLoad.
 *\*\fun     Load PLL trim value.
 *\*\param   none
 *\*\return  none
 **/
void PLL_TrimValueLoad(void)
{
    pFunction get_nvr = (pFunction)(0x1FFF02A5);

    uint32_t value = 0, value1, value3, value4;
    uint32_t temp = 0;

    /* Disable the iCache */
    FLASH->AC &= FLASH_ICACHE_DIS;
    /* ICache Reset */
    FLASH->AC |= FLASH_ICAHRST_MSK;

    get_nvr(0x1FFFF020, &value);
    if ((value & 0xFF) <= 5)
    {
        get_nvr(0x1FFFF230, &value);
        /* Big-endian little-endian exchange */
        value1 = value >> 24;
        value3 = (value & 0xFF0000) >> 8;
        value4 = (value & 0xFF00) << 8;
        value = value1 | value3 | value4;

        temp = AFEC->TRIMR1 & 0xFF000000;
        temp |= value;

        AFEC->TRIMR1 = temp;
    }
    else
    {
    }
    /* Enable the iCache */
    FLASH->AC |= FLASH_ICACHE_EN;
}

/**
 *\*\name    System_Initializes.
 *\*\fun     Setup the microcontroller system
 *\*\        Initialize the Embedded Flash Interface, the PLL and update the
 *\*\        SystemClockFrequency variable.
 *\*\param   none.
 *\*\return  none
 *\*\note    This function should be used only after reset.
 **/
void System_Initializes(void)
{
    /* FPU settings */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    /* set CP10 and CP11 Full Access */
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
#endif

    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSIEN bit */
    RCC->CTRL |= RCC_CTRL_HSIEN;

    /* Reset SCLKSW, AHBPRES, APB1PRES, APB2PRES and MCO bits */
    RCC->CFG &= ~(RCC_CFG_SCLKSW | RCC_CFG_AHBPRES | RCC_CFG_APB1PRES | RCC_CFG_APB2PRES | RCC_CFG_MCO);

    /* Reset HSEEN, CLKSSEN and PLLEN bits */
    RCC->CTRL &= ~(RCC_CTRL_HSEEN | RCC_CTRL_CLKSSEN | RCC_CTRL_PLLEN);

    /* Reset HSEBP bit */
    RCC->CTRL &= ~RCC_CTRL_HSEBP;

    /* Reset PLLSRC, PLLHSEPRES, PLLMULFCT bits */
    RCC->CFG &= ~(RCC_CFG_PLLSRC | RCC_CFG_PLLHSEPRES | RCC_CFG_PLLMULFCT);

    /* Reset CFG2 register */
    RCC->CFG2 = RCC_CFG2_ADC1MPRES_DIV8;

    /* Reset PLLHSIPRE register */
    RCC->PLLHSIPRE |= RCC_PLLHSIPRE_PLLHSIPRE;

    /* Disable all interrupts and clear pending bits  */
    RCC->CLKINT = (RCC_CLKINT_LSIRDICLR | RCC_CLKINT_LSERDICLR | RCC_CLKINT_HSIRDICLR | RCC_CLKINT_HSERDICLR | RCC_CLKINT_PLLRDICLR | RCC_CLKINT_CLKSSICLR | RCC_CLKINT_LSESSICLR);

    RCC->APB1PCLKEN |= RCC_APB1PCLKEN_PWREN;
    RCC->APB1PCLKEN &= (uint32_t)(~RCC_APB1PCLKEN_PWREN);

    /* Enable ICACHE and Disable Prefetch Buffer */
    FLASH->AC |= (uint32_t)(FLASH_AC_ICAHEN);
    FLASH->AC &= (~((uint32_t)FLASH_AC_PRFTBFEN));

#ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM */

    /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
    /* Configure the Flash Latency cycles and enable prefetch buffer */
    System_Clock_Set();

#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
}

/**
 *\*\name    System_Clock_Frequency_Update.
 *\*\fun     Update SystemClockFrequency variable according to Clock Register Values.
 *\*\         The SystemClockFrequency variable contains the core clock (HCLK), it can
 *\*\         be used by the user application to setup the SysTick timer or
 *\*\         configure other parameters.
 *\*\param   none.
 *\*\return  none
 *\*\note    Each time the core clock (HCLK) changes, this function must be called
 *\*\         to update SystemClockFrequency variable value. Otherwise, any
 *\*\         configuration based on this variable will be incorrect.
 *\*\
 *\*\note    The system frequency computed by this function is not the real
 *\*\         frequency in the chip. It is calculated based on the predefined
 *\*\         constant and the selected clock source:
 *\*\
 *\*\           - If SYSCLK source is HSI, SystemClockFrequency will contain the HSI_VALUE
 *\*\
 *\*\           - If SYSCLK source is HSE, SystemClockFrequency will contain the HSE_VALUE
 *\*\
 *\*\           - If SYSCLK source is PLL, SystemClockFrequency will contain the
 *\*\             HSE_VALUE or HSI_VALUE multiplied by the PLL factors.
 *\*\
 *\*\          HSI_VALUE is a constant defined in n32g430.h file (default value
 *\*\             8 MHz) but the real value may vary depending on the variations
 *\*\             in voltage and temperature.
 *\*\
 *\*\          HSE_VALUE is a constant defined in N32G430.h file (default value
 *\*\              8 MHz, depedning on the product used), user has to ensure
 *\*\              that HSE_VALUE is same as the real frequency of the crystal used.
 *\*\              Otherwise, this function may have wrong result.
 *\*\
 *\*\         - The result of this function could be not correct when using
 *\*\ fractional value for HSE crystal.
 **/
void System_Clock_Frequency_Update(void)
{
    uint32_t temp_value = SYSCLOCK_SRC_HSI;
    uint32_t pllmull_value = 0, pllsource_value = 0;

    /* Get SYSCLK source
     * -------------------------------------------------------*/
    temp_value = RCC->CFG & RCC_CFG_SCLKSTS;

    switch (temp_value)
    {
    case SYSCLOCK_SRC_HSI: /* HSI used as system clock */
        SystemClockFrequency = HSI_VALUE;
        break;
    case SYSCLOCK_SRC_HSE: /* HSE used as system clock */
        SystemClockFrequency = HSE_VALUE;
        break;
    case SYSCLOCK_SRC_PLL: /* PLL used as system clock */

        /* Get PLL clock source and multiplication factor
         * ----------------------*/
        pllmull_value = RCC->CFG & RCC_CFG_PLLMULFCT;
        pllsource_value = RCC->CFG & RCC_CFG_PLLSRC;

        if ((pllmull_value & RCC_CFG_PLLMULFCT_4) == 0)
        {
            // PLLMULFCT[4] = 0
            pllmull_value = (pllmull_value >> 18) + 2;
        }
        else
        {
            // PLLMULFCT[4] = 1
            pllmull_value = ((pllmull_value >> 18) - 496) + 1;
        }

        if (pllsource_value == 0x00)
        {
            /* HSI selected as PLL clock entry */
            if ((RCC->PLLHSIPRE & RCC_PLLHSIPRE_PLLHSIPRE) != (uint32_t)RESET)
            { /* HSI oscillator clock divided by 2 */
                SystemClockFrequency = (HSI_VALUE >> 1) * pllmull_value;
            }
            else
            {
                SystemClockFrequency = HSI_VALUE * pllmull_value;
            }
        }
        else
        {
            /* HSE selected as PLL clock entry */
            if ((RCC->CFG & RCC_CFG_PLLHSEPRES) != (uint32_t)RESET)
            { /* HSE oscillator clock divided by 2 */
                SystemClockFrequency = (HSE_VALUE >> 1) * pllmull_value;
            }
            else
            {
                SystemClockFrequency = HSE_VALUE * pllmull_value;
            }
        }

        break;

    default:
        SystemClockFrequency = HSI_VALUE;
        break;
    }

    /* Compute HCLK clock frequency ----------------*/
    /* Get HCLK prescaler */
    temp_value = AHBPrescTable[((RCC->CFG & RCC_CFG_AHBPRES) >> 4)];
    /* HCLK clock frequency */
    SystemClockFrequency >>= temp_value;
}

/**
 *\*\name    System_Clock_Set.
 *\*\fun     Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
 *\*\param   none.
 *\*\return  none
 **/
static void System_Clock_Set(void)
{
    volatile uint32_t temp_value, temp_value1 = 0;
    volatile bool status_value = 0;
    volatile uint32_t counter_value = 0;
    uint8_t id_version = 0;

#if ((SYSCLK_SRC == SYSCLK_USE_HSI) || (SYSCLK_SRC == SYSCLK_USE_HSI_PLL))

    /* Enable HSI */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSIEN);

    /* Wait till HSI is ready and if Time out is reached exit */
    do
    {
        status_value = RCC->CTRL & RCC_CTRL_HSIRDF;
        counter_value++;
    } while ((status_value == 0) && (counter_value != HSI_STARTUP_TIMEOUT));

    status_value = ((RCC->CTRL & RCC_CTRL_HSIRDF) != RESET);
    if (!status_value)
    {
        /* If HSI fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error */
        SystemClockFrequency = HSI_VALUE;
        return;
    }

#elif SYSCLK_SRC == SYSCLK_USE_HSE || SYSCLK_SRC == SYSCLK_USE_HSE_PLL

    /* Enable HSE */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSEEN);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        status_value = RCC->CTRL & RCC_CTRL_HSERDF;
        counter_value++;
    } while ((status_value == 0) && (counter_value != HSE_STARTUP_TIMEOUT));

    status_value = ((RCC->CTRL & RCC_CTRL_HSERDF) != RESET);
    if (!status_value)
    {
        /* If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error */
        SystemClockFrequency = HSI_VALUE;
        return;
    }
#endif

    /* Flash wait state
        C version              D version
        0: HCLK <= 32M         0: HCLK <= 39M
        1: HCLK <= 64M         1: HCLK <= 78M
        2: HCLK <= 96M         2: HCLK <= 117M
        3: HCLK <= 128M        3: HCLK <= 128M
     */
    FLASH->AC &= (uint32_t)((uint32_t)~FLASH_AC_LATENCY);

    id_version = (uint8_t)(DBG->ID & 0x00FF);
    if (id_version <= 0x12)
    {
        FLASH->AC |= (uint32_t)((SYSCLK_FREQ - 1) / SYSCLK_FREQ_32M);
    }
    else
    {
        FLASH->AC |= (uint32_t)((SYSCLK_FREQ - 1) / SYSCLK_FREQ_39M);
    }

    /* HCLK = SYSCLK */
    RCC->CFG |= (uint32_t)RCC_CFG_AHBPRES_DIV1;

    /* PCLK2 max 64M */
    if (SYSCLK_FREQ > SYSCLK_FREQ_64M)
    {
        RCC->CFG |= (uint32_t)RCC_CFG_APB2PRES_DIV2;
    }
    else
    {
        RCC->CFG |= (uint32_t)RCC_CFG_APB2PRES_DIV1;
    }

    /* PCLK1 max 32M */
    if (SYSCLK_FREQ > SYSCLK_FREQ_64M)
    {
        RCC->CFG |= (uint32_t)RCC_CFG_APB1PRES_DIV4;
    }
    else if (SYSCLK_FREQ > SYSCLK_FREQ_32M)
    {
        RCC->CFG |= (uint32_t)RCC_CFG_APB1PRES_DIV2;
    }
    else
    {
        RCC->CFG |= (uint32_t)RCC_CFG_APB1PRES_DIV1;
    }

#if SYSCLK_SRC == SYSCLK_USE_HSI
    /* Select HSI as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t) ~(RCC_CFG_SCLKSW));
    RCC->CFG |= (uint32_t)RCC_CFG_SCLKSW_HSI;

    /* Wait till HSI is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SCLKSTS) != (uint32_t)SYSCLOCK_SRC_HSI)
    {
    }
#elif SYSCLK_SRC == SYSCLK_USE_HSE
    /* Select HSE as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t) ~(RCC_CFG_SCLKSW));
    RCC->CFG |= (uint32_t)RCC_CFG_SCLKSW_HSE;

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SCLKSTS) != (uint32_t)SYSCLOCK_SRC_HSE)
    {
    }
#elif SYSCLK_SRC == SYSCLK_USE_HSI_PLL || SYSCLK_SRC == SYSCLK_USE_HSE_PLL
    PLL_TrimValueLoad();
    /* clear bits */
    RCC->CFG &= (uint32_t)((uint32_t) ~(RCC_CFG_PLLSRC | RCC_CFG_PLLHSEPRES | RCC_CFG_PLLMULFCT));
    RCC->PLLHSIPRE &= (uint32_t)((uint32_t) ~(RCC_PLLHSIPRE_PLLHSIPRE));

    /* set PLL source */
    temp_value = RCC->CFG;
    temp_value |= (SYSCLK_SRC == SYSCLK_USE_HSI_PLL ? RCC_CFG_PLLSRC_HSI : RCC_CFG_PLLSRC_HSE);

#if SYSCLK_SRC == SYSCLK_USE_HSI_PLL
    /* set PLLHSIPRES */
    temp_value1 |= (PLLSRC_DIV == 1 ? RCC_PLLHSIPRE_PLLHSIPRE_HSI : RCC_PLLHSIPRE_PLLHSIPRE_HSI_DIV2);
#elif SYSCLK_SRC == SYSCLK_USE_HSE_PLL
    /* set PLLHSEPRES */
    temp_value |= (PLLSRC_DIV == 1 ? RCC_CFG_PLLHSEPRES_HSE : RCC_CFG_PLLHSEPRES_HSE_DIV2);
#endif

    /* set PLL multiply factor */
#if PLL_MUL <= PLL_MUL_FACTOR_16
    temp_value |= (PLL_MUL - 2) << RCC_PLLMULFCT_OFFSET;
#else
    temp_value |= ((PLL_MUL - 17) << RCC_PLLMULFCT_OFFSET) | (1 << 27);
#endif

    RCC->CFG = temp_value;
    RCC->PLLHSIPRE = temp_value1;

    /* Enable PLL */
    RCC->CTRL |= RCC_CTRL_PLLEN;

    /* Wait till PLL is ready */
    while ((RCC->CTRL & RCC_CTRL_PLLRDF) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t) ~(RCC_CFG_SCLKSW));
    RCC->CFG |= (uint32_t)RCC_CFG_SCLKSW_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SCLKSTS) != (uint32_t)SYSCLOCK_SRC_PLL)
    {
    }
#endif
}
