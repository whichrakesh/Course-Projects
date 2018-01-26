	.file	1 "gcd.c"
	.text
	.align	2
	.globl	gcd
	.set	nomips16
	.ent	gcd
gcd:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	move	$2,$4           # $v0 = $a0                    
	bne	$4,$5,$L7           # jump to L7 if $a0 != $a1
	j	$31                 # else return
$L5:
	bne	$5,$2,$L7           # jump to L7 if $a1 != $v0 
	j	$31                 # else return 
$L7:
	slt	$3,$5,$2            # set $v1 to true if $a1 < $v0
	beq	$3,$0,$L4           # if not true jump to $L4
	subu	$2,$2,$5        # else set $v0 = $v0  - $a1
	b	$L5
$L4:
	subu	$5,$5,$2        # set $a1 = $a1 - $v0
	b	$L5          
	.end	gcd
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
main:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 16, gp= 8
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	li	$4,48			# 0x30
	li	$5,15			# 0xf
	jal	gcd
	la	$3,result
	sw	$2,0($3)
	move	$4,$2
	jal	syscall_print_int
	jal	syscall_print_newline
	move	$2,$0
	lw	$31,28($sp)
	addiu	$sp,$sp,32
	j	$31
	.end	main

