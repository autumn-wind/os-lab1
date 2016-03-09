int __attribute__((__noinline__))
syscall(int id, ...);

int puts(char *buf);
int fork();
int exec(int file, char *args);
int exit();
int gets(char *buf);
int waitpid(int pid);
