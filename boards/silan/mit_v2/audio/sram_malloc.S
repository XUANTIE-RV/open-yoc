/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#define SRAM_HEAP_SIZE 0x400

    .section .sram.bss
    .align 2
__sram_heap_start:
    .space SRAM_HEAP_SIZE
__sram_heap_end:


    .section .sram.data
    .align  2
    .type   has_initialized, @object
    .size   has_initialized, 4
has_initialized:
    .long   0
    .type   F, @object
    .size   F, 4
F:
    .long   0

.import __malloc_lock
.import os_critical_enter
.import os_critical_exit

.section .sram.text
.global sram_malloc
.type   sram_malloc, @function
sram_malloc:
#if __CSKY__==2
	push	l0-l2
    .stack_size sram_malloc, 12

	mov	l0, a0
	mov	l1, lr
//	lrw	a0, __malloc_lock
//	jbsr	os_critical_enter
	mov	a0, l0
	mov	lr, l1

	lrw	a1, has_initialized
	ldw	l0, (a1)
	cmpnei	l0, 0
	bt	.LM0

	//malloc init			
	lrw	a2, __sram_heap_start		// __sram_heap_start 
	stw	a2, (a1, 4)
	stw	l0, (a2)
	lrw	a3, __sram_heap_end		// __sram_heap_end
	subu	a3, a2	
	subi	a3, 8				// heap length
	stw	a3, (a2, 4)
	movi	l0, 1
	stw	l0, (a1)

.LM0:
	addi	l0, a0, 7	
    movi    a0, 7
	andn    l0, a0			//size = (size+7) & (~7)
	
        // traversing the linked list to find a suitable block that is min
        // and big enough for the allocation.
	movi	a0, 0	
	addi	l1, a1, 4	
	ldw	a1, (a1, 4)	
	br	.LM2
.LM1:	
	ldw	a2, (a1, 4)
	cmphs	a2, l0
	bf	.LM4
	cmpnei	a0, 0
	bf	.LM3
	cmphs	a2, a3
	bt	.LM4
.LM3:	
	mov	a3, a2
	mov	l2, l1
	mov	a0, a1
.LM4:
	mov	l1, a1
	ldw	a1, (a1)

.LM2:
	cmpnei	a1, 0
	bt	.LM1

	// if we find one, we try to split this block
	cmpnei	a0, 0
	bf	.LMRETURN			// we have not find one block.

	ldw	a1, (a0, 4)
	subu	a1, a1, l0
	cmplti	a1, 0xf			// len should bigger than 16
	ldw	a1, (a0)
	// if lack of space, the entire allocation out.
	bt      .LM6

	addi	a2, l0, 8
	addu	a2, a0, a2
	stw	a1, (a2)
	ldw	a1, (a0, 4)
	subi	a1, 8
	subu	a1, l0
	stw	a1, (a2, 4)
	stw	a2, (l2)
	stw	l0, (a0, 4)
	br	.LM7
.LM6:
	stw	a1, (l2)
.LM7:
	addi	a0, 8	

.LMRETURN:
	mov	l0, a0
	mov	l1, lr
//	lrw	a0, __malloc_lock
//	jbsr	os_critical_exit
	mov	a0, l0
	mov	lr, l1

	pop	l0-l2

#else 		// __CSKY__==1
	subi	sp, 16
	stw	l0, (sp, 0)
	stw	l1, (sp, 4)
	stw	l2, (sp, 8)
    .stack_size sram_malloc, 16

	mov	l0, a0
	mov	l1, lr
//	lrw	a0, __malloc_lock
//	jbsr	os_critical_enter
	mov	a0, l0
	mov	lr, l1

        lrw     a1, has_initialized
        ldw     l0, (a1)
        cmpnei  l0, 0
        bt      .LM0

        //malloc init
        lrw     a2, __sram_heap_start         // __sram_heap_start
        stw     a2, (a1, 4)
        stw     l0, (a2)
        lrw     a3, __sram_heap_end           // __sram_heap_end
        subu    a3, a2
        subi    a3, 8                           // heap length
        stw     a3, (a2, 4)
        movi    l0, 1
        stw     l0, (a1)

.LM0:
        mov	l0, a0
	addi    l0, 7       
        lsri    l0, 3
        lsli    l0, 3                   //size = (size+7) & (~7)

        // traversing the linked list to find a suitable block that is min
        // and big enough for the allocation.
        movi    a0, 0
	mov	l1, a1   
        addi    l1, 4       
        ldw     a1, (a1, 4)
        br      .LM2
.LM1:   
        ldw     a2, (a1, 4)
        cmphs   a2, l0
        bf      .LM4
        cmpnei  a0, 0
        bf      .LM3
        cmphs   a2, a3
        bt      .LM4
.LM3:
        mov     a3, a2
        mov     l2, l1
        mov     a0, a1
.LM4:
        mov     l1, a1
        ldw     a1, (a1)

.LM2:
        cmpnei  a1, 0
        bt      .LM1

        // if we find one, we try to split this block
        cmpnei  a0, 0
        bf      .LMRETURN                        // we have not find one block.

        ldw     a1, (a0, 4)
        subu    a1, l0
		movi	a2, 0xf
        cmplt  a1, a2                 // len should bigger than 16
        ldw     a1, (a0)
       // if lack of space, the entire allocation out.
        bt      .LM6

	mov	a2, l0
        addi    a2, 8
        addu    a2, a0
        stw     a1, (a2)
        ldw     a1, (a0, 4)
        subi    a1, 8
        subu    a1, l0
        stw     a1, (a2, 4)
        stw     a2, (l2)
        stw     l0, (a0, 4)
        br      .LM7
