
#ifndef __SECTION_H__
#define __SECTION_H__

#include <stdio.h>
#include "types.h"
#include "binary.h"

#define SHN_UNDEF	0x0000
#define SHN_LOPROC	0xFF00
#define SHN_HIPROC	0xFF1F
#define SHN_LOOS	0xFF20
#define SHN_HIOS	0xFF3F
#define SHN_ABS		0xFFF1
#define SHN_COMMON	0xFFF2

#define SHT_NULL	0x00
#define SHT_PROGBITS	0x01
#define SHT_SYMTAB	0x02
#define SHT_STRTAB	0x03
#define SHT_RELA	0x04
#define SHT_HASH	0x05
#define SHT_DYNAMIC	0x06
#define SHT_NOTE	0x07
#define SHT_NOBITS	0x08
#define SHT_REL 	0x09
#define SHT_SHLIB	0x0A
#define SHT_DYNSYM	0x0B
#define SHT_LOOS	0x60000000
#define SHT_HIOS	0x6FFFFFFF
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7FFFFFFF

#define SHF_WRITE	0x01
#define SHF_ALLOC	0x02
#define SHF_EXECINSTR	0x04
#define SHF_MASKOS	0x0F000000
#define SHF_MASKPROC	0xF0000000

struct Elf64_Shdr_s {
	Elf64_Word sh_name; 		/* Section name */
	Elf64_Word sh_type; 		/* Section type */
	Elf64_Xword sh_flags; 		/* Section attributes */
	Elf64_Addr sh_addr; 		/* Virtual address in memory */
	Elf64_Off sh_offset; 		/* Offset in file */
	Elf64_Xword sh_size; 		/* Size of section */
	Elf64_Word sh_link; 		/* Link to other section */
	Elf64_Word sh_info; 		/* Miscellaneous information */
	Elf64_Xword sh_addralign; 	/* Address alignment boundary */
	Elf64_Xword sh_entsize; 	/* Size of entries, if section has table */
};

Elf64_Shdr *read_section_header(FILE *bin, Elf64_Addr addr);

Elf64_Shdr **read_shr_all(FILE *bin, Elf64_Half shr_num, Elf64_Off shr_off, Elf64_Half shr_entrysize);

Elf64_Half get_section_idx(ELF *bin, char *section_name);

void print_shr_info_all(ELF *bin);

#endif
