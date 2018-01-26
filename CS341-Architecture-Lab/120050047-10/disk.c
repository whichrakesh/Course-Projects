#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <unistd.h>
using namespace std;
int main(){
    int fd = open("/dev/sda",O_RDONLY );
    void* buffer;
    struct timeval tv;
    int SINGLE_SECTOR = 512;
    int measurements = 10000;
    for(int i = 0; i<measurements; i = i + 11){
        gettimeofday(&tv,NULL);
        double t1 = tv.tv_sec+(tv.tv_usec/1000000.0);
        
        lseek(fd, i*SINGLE_SECTOR, SEEK_CUR);
        read(fd,buffer, SINGLE_SECTOR);
        
        gettimeofday(&tv,NULL);
        double t2 = tv.tv_sec+(tv.tv_usec/1000000.0); 
        cout<< i << " " << (t2 - t1) << endl;
    }
    close(fd);
}
