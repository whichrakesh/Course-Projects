.text

main:

	lui $t0,0x1000
	ori $t0, $t0, 0x0001

	lui $t1,0x2000
	ori $t1,$t1,0x0002

	add $t2,$t1,$t0


	li $v0,1
	move $a0,$t2
	syscall
	
