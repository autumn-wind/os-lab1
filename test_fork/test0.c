#include "syscall.h"

volatile int x = 0;
int main(char *args) {
	int n = fork();
	while(1) {
		if(x % 10000000 == 0) {
			puts(args);
			if(n){
				puts("Parent process is running.\n");
			}else{
				puts("Child process is running.\n");
			}
		}
		x ++;
	}
	return 0;
}
