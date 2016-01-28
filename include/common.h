#ifndef __COMMON_H__
#define __COMMON_H__

#include "types.h"
#include "const.h"

void printk(const char *ctl, ...);
extern char hexBoard[];
extern void serial_printc(char);

#include "assert.h"


#endif
