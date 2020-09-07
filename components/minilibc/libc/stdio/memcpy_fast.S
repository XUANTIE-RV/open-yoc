/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#define ALIGN .align 2

#undef ENTRY
#define ENTRY(name) ENTRY_M name
    .macro ENTRY_M name
        .text
        .global \name
        .weak   \name
        .type   \name, @function
        ALIGN
   \name:
    .endm

#undef ENTRY_alias
#define ENTRY_alias(name, alias_name) ENTRY_N name alias_name
    .macro ENTRY_N name  alias_name    //ENTRY_N name
        .text
        .global \alias_name
        .weak   \alias_name
        .set    \alias_name, \name
        .global \name
        .weak   \name
        .type   \name, @function
        ALIGN
   \name:
    .endm



#define DEFAULT_ENTRY_fast_alias(alias_name) ENTRY_alias(__##alias_name##_fast, alias_name)
#define DEFAULT_ENTRY_alias(name) ENTRY(__##name##_size)

#if defined __CK802__ || defined __CK801__ || (defined(__CSKY__) && (__CSKY__ == 1))
.macro 	M_BEZ	rx, label
           cmpnei   \rx, 0
           bf       \label
.endm
.macro 	M_BNEZ	rx, label
           cmpnei   \rx, 0
           bt       \label
.endm
#else
.macro 	M_BEZ   rx, label
          bez   \rx, \label
.endm
.macro 	M_BNEZ  rx, label
          bnez  \rx, \label
.endm
#endif


#ifdef __CK801__
#define t1 l1
#define t0 l2
#define PROLOG push  l0, l1, l2
#define EPILOG pop l0, l1, l2
#define STKSIZE 12
#elif defined(__CK810__) || defined(__CK807__) || defined(__CK804__) || defined(__CK803__)
#define PROLOG
#define EPILOG rts
#define STKSIZE 0
#else
#define t1 t1
#define t0 t0
#define PROLOG push  l0
#define EPILOG pop l0
#define STKSIZE 4
#endif

.macro      GET_FRONT_BITS rx ry
#ifdef      __cskyLE__
    lsr     \rx, \ry
#else
    lsl     \rx, \ry
#endif
.endm

.macro      GET_AFTER_BITS rx ry
#ifdef      __cskyLE__
    lsl     \rx, \ry
#else
    lsr     \rx, \ry
#endif
.endm


.global __memcpy_fast
.global memcpy
memcpy:
__memcpy_fast:
    PROLOG
    .stack_size __memcpy_fast, STKSIZE
#if defined(__CK810__) || defined(__CK807__) || defined(__CK804__) || defined(__CK803__)
    mov     r3, r0
    cmplti  r2, 4                                            /* If len less than 4 bytes */
    jbt     .L_copy_by_byte

    mov     r12, r0
    andi    r12, 3
    bnez    r12, .L_dest_not_aligned                         /* If dest is not 4 bytes aligned */
.L0:
    mov     r12, r1
    andi    r12, 3
    bnez    r12, .L_dest_aligned_but_src_not_aligned         /* If dest is aligned, but src is not aligned */

    cmplti  r2, 16                                           /* dest and src are all aligned */
    jbt     .L_aligned_and_len_less_16bytes                  /* If len less than 16 bytes */

.L_aligned_and_len_larger_16bytes:                           /* src and dst are all aligned, and len > 16 bytes */
    ldw     r18, (r1, 0)
    ldw     r19, (r1, 4)
    ldw     r20, (r1, 8)
    ldw     r21, (r1, 12)
    stw     r18, (r3, 0)
    stw     r19, (r3, 4)
    stw     r20, (r3, 8)
    stw     r21, (r3, 12)
    subi    r2, 16
    addi    r1, 16
    addi    r3, 16
    cmplti  r2, 16
    jbf     .L_aligned_and_len_larger_16bytes

.L_aligned_and_len_less_16bytes:
    cmplti  r2, 4
    jbt     .L_copy_by_byte
    ldw     r18, (r1, 0)
    stw     r18, (r3, 0)
    subi    r2, 4
    addi    r1, 4
    addi    r3, 4
    jbr     .L_aligned_and_len_less_16bytes

.L_copy_by_byte:                                    /* len less than 4 bytes */
    cmpnei  r2, 0
    jbf     .L_return
    ldb     r18, (r1, 0)
    stb     r18, (r3, 0)
    subi    r2, 1
    addi    r1, 1
    addi    r3, 1
    jbr     .L_copy_by_byte

.L_return:
    EPILOG

/* If dest is not aligned, just copying some bytes makes the dest align.
   After that, we judge whether the src is aligned. */

