	.file	"main.c"
	.text
	.type	sti, @function
sti:
.LFB7:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
#APP
# 96 "/home/doge/os-labs/os-lab1/include/x86/cpu.h" 1
	sti
# 0 "" 2
#NO_APP
	nop
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	sti, .-sti
	.type	cli, @function
cli:
.LFB8:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
#APP
# 102 "/home/doge/os-labs/os-lab1/include/x86/cpu.h" 1
	cli
# 0 "" 2
#NO_APP
	nop
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	cli, .-cli
	.type	wait_intr, @function
wait_intr:
.LFB9:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
#APP
# 108 "/home/doge/os-labs/os-lab1/include/x86/cpu.h" 1
	hlt
# 0 "" 2
#NO_APP
	nop
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	wait_intr, .-wait_intr
	.globl	xxx
	.data
	.align 4
	.type	xxx, @object
	.size	xxx, 4
xxx:
	.long	2
	.section	.rodata
.LC0:
	.string	"main.c"
	.align 4
.LC1:
	.string	"\n\033[1;31msystem panic\033[0m in function \"%s\", line %d, file \"%s\":\n"
.LC2:
	.string	"0"
	.align 4
.LC3:
	.string	"assertion fail (%s) in function \"%s\", line %d, file \"%s\"\n"
	.text
	.globl	os_init
	.type	os_init, @function
os_init:
.LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	$1, xxx
	call	init_page
	movl	$-1073741824, %eax
	movl	$os_init_cont, %edx
#APP
# 28 "main.c" 1
	 addl %eax, %esp
						jmp *%edx
# 0 "" 2
#NO_APP
	call	cli
	pushl	$.LC0
	pushl	$31
	pushl	$__FUNCTION__.1691
	pushl	$.LC1
	call	printk
	addl	$16, %esp
	subl	$12, %esp
	pushl	$.LC0
	pushl	$31
	pushl	$__FUNCTION__.1691
	pushl	$.LC2
	pushl	$.LC3
	call	printk
	addl	$32, %esp
.L5:
	call	wait_intr
	jmp	.L5
	.cfi_endproc
.LFE14:
	.size	os_init, .-os_init
	.globl	os_init_cont
	.type	os_init_cont, @function
os_init_cont:
.LFB15:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	call	init_segment
	call	init_serial
	call	init_idt
	call	init_intr
	call	init_proc
	call	welcome
	call	sti
.L7:
	call	wait_intr
	jmp	.L7
	.cfi_endproc
.LFE15:
	.size	os_init_cont, .-os_init_cont
	.section	.rodata
.LC4:
	.string	"Hello, OS World!\n"
	.text
	.globl	welcome
	.type	welcome, @function
welcome:
.LFB16:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC4
	call	printk
	addl	$16, %esp
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE16:
	.size	welcome, .-welcome
	.section	.rodata
	.align 4
	.type	__FUNCTION__.1691, @object
	.size	__FUNCTION__.1691, 8
__FUNCTION__.1691:
	.string	"os_init"
	.ident	"GCC: (GNU) 5.3.0"
	.section	.note.GNU-stack,"",@progbits
