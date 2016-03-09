#include "syscall.h"

void read_line(char *);
int parse(char *);

char cmd[1024];

int main() {
	int filename, pid;
	while(1) {
		read_line(cmd);
		puts(cmd);
		filename = parse(cmd);
		if( (pid = fork()) == 0) {
			exec(filename, cmd);
		}
		else {
			waitpid(pid);
		}
	}
	return 0;
}

void read_line(char *buf){
	gets(buf);
}

int parse(char *buf){
	return buf[2] - '0';
}
