    .syntax unified
    .cpu cortex-m3
    .thumb

    .global g_pfnVectors
    .global Default_Handler

    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr  r0, =_sdata
    ldr  r1, =_edata
    ldr  r2, =_sidata
    movs r3, #0
    b    LoopCopyData
CopyData:
    ldr  r4, [r2, r3]
    str  r4, [r0, r3]
    adds r3, r3, #4
LoopCopyData:
    adds r4, r0, r3
    cmp  r4, r1
    bcc  CopyData

    ldr  r2, =_sbss
    ldr  r4, =_ebss
    movs r3, #0
    b    LoopZeroBss
ZeroBss:
    str  r3, [r2]
    adds r2, r2, #4
LoopZeroBss:
    cmp  r2, r4
    bcc  ZeroBss

    bl   SystemInit
    bl   main
LoopForever:
    b    LoopForever
    .size Reset_Handler, .-Reset_Handler

    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
    b Infinite_Loop
    .size Default_Handler, .-Default_Handler

    .section .isr_vector,"a",%progbits
    .type g_pfnVectors, %object
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDG_IRQHandler
    .word PVD_IRQHandler
    .word TAMPER_IRQHandler
    .word RTC_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA1_Channel1_IRQHandler
    .word DMA1_Channel2_IRQHandler
    .word DMA1_Channel3_IRQHandler
    .word DMA1_Channel4_IRQHandler
    .word DMA1_Channel5_IRQHandler
    .word DMA1_Channel6_IRQHandler
    .word DMA1_Channel7_IRQHandler
    .word ADC1_2_IRQHandler
    .word USB_HP_CAN1_TX_IRQHandler
    .word USB_LP_CAN1_RX0_IRQHandler
    .word CAN1_RX1_IRQHandler
    .word CAN1_SCE_IRQHandler
    .word EXTI9_5_IRQHandler
    .word TIM1_BRK_IRQHandler
    .word TIM1_UP_IRQHandler
    .word TIM1_TRG_COM_IRQHandler
    .word TIM1_CC_IRQHandler
    .word TIM2_IRQHandler
    .word TIM3_IRQHandler
    .word TIM4_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word I2C2_EV_IRQHandler
    .word I2C2_ER_IRQHandler
    .word SPI1_IRQHandler
    .word SPI2_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word USART3_IRQHandler
    .word EXTI15_10_IRQHandler
    .word RTCAlarm_IRQHandler
    .word USBWakeUp_IRQHandler
    .size g_pfnVectors, .-g_pfnVectors

    .irp name, NMI_Handler, HardFault_Handler, MemManage_Handler, BusFault_Handler, UsageFault_Handler, SVC_Handler, DebugMon_Handler, PendSV_Handler, SysTick_Handler, WWDG_IRQHandler, PVD_IRQHandler, TAMPER_IRQHandler, RTC_IRQHandler, FLASH_IRQHandler, RCC_IRQHandler, EXTI0_IRQHandler, EXTI1_IRQHandler, EXTI2_IRQHandler, EXTI3_IRQHandler, EXTI4_IRQHandler, DMA1_Channel1_IRQHandler, DMA1_Channel2_IRQHandler, DMA1_Channel3_IRQHandler, DMA1_Channel4_IRQHandler, DMA1_Channel5_IRQHandler, DMA1_Channel6_IRQHandler, DMA1_Channel7_IRQHandler, ADC1_2_IRQHandler, USB_HP_CAN1_TX_IRQHandler, USB_LP_CAN1_RX0_IRQHandler, CAN1_RX1_IRQHandler, CAN1_SCE_IRQHandler, EXTI9_5_IRQHandler, TIM1_BRK_IRQHandler, TIM1_UP_IRQHandler, TIM1_TRG_COM_IRQHandler, TIM1_CC_IRQHandler, TIM2_IRQHandler, TIM3_IRQHandler, TIM4_IRQHandler, I2C1_EV_IRQHandler, I2C1_ER_IRQHandler, I2C2_EV_IRQHandler, I2C2_ER_IRQHandler, SPI1_IRQHandler, SPI2_IRQHandler, USART1_IRQHandler, USART2_IRQHandler, USART3_IRQHandler, EXTI15_10_IRQHandler, RTCAlarm_IRQHandler, USBWakeUp_IRQHandler
    .weak \name
    .thumb_set \name, Default_Handler
    .endr
