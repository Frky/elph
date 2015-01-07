
#ifndef __ELF64_READ_H__
#define __ELF64_READ_H__

#include <stdio.h>
#include "types.h"

#define DEBUG_READING		0

Elf64_Half Elf64_read_half_le(FILE *bin_file);
Elf64_Word Elf64_read_word_le(FILE *bin_file);
Elf64_Xword Elf64_read_xword_le(FILE *bin_file);
Elf64_Off Elf64_read_off_le(FILE *bin_file);
Elf64_Addr Elf64_read_addr_le(FILE *bin_file);

unsigned char Elf_read_byte(FILE *bin_file);

#endif
