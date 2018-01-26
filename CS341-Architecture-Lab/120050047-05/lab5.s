.data
temp : .asciiz "rakesh"
#.align 2
arrayi : .space 32

.text

main :
	la $s0, arrayi
	li $t7, 12
	sw $t7, 0($s0)