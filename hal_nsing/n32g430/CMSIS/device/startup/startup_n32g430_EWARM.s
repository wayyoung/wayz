;     Copyright (c) 2022, NSING Technologies Pte. Ltd.
; 
;     All rights reserved.
;
;     This software is the exclusive property of NSING Technologies Pte. Ltd. (Hereinafter 
; referred to as NSING). This software, and the product of NSING described herein 
; (Hereinafter referred to as the Product) are owned by NSING under the laws and treaties
; of the Republic of Singapore and other applicable jurisdictions worldwide.
;
;     NSING does not grant any license under its patents, copyrights, trademarks, or other 
; intellectual property rights. Names and brands of third party may be mentioned or referred 
; thereto (if any) for identification purposes only.
;
;     NSING reserves the right to make changes, corrections, enhancements, modifications, and 
; improvements to this software at any time without notice. Please contact NSING and obtain 
; the latest version of this software before placing orders.

;     Although NSING has attempted to provide accurate and reliable information, NSING assumes 
; no responsibility for the accuracy and reliability of this software.
; 
;     It is the responsibility of the user of this software to properly design, program, and test 
; the functionality and safety of any application made of this information and any resulting product. 
; In no event shall NSING be liable for any direct, indirect, incidental, special,exemplary, or 
; consequential damages arising in any way out of the use of this software or the Product.
;
;     NSING Products are neither intended nor warranted for usage in systems or equipment, any
; malfunction or failure of which may cause loss of human life, bodily injury or severe property 
; damage. Such applications are deemed, "Insecure Usage".
;
;     All Insecure Usage shall be made at user's risk. User shall indemnify NSING and hold NSING 
; harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
; to any customer's Insecure Usage.

;     Any express or implied warranty with regard to this software or the Product, including,but not 
; limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
; are disclaimed to the fullest extent permitted by law.

;     Unless otherwise explicitly permitted by NSING, anyone may not duplicate, modify, transcribe
; or otherwise distribute this software for any purposes, in whole or in part.
;
;     NSING products and technologies shall not be used for or incorporated into any products or systems
; whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
; User shall comply with any applicable export control laws and regulations promulgated and administered by 
; the governments of any countries asserting jurisdiction over the parties or transactions.

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
  
    MODULE  ?cstartup
        
        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  System_Initializes        
        PUBLIC  __vector_table

        DATA
       
