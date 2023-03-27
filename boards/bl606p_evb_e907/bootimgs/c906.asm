
out/bl606p_c906_ipc_demo/yoc.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000054500000 <__stext>:
    54500000:	a815                	j	54500034 <Reset_Handler>
    54500002:	0001                	nop
    54500004:	594b5343          	fmadd.s	ft6,fs6,fs4,fa1,unknown
    54500008:	594b5343          	fmadd.s	ft6,fs6,fs4,fa1,unknown
	...

0000000054500030 <_start>:
    54500030:	0034                	addi	a3,sp,8
    54500032:	5450                	lw	a2,44(s0)

0000000054500034 <Reset_Handler>:
    54500034:	30401073          	csrw	mie,zero
    54500038:	34401073          	csrw	mip,zero
