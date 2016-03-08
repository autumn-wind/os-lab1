#include "syscall.h"

volatile int x = 0;
int main(char *args) {
	while(1) {
		if(x % 10000000 == 0) {
			puts(args);
			puts("Hello World!\n");
		}
 
		x ++;
	}
	return 0;
}