__vector_table
        DCD     sfe(CSTACK)
    DCD     Reset_Handler              ; Reset Handler
    DCD     NMI_Handler                ; NMI Handler
    DCD     HardFault_Handler          ; Hard Fault Handler
    DCD     MemManage_Handler          ; MPU Fault Handler
    DCD     BusFault_Handler           ; Bus Fault Handler
    DCD     UsageFault_Handler         ; Usage Fault Handler
    DCD     0                          ; Reserved
    DCD     0                          ; Reserved
    DCD     0                          ; Reserved
    DCD     0                          ; Reserved
    DCD     SVC_Handler                ; SVCall Handler
    DCD     DebugMon_Handler           ; Debug Monitor Handler
    DCD     0                          ; Reserved
    DCD     PendSV_Handler             ; PendSV Handler
    DCD     SysTick_Handler            ; SysTick Handler

    ; External Interrupts
    DCD     WWDG_IRQHandler            ; Window Watchdog
    DCD     PVD_IRQHandler             ; PVD through EXTI Line detect
    DCD     RTC_TAMPER_STAMP_IRQHandler; EXTI Line18
    DCD     RTC_WKUP_IRQHandler        ; RTC_WKUP
    DCD     FLASH_IRQHandler           ; Flash
    DCD     RCC_IRQHandler             ; RCC
    DCD     EXTI0_IRQHandler           ; EXTI Line 0
    DCD     EXTI1_IRQHandler           ; EXTI Line 1
    DCD     EXTI2_IRQHandler           ; EXTI Line 2
    DCD     EXTI3_IRQHandler           ; EXTI Line 3
    DCD     EXTI4_IRQHandler           ; EXTI Line 4
    DCD     DMA_Channel1_IRQHandler    ; DMA Channel 1
    DCD     DMA_Channel2_IRQHandler    ; DMA Channel 2
    DCD     DMA_Channel3_IRQHandler    ; DMA Channel 3
    DCD     DMA_Channel4_IRQHandler    ; DMA Channel 4
    DCD     DMA_Channel5_IRQHandler    ; DMA Channel 5
    DCD     DMA_Channel6_IRQHandler    ; DMA Channel 6
    DCD     DMA_Channel7_IRQHandler    ; DMA Channel 7
    DCD     DMA_Channel8_IRQHandler    ; DMA Channel 8
    DCD     ADC_IRQHandler             ; ADC
    DCD     MMU_IRQHandler             ; MMU
    DCD     COMP_1_2_3_IRQHandler      ; COMP1 & COMP2 & COMP3
    DCD     EXTI9_5_IRQHandler         ; EXTI Line 9..5
    DCD     TIM1_BRK_IRQHandler        ; TIM1 Break
    DCD     TIM1_UP_IRQHandler         ; TIM1 Update
    DCD     TIM1_TRG_COM_IRQHandler    ; TIM1 Trigger and Commutation
    DCD     TIM1_CC_IRQHandler         ; TIM1 Capture Compare
    DCD     TIM2_IRQHandler            ; TIM2
    DCD     TIM3_IRQHandler            ; TIM3
    DCD     TIM4_IRQHandler            ; TIM4
    DCD     I2C1_EV_IRQHandler         ; I2C1 Event
    DCD     I2C1_ER_IRQHandler         ; I2C1 Error
    DCD     I2C2_EV_IRQHandler         ; I2C2 Event
    DCD     I2C2_ER_IRQHandler         ; I2C2 Error
    DCD     SPI1_IRQHandler            ; SPI1
    DCD     SPI2_IRQHandler            ; SPI2/I2S2
    DCD     USART1_IRQHandler          ; USART1
    DCD     USART2_IRQHandler          ; USART2
    DCD     UART3_IRQHandler           ; UART3
    DCD     EXTI15_10_IRQHandler       ; EXTI Line 15..10
    DCD     RTCAlarm_IRQHandler        ; RTC Alarm through EXTI Line
    DCD     LPTIM_WKUP_IRQHandler      ; LPTIM_WKUP through EXTI line 20
    DCD     TIM8_BRK_IRQHandler        ; TIM8 Break
    DCD     TIM8_UP_IRQHandler         ; TIM8 Update
    DCD     TIM8_TRG_COM_IRQHandler    ; TIM8 Trigger and Commutation
    DCD     TIM8_CC_IRQHandler         ; TIM8 Capture Compare
    DCD     UART4_IRQHandler           ; UART4
    DCD     TIM5_IRQHandler            ; TIM5
    DCD     TIM6_IRQHandler            ; TIM6
    DCD     CAN_TX_IRQHandler          ; CAN TX
    DCD     CAN_RX0_IRQHandler         ; CAN RX0
    DCD     CAN_RX1_IRQHandler         ; CAN RX1
    DCD     CAN_SCE_IRQHandler         ; CAN SCE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =System_Initializes
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
               
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WWDG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WWDG_IRQHandler
        B WWDG_IRQHandler

        PUBWEAK PVD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PVD_IRQHandler
        B PVD_IRQHandler

        PUBWEAK RTC_TAMPER_STAMP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_TAMPER_STAMP_IRQHandler
        B RTC_TAMPER_STAMP_IRQHandler

        PUBWEAK RTC_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_WKUP_IRQHandler
        B RTC_WKUP_IRQHandler

        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FLASH_IRQHandler
        B FLASH_IRQHandler

        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RCC_IRQHandler
        B RCC_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI0_IRQHandler
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI1_IRQHandler
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI2_IRQHandler
        B EXTI2_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI3_IRQHandler
        B EXTI3_IRQHandler

        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI4_IRQHandler
        B EXTI4_IRQHandler

        PUBWEAK DMA_Channel1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel1_IRQHandler
        B DMA_Channel1_IRQHandler

        PUBWEAK DMA_Channel2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel2_IRQHandler
        B DMA_Channel2_IRQHandler

        PUBWEAK DMA_Channel3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel3_IRQHandler
        B DMA_Channel3_IRQHandler

        PUBWEAK DMA_Channel4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel4_IRQHandler
        B DMA_Channel4_IRQHandler

        PUBWEAK DMA_Channel5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel5_IRQHandler
        B DMA_Channel5_IRQHandler

        PUBWEAK DMA_Channel6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel6_IRQHandler
        B DMA_Channel6_IRQHandler

        PUBWEAK DMA_Channel7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel7_IRQHandler
        B DMA_Channel7_IRQHandler

        PUBWEAK DMA_Channel8_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_Channel8_IRQHandler
        B DMA_Channel8_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_IRQHandler
        B ADC_IRQHandler

        PUBWEAK MMU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MMU_IRQHandler
        B MMU_IRQHandler

        PUBWEAK COMP_1_2_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
