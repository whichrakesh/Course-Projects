	.file	1 "gcd.c"
	.section .mdebug.abi32
	.previous
	.abicalls
	.text
	.align	2
	.globl	gcd
	.set	nomips16
	.ent	gcd
	.type	gcd, @function
gcd:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	sw	$5,12($fp)
$L5:
	lw	$3,8($fp)
	lw	$2,12($fp)
	#nop
	bne	$3,$2,$L2
	lw	$2,8($fp)
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	j	$31
$L2:
	lw	$3,8($fp)
	lw	$2,12($fp)
	#nop
	slt	$2,$2,$3
	beq	$2,$0,$L3
	lw	$3,8($fp)
	lw	$2,12($fp)
	#nop
	subu	$2,$3,$2
	sw	$2,8($fp)
	b	$L5
$L3:
	lw	$3,12($fp)
	lw	$2,8($fp)
	#nop
	subu	$2,$3,$2
	sw	$2,12($fp)
	b	$L5
	.end	gcd
	.size	gcd, .-gcd

	.comm	result,4,4
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	.frame	$fp,40,$31		# vars= 8, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	.cprestore	16
	sw	$4,40($fp)
	sw	$5,44($fp)
	li	$2,48			# 0x30
	sw	$2,28($fp)
	li	$2,15			# 0xf
	sw	$2,24($fp)
	lw	$4,28($fp)
	lw	$5,24($fp)
	jal	gcd
	move	$3,$2
	la	$2,result
	sw	$3,0($2)
	la	$2,result
	lw	$2,0($2)
	#nop
	move	$4,$2
	jal	syscall_print_int
	jal	syscall_print_newline
	move	$2,$0
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	j	$31
	.end	main
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7"
