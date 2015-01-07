
#ifndef __BINARY_H__
#define __BINARY_H__

#include <stdio.h>
#include "header.h"
#include "section.h"

struct ELF_s {
	FILE *file;
	size_t 	size;
	Elf64_Ehdr *ehr;
	Elf64_Shdr **shr;
};

#endif
