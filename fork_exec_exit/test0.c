#include "syscall.h"

char buf[] = "Aloha!!";
volatile int x = 0;

int main() {
	while(1) {
		if(x % 1000000000 == 0) {
			int n = fork();
			if(n == 0){
				exec(1, buf);
			}
		}
		x ++;
	}
	return 0;
}
