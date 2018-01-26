.data
result: .asciiz "The result array is:"
prompt: .asciiz " Enter the integer to be stored in the array:"
space: .asciiz " "

.align 2
array: .space 40
array3: .space 40

.text

main:
	la $s0,array
	li $t0, 0
	firstwhileloop:
	beq $t0,8,firstloopexit
		li $v0, 4 #syscall to print string
	    la $a0, prompt  #address of string to print
	    syscall

	    li $v0, 5 #syscall to read an integer
	    syscall

	    sll $t1,$t0,2
	    add $t1,$s0,$t1
	    sw $v0,0($t1)

	    addi $t0,$t0,1
	    j firstwhileloop
	firstloopexit:

   la $a0,array
   li $a1,4
   add $a2,$a0,16
   li $a3,4

   addi $sp,$sp,-4
   la $t0,array3
   sw $t0,0($sp)
   
   jal merge
   move $s0,$v0

   li $v0, 4 #syscall to print string
   la $a0, result  #address of string to print
   syscall

   li $t0,0
   li $s1,8
   while:
   		beq $t0,$s1,whileexit

   			li $v0, 4 #syscall to print string
   			la $a0, space #address of string to print
  			syscall

   			li $v0,1
   			sll $t1,$t0,2
   			add $t3,$t1,$s0
   			lw $a0,0($t3)
   			syscall 

   			addi $t0,$t0,1
   			j while
   	whileexit:
   	addi $sp,$sp,4

   	li $v0,10 #exit on successful execution
	syscall

merge: 
	li $s0,0
	li $s1,0
	lw $v0,0($sp)
	whileloop:
		slt $t0,$s0,$a1
		slt $t1,$s1,$a3
		and $t2,$t0,$t1
		beq $t2,$0,loopexit

			sll $t0,$s0,2
			add $t0,$t0,$a0

			sll $t1,$s1,2
			add $t1,$t1,$a2

			lw $t0,0($t0)
			lw $t1,0($t1)

			add $t3,$s0,$s1
			sll $t3,$t3,2
			add $t3,$t3,$v0

			slt $t2,$t0,$t1

			beq $t2,$0,else				
				sw $t0,0($t3)
				addi $s0,$s0,1
				j exit
			else:
				sw $t1,0($t3)
				addi $s1,$s1,1
			exit:
		j whileloop	
	loopexit:
	bne $s0,$a1,else1
		while1:
		beq $s1,$a3,exit1
			add $t3,$s0,$s1
			sll $t3,$t3,2
			add $t3,$t3,$v0

			sll $t1,$s1,2
			add $t1,$t1,$a2
			lw $t1,0($t1)

			sw $t1,0($t3)
			addi $s1,$s1,1
			j while1
	else1:
		while2:
		beq $s0,$a1,exit1
			add $t3,$s0,$s1
			sll $t3,$t3,2
			add $t3,$t3,$v0

			sll $t0,$s0,2
			add $t0,$t0,$a0
			lw $t0,0($t0)

			sw $t0,0($t3)
			addi $s0,$s0,1
			j while2
	exit1:
	jr $ra


