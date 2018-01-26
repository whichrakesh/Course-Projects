#include <iostream>
#include <unistd.h>
#include <cmath>

using namespace std;

int main() {
	for ( long long int i = 0 ; i <= 10000000000 ; i++ ) {
		cout << pow (i, 5) << "\n";
	}
	
		
	sleep(300);
	return 0;
}
