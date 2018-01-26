	.file	1 "gcd.c"
	.text
	.align	2
	.globl	gcd
	.set	nomips16
	.ent	gcd
gcd:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)              # store $fp into stack
	move	$fp,$sp
	sw	$4,8($fp)               # store $a0 into stack
	sw	$5,12($fp)              # store $a1 into stack
$L5:
	lw	$3,8($fp)               # load $a0 from stack
	lw	$2,12($fp)              # load $a1 from stack
	#nop
	bne	$3,$2,$L2               # if not equal jump to $L2
	lw	$2,8($fp)               # load $a0 from stack
	move	$sp,$fp         
	lw	$fp,4($sp)              # load $a1 from stack
	addiu	$sp,$sp,8               # restore stack
	j	$31                     # return
$L2:
	lw	$3,8($fp)               # load $a0 from stack
	lw	$2,12($fp)              # load $a1 from stack
	#nop
	slt	$2,$2,$3                # if $a1 < $a0, set $a1 to true
	beq	$2,$0,$L3               # if not true jump to L3
	lw	$3,8($fp)               # load $a0 from stack
	lw	$2,12($fp)              # load $a1 from stack
	#nop
	subu	$2,$3,$2                # subtract $a1 from $a0 and store it in $a1
	sw	$2,8($fp)               # store $a1 value in $a0 position in stack
	b	$L5
$L3:
	lw	$3,12($fp)              # load $a1 from stack
	lw	$2,8($fp)               # load $a0 from stack
	#nop
	subu	$2,$3,$2                # subtract $a0 from $a1 and store it in $a0
	sw	$2,12($fp)              # store $a0 value in $a1 position in stack
	b	$L5
	.end	gcd

	.align	2
	.globl	main
	.set	nomips16
	.ent	main
main:
	.frame	$fp,40,$31		# vars= 8, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-40             # increasing stack pointer by 40
	sw	$31,36($sp)             # storing $ra into stack
	sw	$fp,32($sp)             # storing frame pointer into stack
	move	$fp,$sp                 # $fp = $sp
	sw	$4,40($fp)              # storing $a0 into stack
	sw	$5,44($fp)              # storing $a1 into stack
	li	$2,48			# 0x30
	sw	$2,28($fp)              # storing x into stack
	li	$2,15			# 0xf
	sw	$2,24($fp)              # storing y into stack
	lw	$4,28($fp)              # loading x from stack into $a0
	lw	$5,24($fp)              # loading y from stack into $a1
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
