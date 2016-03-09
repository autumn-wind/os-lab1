#include "syscall.h"

char buf[] = "Aloha!!";
volatile int x = 0;

int main() {
	while(1) {
		if(x % 10000000 == 0) {
			int n = fork();
			if(!n){
				exec(1, buf);
			}
		}
		x ++;
	}
	return 0;
}
