
#ifndef __SYM_TAB_H__
#define __SYM_TAB_H__

#include <stdio.h>
#include "types.h"

#define STB_LOCAL	0x0
#define STB_GLOBAL	0x1
#define STB_WEAK	0x2
#define STB_LOOS	0xA
#define STB_HIOS	0xC
#define STB_LOPROC	0xD
#define STB_HIPROC	0xF

#define STT_NOTYPE	0x0
#define STT_OBJECT	0x1
#define STT_FUNC	0x2
#define STT_SECTION	0x3
#define STT_FILE	0x4
#define STT_LOOS	0xA
#define STT_HIOS	0xC
#define STT_LOPROC	0xD
#define STT_HIPROC	0xF

struct Elf64_Sym_s {
	Elf64_Word st_name; 		/* Symbol name */
	unsigned char st_info; 		/* Type and Binding attributes */
	unsigned char st_other; 	/* Reserved */
	Elf64_Half st_shndx; 		/* Section table index */
	Elf64_Addr st_value; 		/* Symbol value */
	Elf64_Xword st_size; 		/* Size of object (e.g., common) */
};

Elf64_Sym **read_sym_tab(FILE *bin, Elf64_Shdr *symtab_hr, Elf64_Xword *nb_entries);

void print_sym_info(ELF *bin);

#endif