COMP_1_2_3_IRQHandler
        B COMP_1_2_3_IRQHandler

        PUBWEAK EXTI9_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI9_5_IRQHandler
        B EXTI9_5_IRQHandler

        PUBWEAK TIM1_BRK_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_BRK_IRQHandler
        B TIM1_BRK_IRQHandler

        PUBWEAK TIM1_UP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_UP_IRQHandler
        B TIM1_UP_IRQHandler
        
        PUBWEAK TIM1_TRG_COM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_TRG_COM_IRQHandler
        B TIM1_TRG_COM_IRQHandler
        
        PUBWEAK TIM1_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_CC_IRQHandler
        B TIM1_CC_IRQHandler
        
        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM2_IRQHandler
        B TIM2_IRQHandler
        
        PUBWEAK TIM3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM3_IRQHandler
        B TIM3_IRQHandler
        
        PUBWEAK TIM4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM4_IRQHandler
        B TIM4_IRQHandler
        
        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_EV_IRQHandler
        B I2C1_EV_IRQHandler
        
        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_ER_IRQHandler
        B I2C1_ER_IRQHandler
        
        PUBWEAK I2C2_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_EV_IRQHandler
        B I2C2_EV_IRQHandler
        
        PUBWEAK I2C2_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_ER_IRQHandler
        B I2C2_ER_IRQHandler
        
        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI1_IRQHandler
        B SPI1_IRQHandler

        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI2_IRQHandler
        B SPI2_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_IRQHandler
        B USART1_IRQHandler
        
        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART2_IRQHandler
        B USART2_IRQHandler
        
        PUBWEAK UART3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART3_IRQHandler
        B UART3_IRQHandler
        
        PUBWEAK EXTI15_10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI15_10_IRQHandler
        B EXTI15_10_IRQHandler
        
        PUBWEAK RTCAlarm_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTCAlarm_IRQHandler
        B RTCAlarm_IRQHandler
        
        PUBWEAK LPTIM_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LPTIM_WKUP_IRQHandler
        B LPTIM_WKUP_IRQHandler
        
        PUBWEAK TIM8_BRK_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_BRK_IRQHandler
        B TIM8_BRK_IRQHandler
        
        PUBWEAK TIM8_UP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_UP_IRQHandler
        B TIM8_UP_IRQHandler
        
        PUBWEAK TIM8_TRG_COM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_TRG_COM_IRQHandler
        B TIM8_TRG_COM_IRQHandler

        PUBWEAK TIM8_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_CC_IRQHandler
        B TIM8_CC_IRQHandler
        
        PUBWEAK UART4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART4_IRQHandler
        B UART4_IRQHandler
        
        PUBWEAK TIM5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM5_IRQHandler
        B TIM5_IRQHandler
        
        PUBWEAK TIM6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM6_IRQHandler
        B TIM6_IRQHandler
        
        PUBWEAK CAN_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN_TX_IRQHandler
        B CAN_TX_IRQHandler
        
        PUBWEAK CAN_RX0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN_RX0_IRQHandler
        B CAN_RX0_IRQHandler
        
        PUBWEAK CAN_RX1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN_RX1_IRQHandler
        B CAN_RX1_IRQHandler
        
        PUBWEAK CAN_SCE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN_SCE_IRQHandler
        B CAN_SCE_IRQHandler

        END

