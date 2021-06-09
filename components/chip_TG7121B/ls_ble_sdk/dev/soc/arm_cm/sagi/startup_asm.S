/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.arch	armv7-m

	.section .stack
	.align	3

	.equ	Stack_Size, 0x1000

	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3

	.equ	Heap_Size, 0x2000

	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit


    .section .text
	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	SysTick_Handler
	def_irq_handler	MemManage_Handler
	def_irq_handler	BusFault_Handler
	def_irq_handler	UsageFault_Handler
	def_irq_handler	DebugMon_Handler
	def_irq_handler	RTC1_Handler	
	def_irq_handler	IWDT_Handler	
	def_irq_handler	RTC2_Handler	
	def_irq_handler	EXT_Handler		
	def_irq_handler	LVD33_Handler	
	def_irq_handler	MAC_LP_Handler  
	def_irq_handler	MAC1_Handler		
	def_irq_handler	DMA_Handler
	def_irq_handler	QSPI_Handler
    def_irq_handler SWINT1_Handler
	def_irq_handler	CACHE_Handler	
	def_irq_handler	I8080_Handler
	def_irq_handler	TRNG_Handler	
	def_irq_handler	ECC_Handler		
	def_irq_handler	CRYPT_Handler	
	def_irq_handler	BSTIM1_Handler	
	def_irq_handler	GPTIMA1_Handler	
	def_irq_handler	GPTIMB1_Handler	
	def_irq_handler	GPTIMC1_Handler	
	def_irq_handler	ADTIM1_Handler	
	def_irq_handler	I2C1_Handler	
	def_irq_handler	I2C2_Handler	
	def_irq_handler	I2C3_Handler	
	def_irq_handler	UART1_Handler	
	def_irq_handler	UART2_Handler	
	def_irq_handler UART3_Handler	
	def_irq_handler SPI1_Handler	
	def_irq_handler SPI2_Handler	
	def_irq_handler PDM_Handler		
	def_irq_handler GPIO_Handler	
	def_irq_handler WWDT_Handler	
	def_irq_handler SWINT2_Handler	
	def_irq_handler LCD_Handler
	def_irq_handler ADC_Handler
    def_irq_handler DAC_Handler
    def_irq_handler MAC2_Handler
    def_irq_handler ADC24_Handler
	.end

