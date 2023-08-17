
out/smart_speaker_alg_bl606p_lyeva/yoc.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000054600000 <__stext>:
    54600000:	a815                	j	54600034 <Reset_Handler>
    54600002:	0001                	nop
    54600004:	594b5343          	fmadd.s	ft6,fs6,fs4,fa1,unknown
    54600008:	594b5343          	fmadd.s	ft6,fs6,fs4,fa1,unknown
	...

0000000054600030 <_start>:
    54600030:	0034                	addi	a3,sp,8
    54600032:	5460                	lw	s0,108(s0)

0000000054600034 <Reset_Handler>:
    54600034:	30401073          	csrw	mie,zero
    54600038:	34401073          	csrw	mip,zero
