Group members: Rakesh Ranjan Nayak
			   Suman Sourabh

Implementation details:
	1. Sethi Ulman algorithm taking into consideration associative and commutative property of operators.
	2. constant propagation and folding
	3. handled relational and logical operations using truelist, falselist and backpatch
	4. avoided jump to jump using nextlist for statements
	5. saved those registers which are used till now while calling a function and restoring after function calls.
	6. separate printf implementaion which prints string, int and float separated by comma.
	7. handled arithmatic and assignment operations on relational and logical operations such as i = (b < c) or (b < c) + 2.
	8. conversion of integers to boolean.
	for example: if(x) or (x and y < 4)
	9. handled PP operator as Clang handles it.
	10. handled recursions in functions
	11. macros for binary operations to reduce code size.
	12. used array indexing similar to gcc
