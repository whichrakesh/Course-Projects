.data
prompt1: .asciiz "Enter the first integer:"
prompt2: .asciiz "Enter the second integer:"
prompt: .asciiz " Enter the integer to be stored in the array:"
result: .asciiz "The sum is:"

.align 2
array: .space 32 

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

    add $t1,$s0,$s1

    li $v0, 4 #syscall to print the result string
    la $a0, result  #address of string to print
    syscall

	li $v0,1
	move $a0,$t1
	syscall

	la $s0,array

	li $v0,1
	move $a0,$s0
	syscall

	li $t0, 0
	loop:
	beq $t0,8,loopexit
		li $v0, 4 #syscall to print string
	    la $a0, prompt  #address of string to print
	    syscall

	    li $v0, 5 #syscall to read an integer
	    syscall

	    sll $t1,$t0,2
	    add $t1,$s0,$t1
	    sw $v0,0($t1)

	    addi $t0,$t0,1
	    j loop
	loopexit:

	
