.data
result: .asciiz "The resulting sorted array is: "
prompt: .asciiz "Enter the integer: "
space: .asciiz " "

.align 2
#array: .word 3,4,5,2,1,8,7,6
array: .space 40
array3: .space 40

.text
#######################################################################################################################
main:
	la $s0,array #input array
	li $s1,8	#array length
	li $t0, 0
	firstwhileloop:
	beq $t0,$s1,firstloopexit #loop to input 8 integers and store it into $s0
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

   # call to merge sort
   la $a0,array
   li $a1,8
   jal mergesort

   li $v0, 4 #syscall to print string
   la $a0, result  #address of string to print
   syscall

   li $t0,0
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

   	li $v0,10 #exit on successful execution
	syscall
################################################################################################################
merge: # merge function
	addi $sp,$sp,-8
	sw $s0,4($sp)
	sw $s1,0($sp)

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
	lw $s0,4($sp)
	lw $s1,0($sp)
	addi $sp,$sp,8
	jr $ra

##############################################################################################################
mergesort:	# merge sort function
			# arguements: a0: pointer to the array, a1: size of the array

	addi $sp,$sp,-24 #increase stack pointer by 24
	sw $s0,20($sp)
	sw $s1,16($sp)
	sw $s2,12($sp)

	sw $ra,8($sp)	#store $ra into stack
	sw $a0,4($sp)	#store $a0 into stack
	sw $a1,0($sp)	#store $a1 into stack
	move $s0,$a1	#size of the array

	li $t0,1
	bne $a1,$t0,mergesortelse #base case
		j mergesortexit		
	mergesortelse:
		div $a1,$a1,2
		jal mergesort 	# merge sort for the first half

		lw $a1,0($sp)
		div $a1,$a1,2
		lw $a0,4($sp)
		sll $t1,$a1,2
		add $a0,$a0,$t1
		jal mergesort # merge sort for the second half

		lw $a0,4($sp)
		lw $a1,0($sp)
		div $a1,$a1,2
		sll $t1,$a1,2
		add $a2,$a0,$t1
		move $a3,$a1

		addi $sp,$sp,-4
	    la $s1,array3
	    sw $s1,0($sp)
	    jal merge
	    addi $sp,$sp,4

	    li $t1,0
	    lw $s2,4($sp) #original array
	    copyloop:
	    beq $t1,$s0,copyexit
	    	lw $t2,0($s1)
	    	sw $t2,0($s2)
	    	addi $s2,$s2,4
	    	addi $s1,$s1,4
	    	addi $t1,$t1,1
	    	j copyloop
	    copyexit:

	mergesortexit:

	lw $ra,8($sp) #restore $ra
	lw $s0,20($sp)
	lw $s1,16($sp)
	lw $s2,12($sp)
	addi $sp,$sp,24	#decrease stack pointer by 20
	jr $ra	#jump to register $ra

########################################################################################################################