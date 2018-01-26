#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>
int main(){
	for(int i = 0; i < 3; i++){
		if(fork()==0){
			sleep(i*10+2);
			printf("thread");
			kill(0,SIGTERM);			
			exit(0);
		}
	}
	while(wait(NULL) != -1){}
	printf("completed");
	exit(0);
}