.text

main:

	li $t0,1
	addi $t1,$t0,2
	li $v0,1
	move $a0,$t1
	syscall
	
