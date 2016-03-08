#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#define NEW_PAGE -3
#define SHARE_KERNEL_PAGE -4
#define CLEAN_ADDR -5
#define FORK_PROCESS -6
#define COPY_FATHER_PAGE -7
#define EXEC_PROCESS -8
#define GET_ARGS_PHY_ADDR -9
#define EXIT_PROCESS -10
#define GET_PAGE_DIR -11
#define GET_STACK_PAGE -12

size_t do_read(int file_name, uint8_t *buf, off_t offset, size_t len);

pid_t FM, PM, MM;

/* Structure of a ELF binary header */
struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;
	unsigned int   phoff;
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

/* Structure of program header inside ELF binary */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};

#endif
