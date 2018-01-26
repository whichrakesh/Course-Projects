float sum(float i, float j){
	int l,m; 
	l = (3 * 4) * (1 * 4);
	m = 1;
	return i + j;	
}

int factorial(int n){
	if(n <= 0){
		return 1;
	} else {
		return n * factorial(n-1);
	}
}

void main(){
	int i, k, j, l;
	int Ar[10][5];
	float f,g;

	Ar[1][0] = 1;
	/** checking Arrays and function calls, register save and restore */
	i = 1;
	j = 0;
	g = 2.3;
	f = Ar[Ar[i][j]][sum(1,-1)] + g + sum(3.0,Ar[1][0]); 
	printf("should output 7.3, f=", f);
	
	i = 20;
	j = 3;
	printf("should output 11, ", i - j - j - j);

	/** checking recursion */
	i = factorial(6);
	printf("should output 720, i=",i);

	/* checking long expressions, should not crash */
	l = 8;
	i = 4;
	k = 23;
	j = i * l + k * l / 5 - l;
	l =  i + i + (l + k) + j + (j*k + l);
	printf("should output 1487, l=",l);
	
	/** checking while,for loops and conditional operations*/
	i = 0;
	l = 0;
	while(l < 4){
		printf(l);
		for (k = 0; k < 10; k++){
			if(!(k == 5 || l ) && i > 10)
				printf(k,l,i);
			else
				i = i + k;							
		}
		l = l+1;
	}
	printf(i);

	/** checking Plus plus */
	i = 0;
	printf("should output 1, i=",i++ + i++);
	printf("should output 2, i=", i);

	/** checking operations on relational operations */

	j = (l < 4) + 1;
	printf("should output 1, j=", j);
	i = (l == 4 && i > 0 && j);
	if(i)
		printf("inside for loop, i=" , i);
	else
		;

	/* checking Constant folding and propagation */
	i = 2 + 3 * 5.0;
	printf("should output 17, i=", i);

	/* checking consorrectness during PP or assignment operation */
	i = 0;
	j = i++ + Ar[i][0] + (i=-1);
	printf("should output 0,-1, j=",j," i=",i);
}