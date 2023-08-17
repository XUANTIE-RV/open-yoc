
out/bl606p_c906_ipc_demo/yoc.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000054c00000 <__stext>:
    54c00000:	a815                	j	54c00034 <Reset_Handler>
    54c00002:	0001                	nop
    54c00004:	594b5343          	fmadd.s	ft6,fs6,fs4,fa1,unknown
    54c00008:	594b5343          	fmadd.s	ft6,fs6,fs4,fa1,unknown
	...

0000000054c00030 <_start>:
    54c00030:	0034                	addi	a3,sp,8
    54c00032:	54c0                	lw	s0,44(s1)

0000000054c00034 <Reset_Handler>:
    54c00034:	30401073          	csrw	mie,zero
    54c00038:	34401073          	csrw	mip,zero
