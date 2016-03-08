#include "syscall.h"

volatile int x = 0;
char buf[] = "Aloha!!\n";

int main() {
	while(1) {
		if(x % 10000000 == 0) {
			puts(buf);
		}
 
		x ++;
	}
	return 0;
}
