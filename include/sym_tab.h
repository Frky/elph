
#ifndef __SYM_TAB_H__
#define __SYM_TAB_H__

#include <stdio.h>
#include "types.h"

struct Elf64_Sym_s {
	Elf64_Word st_name; 		/* Symbol name */
	unsigned char st_info; 		/* Type and Binding attributes */
	unsigned char st_other; 	/* Reserved */
	Elf64_Half st_shndx; 		/* Section table index */
	Elf64_Addr st_value; 		/* Symbol value */
	Elf64_Xword st_size; 		/* Size of object (e.g., common) */
};

Elf64_Sym **read_sym_tab(FILE *bin, Elf64_Shdr *symtab_hr, Elf64_Xword *nb_entries);

void print_symtab_info(ELF *bin);

#endif