.L_dest_not_aligned:
    rsub    r13, r1, r3                              /* consider overlapped case */
    abs     r13, r13
    cmplt   r13, r2
    jbt     .L_copy_by_byte

.L1:
    ldb     r18, (r1, 0)                             /* makes the dest align. */
    stb     r18, (r3, 0)
    addi    r12, 1
    subi    r2, 1
    addi    r1, 1
    addi    r3, 1
    cmpnei  r12, 4
    jbt     .L1
    cmplti  r2, 4
    jbt     .L_copy_by_byte
    jbf     .L0                                     /* judge whether the src is aligned. */

.L_dest_aligned_but_src_not_aligned:
    rsub    r13, r1, r3                             /* consider overlapped case */
    abs     r13, r13
    cmplt   r13, r2
    jbt     .L_copy_by_byte

    bclri   r1, 0
    bclri   r1, 1
    ldw     r18, (r1, 0)
    addi    r1, 4

    movi    r13, 8
    mult    r13, r12
    mov     r24, r13                                /* r12 is used to store the misaligned bits */
    rsubi   r13, 32
    mov     r25, r13

    cmplti  r2, 16
    jbt     .L_not_aligned_and_len_less_16bytes

.L_not_aligned_and_len_larger_16bytes:
    ldw     r20, (r1, 0)
    ldw     r21, (r1, 4)
    ldw     r22, (r1, 8)
    ldw     r23, (r1, 12)

    GET_FRONT_BITS r18 r24                          /* little or big endian? */
    mov     r19, r20
    GET_AFTER_BITS r20 r25
    or      r20, r18

    GET_FRONT_BITS r19 r24
    mov     r18, r21
    GET_AFTER_BITS r21 r13
    or      r21, r19

    GET_FRONT_BITS r18 r24
    mov     r19, r22
    GET_AFTER_BITS r22 r25
    or      r22, r18

    GET_FRONT_BITS r19 r24
    mov     r18, r23
    GET_AFTER_BITS r23 r25
    or      r23, r19

    stw     r20, (r3, 0)
    stw     r21, (r3, 4)
    stw     r22, (r3, 8)
    stw     r23, (r3, 12)
    subi    r2, 16
    addi    r1, 16
    addi    r3, 16
    cmplti  r2, 16
    jbf     .L_not_aligned_and_len_larger_16bytes

.L_not_aligned_and_len_less_16bytes:
    cmplti  r2, 4
    jbf     .L2
    rsubi   r12, 4                                   /* r12 is used to stored the misaligned bits */
    subu    r1, r12                                  /* initial the position */
    jbr     .L_copy_by_byte
.L2:
    ldw     r21, (r1, 0)
    GET_FRONT_BITS r18 r24
    mov     r19, r18
    mov     r18, r21
    GET_AFTER_BITS r21 r25
    or      r21, r19
    stw     r21, (r3, 0)
    subi    r2, 4
    addi    r1, 4
    addi    r3, 4
    jbr     .L_not_aligned_and_len_less_16bytes

#else

    mov	t0, a0
#ifdef __CK801__
    mov     l0, a1
    or  l0, l0, t0
    movi    a3, 3
    and l0, l0, a3
#else
    or  t1, a1, a0
    andi    l0, t1, 3
#endif
    M_BEZ   l0, .L23
    M_BEZ   a2, .L5
.L11:
    ld.b    a3, (a1)
    addi    a1, a1, 1
    subi    a2, a2, 1
    st.b    a3, (t0)
    addi    t0, t0, 1
    M_BNEZ  a2, .L11
.L5:
    EPILOG
.L23:
    cmplti  a2, 16
    bt  .L20
.L14:
    ld.w    t1, (a1)
    ld.w    a3, (a1, 4)
    ld.w    l0, (a1, 8)
    st.w    t1, (t0)
    ld.w    t1, (a1, 12)
    st.w    a3, (t0, 4)
    st.w    l0, (t0, 8)
    st.w    t1, (t0, 12)
    subi    a2, a2, 16
    addi    a1, a1, 16
    addi    t0, t0, 16
    cmplti  a2, 16
    bf  .L14
.L20:
    cmplti  a2, 4
    bt  .L21
.L13:
    ld.w    a3, (a1)
    subi    a2, a2, 4
    addi    a1, a1, 4
    st.w    a3, (t0)
    addi    t0, t0, 4
    cmplti  a2, 4
    bf  .L13
.L21:
    M_BEZ   a2, .L5
    ld.b    a3, (a1)
    addi    a1, a1, 1
    subi    a2, a2, 1
    st.b    a3, (t0)
    addi    t0, t0, 1
    br  .L21
#endif

        .size   __memcpy_fast, .-__memcpy_fast

