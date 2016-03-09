#include "syscall.h"

char buf[] = "Hello World!!";
volatile int x = 0;

int main(char *args) {
	puts(args);
	exec(1, buf);
	while(1) {
		if(x % 10000000 == 0) {
			puts("exec() fails!\n");
		}
		x ++;
	}
	return 0;
}
