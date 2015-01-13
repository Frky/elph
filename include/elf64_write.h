
#ifndef __ELF64_WRITE_H__
#define __ELF64_WRITE_H__

#include <stdio.h>
#include "types.h"

void Elf64_write_half_le(FILE *bin_file, Elf64_Half half);
void Elf64_write_word_le(FILE *bin_file, Elf64_Word word);
void Elf64_write_xword_le(FILE *bin_file, Elf64_Xword xword);
void Elf64_write_off_le(FILE *bin_file, Elf64_Off off);
void Elf64_write_addr_le(FILE *bin_file, Elf64_Addr addr);

void Elf_write_byte(FILE *bin_file, unsigned char byte);

#endif
