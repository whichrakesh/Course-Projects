#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
	for ( long long i = 0 ; i <= 10000000000 ; i++ ) {
		long long x = 1 ;
		for ( long int j = 1 ; j <= 5 ; j++ ) 
			x *= i ;
		cout << x << "\n" ;
	}
	
		
	sleep(300);
	return 0;
}