.LM6:
        stw     a1, (l2)
.LM7:
        addi    a0, 8

.LMRETURN:
	mov	l0, a0
	mov	l1, lr
//	lrw	a0, __malloc_lock
//	jbsr	os_critical_exit
	mov	a0, l0
	mov	lr, l1

	ldw	l0, (sp, 0)
	ldw	l1, (sp, 4)
	ldw	l2, (sp, 8)
	addi	sp, 16
	rts
#endif			// __CSKY__==2

        .size   sram_malloc, .-sram_malloc

.section .sram.text
.global sram_free
.type   sram_free, @function
sram_free:
#if __CSKY__==2
	push	l0-l1
#else		// __CSKY__==1
	subi	sp, 8
	stw	l0, (sp, 0)
	stw	l1, (sp, 4)
#endif		// __CSKY__==2

	mov	l0, a0
	mov	l1, lr
//	lrw	a0, __malloc_lock
//	jbsr	os_critical_enter
	mov	a0, l0
	mov	lr, l1

	// if free pointer is 0, return
	cmpnei	a0, 0
	bf	.LFRETURN
	
#if __CSKY__==2
	subi	a2, a0, 8
#else           // __CSKY__==1
	mov	a2, a0
	subi	a2, 8
#endif          // __CSKY__==2
	ldw	a3, (a2, 4)
	addu	a0, a3
	
	// traversing the linked list to check whether the block can be merged
	lrw     l0, F
	ldw	a3, (l0)

	// if the block's top is someone's bottom.
.LF5:
	cmpne	a0, a3
	bt 	.LF0
	ldw	a3, (a0, 4)
	ldw	a1, (a2, 4)
	addu	a1, a3
	addi	a1, 8
	stw	a1, (a2, 4)
	ldw	a3, (a0)	
.LF0:
	cmpnei	a3, 0
	bf	.LF1
	ldw	a1, (a3, 4)
	addi	a1, 8
	addu	a1, a3
	br	.LF2
.LF1:
	mov	a1, a3
.LF2:
	// if the block's bottom is someone's top.
	cmpne	a1, a2
	bt	.LF3
	ldw	a0, (a3, 4)
	ldw	a2, (a1, 4)
	addu	a0, a2
	addi	a0, 8
	stw	a0, (a3, 4)
	movi	a2, 0
	stw	a2, (a1, 4)
	stw	a3, (a1)
	stw	a3, (l0)
	br	.LFRETURN
.LF3:
	cmphs	a1, a2
	bt	.LF4
	stw	a2, (l0)
	stw	a3, (a2)
	br 	.LFRETURN
.LF4:
	mov	l0, a3
	ldw	a3, (a3)
	br	.LF5
.LFRETURN:
//	lrw	a0, __malloc_lock
	mov	l1, lr
//	jbsr	os_critical_exit
	mov	lr, l1

#if __CSKY__==2
	pop	l0-l1
#else		// __CSKY__==1
	ldw	l0, (sp, 0)
	ldw	l1, (sp, 4)
	addi	sp, 8
	rts
#endif		// __CSKY__==2
        .size   sram_free, .-sram_free


.section .sram.text
.global sram_calloc
.type   sram_calloc, @function
sram_calloc:
#if __CSKY__==2
	push	l0, r15
#else           // __CSKY__==1
	subi	sp, 8
	stw	l0, (sp, 0)
	stw	r15, (sp, 4)
#endif		// __CSKY__==2
	mult	a0, a1
	mov	l0, a0
	jbsr	malloc
	cmplti  l0, 1
        bt      .LCRETURN
	cmpnei	a0, 0
	bf	.LCRETURN
	movi	a1, 0
	mov	a2, l0
	jbsr	memset
.LCRETURN:
#if __CSKY__==2
	pop	l0, r15
#else           // __CSKY__==1
        ldw     l0, (sp, 0)
	ldw	r15, (sp, 4)
        addi    sp, 8
        rts
#endif          // __CSKY__==2
        .size   sram_calloc, .-sram_calloc

.section .sram.text
.global sram_realloc
.type   sram_realloc, @function
sram_realloc:
#if __CSKY__==2
	push	l0-l2, r15
#else           // __CSKY__==1
	subi	sp, 16
	stw		l0, (sp, 0)
	stw		l1, (sp, 4)
	stw		l2, (sp, 8)
	stw		r15, (sp, 12)
#endif          // __CSKY__==2
	
	mov		l0, a0
	mov		l1, a1
	mov		a0, a1
	jbsr		malloc
	mov		l2, a0
	lrw		a1, __sram_heap_start
	cmphs		l0, a1
	bf		.LRRETURN
	lrw     	a1, __sram_heap_end
	cmphs		a1, l0
	bf		.LRRETURN
	cmpnei  	a0, 0
	bf  		.LRRETURN
	mov		a1, l0
	mov		a2, l1
	jbsr		memcpy
	mov		a0, l2
.LRRETURN:

#if __CSKY__==2
	pop		l0-l2, r15
#else           // __CSKY__==1
    ldw     l0, (sp, 0)
    ldw     l1, (sp, 4)
	ldw		l2, (sp, 8)
    ldw     r15, (sp, 12)
	addi	sp, 16	
	rts
#endif          // __CSKY__==2
        .size   sram_realloc, .-sram_realloc
