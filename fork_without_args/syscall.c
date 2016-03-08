#define SYS_puts 0
#define SYS_fork 1

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
