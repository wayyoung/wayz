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

Stack_Size      EQU     0x00001500

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp
                                                  
; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000300

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ; Top of Stack
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
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY
                
; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler              [WEAK]
                IMPORT  __main
                IMPORT  System_Initializes
                LDR     R0, =System_Initializes
                BLX     R0               
                LDR     R0, =__main
                BX      R0
                ENDP
                
; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                 [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler           [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler           [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler            [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler          [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                 [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler            [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler              [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler             [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  WWDG_IRQHandler             [WEAK]
                EXPORT  PVD_IRQHandler              [WEAK]
                EXPORT  RTC_TAMPER_STAMP_IRQHandler [WEAK]
                EXPORT  RTC_WKUP_IRQHandler         [WEAK]
                EXPORT  FLASH_IRQHandler            [WEAK]
                EXPORT  RCC_IRQHandler              [WEAK]
                EXPORT  EXTI0_IRQHandler            [WEAK]
                EXPORT  EXTI1_IRQHandler            [WEAK]
                EXPORT  EXTI2_IRQHandler            [WEAK]
                EXPORT  EXTI3_IRQHandler            [WEAK]
                EXPORT  EXTI4_IRQHandler            [WEAK]
                EXPORT  DMA_Channel1_IRQHandler     [WEAK]
                EXPORT  DMA_Channel2_IRQHandler     [WEAK]
                EXPORT  DMA_Channel3_IRQHandler     [WEAK]
                EXPORT  DMA_Channel4_IRQHandler     [WEAK]
                EXPORT  DMA_Channel5_IRQHandler     [WEAK]
                EXPORT  DMA_Channel6_IRQHandler     [WEAK]
                EXPORT  DMA_Channel7_IRQHandler     [WEAK]
                EXPORT  DMA_Channel8_IRQHandler     [WEAK]
                EXPORT  ADC_IRQHandler              [WEAK]
                EXPORT  MMU_IRQHandler              [WEAK]
                EXPORT  COMP_1_2_3_IRQHandler       [WEAK]
                EXPORT  EXTI9_5_IRQHandler          [WEAK]
                EXPORT  TIM1_BRK_IRQHandler         [WEAK]
                EXPORT  TIM1_UP_IRQHandler          [WEAK]
                EXPORT  TIM1_TRG_COM_IRQHandler     [WEAK]
                EXPORT  TIM1_CC_IRQHandler          [WEAK]
                EXPORT  TIM2_IRQHandler             [WEAK]
                EXPORT  TIM3_IRQHandler             [WEAK]
                EXPORT  TIM4_IRQHandler             [WEAK]
                EXPORT  I2C1_EV_IRQHandler          [WEAK]
                EXPORT  I2C1_ER_IRQHandler          [WEAK]
                EXPORT  I2C2_EV_IRQHandler          [WEAK]
                EXPORT  I2C2_ER_IRQHandler          [WEAK]
                EXPORT  SPI1_IRQHandler             [WEAK]
                EXPORT  SPI2_IRQHandler             [WEAK]
                EXPORT  USART1_IRQHandler           [WEAK]
                EXPORT  USART2_IRQHandler           [WEAK]
                EXPORT  UART3_IRQHandler            [WEAK]
                EXPORT  EXTI15_10_IRQHandler        [WEAK]
                EXPORT  RTCAlarm_IRQHandler         [WEAK]
                EXPORT  LPTIM_WKUP_IRQHandler       [WEAK]
                EXPORT  TIM8_BRK_IRQHandler         [WEAK]
                EXPORT  TIM8_UP_IRQHandler          [WEAK]
                EXPORT  TIM8_TRG_COM_IRQHandler     [WEAK]
                EXPORT  TIM8_CC_IRQHandler          [WEAK]
                EXPORT  UART4_IRQHandler            [WEAK]
                EXPORT  TIM5_IRQHandler             [WEAK]
                EXPORT  TIM6_IRQHandler             [WEAK]
                EXPORT  CAN_TX_IRQHandler           [WEAK]
                EXPORT  CAN_RX0_IRQHandler          [WEAK]
                EXPORT  CAN_RX1_IRQHandler          [WEAK]
                EXPORT  CAN_SCE_IRQHandler          [WEAK]

WWDG_IRQHandler
PVD_IRQHandler
RTC_TAMPER_STAMP_IRQHandler
RTC_WKUP_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA_Channel1_IRQHandler
DMA_Channel2_IRQHandler
DMA_Channel3_IRQHandler
DMA_Channel4_IRQHandler
DMA_Channel5_IRQHandler
DMA_Channel6_IRQHandler
DMA_Channel7_IRQHandler
DMA_Channel8_IRQHandler
ADC_IRQHandler
MMU_IRQHandler
COMP_1_2_3_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
UART3_IRQHandler
EXTI15_10_IRQHandler
RTCAlarm_IRQHandler
LPTIM_WKUP_IRQHandler
TIM8_BRK_IRQHandler
TIM8_UP_IRQHandler
TIM8_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
UART4_IRQHandler
TIM5_IRQHandler
TIM6_IRQHandler
CAN_TX_IRQHandler
CAN_RX0_IRQHandler
CAN_RX1_IRQHandler
CAN_SCE_IRQHandler
                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB
                
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
                
                 ELSE
                
                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap
                 
__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END
