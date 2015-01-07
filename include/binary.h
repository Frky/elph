
#ifndef __BINARY_H__
#define __BINARY_H__

#include <stdio.h>
#include "types.h"
#include "header.h"

#define DEBUG_READING		0

/*
typedef struct {
	FILE *file;
	size_t 	size;
	Elf64_Ehdr *ehr;
} ELF;
*/

unsigned char Elf_read_byte(FILE *bin_file);

Elf64_Half Elf64_read_half_le(FILE *bin_file);
Elf64_Word Elf64_read_word_le(FILE *bin_file);
Elf64_Xword Elf64_read_addr_le(FILE *bin_file);

#endif
