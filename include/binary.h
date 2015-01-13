
#ifndef __BINARY_H__
#define __BINARY_H__

#include <stdio.h>
#include "elf64_hdr.h"
#include "elf64_shr.h"
#include "fdetect.h"

struct ELF_s {
	FILE *file;
	size_t 	size;
	Elf64_Ehdr *ehr;
	Elf64_Shdr **shr;
	Elf64_Phdr **phr;
	Elf64_Sym **symtab;
	Elf64_Xword symtab_idx;
	Elf64_Xword symtab_num;
	Elf64_Sym **dynsym;
	Elf64_Xword dynsym_idx;
	Elf64_Xword dynsym_num;
	Elf64_Func **ftab;
	Elf64_Xword ftab_num;
};

#endif
