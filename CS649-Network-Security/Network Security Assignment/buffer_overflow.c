#include <stdio.h>
main( ) {
	int i = 1;
	A(5, 3);
	i = 2;
	printf("%d",  i);
}

A(int m, int n)  {
	char *ptr;
	char buf1[5];
 	int z = m + n;
	char buf2[10];
	ptr = buf2 + 26;
	//printf("%p\n%p\n%p\n",ptr,buf1,buf2);
	*ptr += 8;
}
