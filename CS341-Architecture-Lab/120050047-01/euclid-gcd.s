.data
prompt1: .asciiz "\n\n Enter the first integer please:"
prompt2: .asciiz "Enter the second integer please:"
result: .asciiz "The result is:"

.text

main:
	li $v0, 4 #syscall to print string
    la $a0, prompt1  #address of string to print
    syscall

    li $v0, 5 #syscall to read an integer
    syscall
    move $s0, $v0  #move the number to read into $t0

    #second number
    li $v0, 4
    la $a0, prompt2
    syscall

    li $v0,5        
    syscall
    move $s1,$v0

	ori $s2,$0,0

	loop:
		beq $s1,$0,exit
		slt $t0,$s0,$s1
		beq $t0,$0,exit1
			addi $s2,$s0,0
			addi $s0,$s1,0
			addi $s1,$s2,0
		exit1:
		addi $s2,$s1,0
		sub $s1,$s0,$s1
		addi $s0,$s2,0
		j loop	
	exit:

	li $v0, 4
    la $a0, result
    syscall

	li $v0,1
	move $a0,$s0
	syscall
	
