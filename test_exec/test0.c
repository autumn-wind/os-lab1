#include "syscall.h"

char buf[] = "Aloha!!";
volatile int x = 0;

int main() {
	exec(1, buf);
	while(1) {
		if(x % 10000000 == 0) {
			puts("exec() fails!\n");
		}
		x ++;
	}
	return 0;
}
