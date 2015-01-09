
#ifndef __FDETECT_H__
#define __FDETECT_H__

#include "types.h"
#include "binary.h"

struct Elf64_Func_s {
	char *f_name;
	Elf64_Off f_offset;
	Elf64_Addr f_addr;
};

Elf64_Xword get_func_num(ELF *bin);

Elf64_Func **get_func(ELF *bin);

void print_func_info(Elf64_Func **funcs, Elf64_Xword f_num);
#endif
