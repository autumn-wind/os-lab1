#include "syscall.h"

char buf[] = "Aloha!!";
volatile int x = 0;

int main(char *args) {
	puts(args);
	puts("New process is running!\n");
	exit();
	return 0;
}
