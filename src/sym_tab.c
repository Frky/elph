
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "elf64_read.h"
#include "section.h"
#include "sym_tab.h"


Elf64_Sym **read_sym_tab(FILE *bin, Elf64_Shdr *symtab_hr, Elf64_Xword *nb_entries) {
	Elf64_Sym **symtab;
	size_t i;

	*nb_entries = symtab_hr->sh_size / sizeof(Elf64_Sym);
 	symtab = malloc(*nb_entries * sizeof(Elf64_Sym *));

	fseek(bin, symtab_hr->sh_offset, SEEK_SET);

	for (i = 0; i < *nb_entries; i++) {
		symtab[i] = malloc(sizeof(Elf64_Sym));
		symtab[i]->st_name = Elf64_read_word_le(bin);
		symtab[i]->st_info = Elf_read_byte(bin);
		symtab[i]->st_other = Elf_read_byte(bin);
		symtab[i]->st_shndx = Elf64_read_half_le(bin);
		symtab[i]->st_value = Elf64_read_addr_le(bin);
		symtab[i]->st_size = Elf64_read_xword_le(bin);
	}

	return symtab;
}

char *get_sym_name(FILE *bin, Elf64_Sym *sym, Elf64_Off strtab_offset) {
	unsigned int offset = strtab_offset + sym->st_name;
	char *buffer = malloc(255 * sizeof(char));
	char *sym_name;
	int name_size = 0;

  	fseek(bin, offset, SEEK_SET);
    	do { 
        	buffer[name_size] = fgetc(bin);
        	name_size++;
    	} while (buffer[name_size -1] != '\0' && name_size < 255);

	sym_name = malloc(name_size * sizeof(char));
	strcpy(sym_name, buffer);
	return sym_name;
}


void print_symtab_info(ELF *bin) {
	size_t i;
	Elf64_Half strtab_idx = get_section_idx (bin, ".strtab");
	Elf64_Off strtab_offset = bin->shr[strtab_idx]->sh_offset;

	for (i = 0; i < bin->symtab_num; i++) {
		printf("%s\n", get_sym_name(bin->file, bin->symtab[i], strtab_offset));
	}
}
