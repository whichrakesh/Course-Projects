.data
promptz: .asciiz "enter a postive number"

.text
main:
	li $v0,4 #print promptz
	la $a0,promptz
	syscall

	li $v0,5 #input integer n
	syscall

	move $a0,$v0 #pass argument to factorial
	jal fact

	addi $a0,$v0,0 #print fact(n)
	li $v0,1
	syscall

	li $v0,10 #exit on successful execution
	syscall

fact:	#factorial function
	addi $sp,$sp,-8 #decrease stack pointer by 8
	sw $ra,4($sp)	#store $ra into stack
	sw $a0,0($sp)	#store $a0 into stack
	bne $a0,$0,else
		li $v0,1   #set return value to be 1
		j exit     
	else:
		addi $a0,$a0,-1 
		jal fact
		lw $a0,0($sp)
		mul $v0,$a0,$v0 #fact(n) = n * fact(n-1)
	exit:
	lw $ra,4($sp) #restore $ra
	addi $sp,$sp,8	#increase stack pointer by 8
	jr $ra	#jump to register $ra
