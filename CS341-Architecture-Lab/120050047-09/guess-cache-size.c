#include <iostream>
#include <stdlib.h>
#include <sys/time.h> 
#define size 1000000
using namespace std;
int main()
{
    int Array[size];
    long long guess_size = 1;
    struct timeval tv;
    for( int j = 0; j < 30; j++)
    {        
        for(long long count = 0; count < guess_size; count++){
            Array[count]=0;
        }
        double avg = 0;
        gettimeofday(&tv,NULL);
        double t1 = tv.tv_sec+(tv.tv_usec/1000000.0);  
        for(long long i=0; i<20000000; i++)
        {
            int temp = Array[(i*97)%guess_size];
        }
        gettimeofday(&tv,NULL);
        double t2 = tv.tv_sec+(tv.tv_usec/1000000.0); 
        cout<< guess_size << '\t' << (t2 - t1)/20000000 << endl;
        guess_size = guess_size*2;
    }
     
}
