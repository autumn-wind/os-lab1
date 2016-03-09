#define SYS_puts 0
#define SYS_fork 1
#define SYS_exec 2
#define SYS_exit 3
#define SYS_gets 4
#define SYS_waitpid 5

int __attribute__((__noinline__))
syscall(int id, ...) {
	int ret;
	int *args = &id;
	asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));
	return ret;
}

int puts(char *buf){
	return syscall(SYS_puts, buf);
}

int fork(){
	return syscall(SYS_fork);
}

int exec(int file, char *args){
	return syscall(SYS_exec, file, args);
}

int exit(){
	return syscall(SYS_exit);
}

int gets(char *buf){
	return syscall(SYS_gets, buf);
}

int waitpid(int pid){
	return syscall(SYS_waitpid, pid);
}
