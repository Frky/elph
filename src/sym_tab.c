
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "elf64_read.h"
#include "section.h"
#include "sym_tab.h"


/*
 * Read the symbol table. For each symbol, allocate and fill a symbol structure, 
 * and return an array containing all symbol structures parsed.
 * 
 * @param	bin		File where to read the info about section header
 * @param	symtab_hr	Pointer to the header of the symbol table
 * @param	nb_entries	Address of an int to be filled with the 
 *				number of symbols in the table
 *
 * @req		The file pointed by bin must have been previously opened
 *
 * @ret		Array of symbol structures filled with read info
 *
 */
Elf64_Sym **read_sym_tab(FILE *bin, Elf64_Shdr *symtab_hr, Elf64_Xword *nb_entries) {
	/* Table of symbols */
	Elf64_Sym **symtab;
	size_t i;

	/* Computing the number of entries in the table */
	*nb_entries = symtab_hr->sh_size / sizeof(Elf64_Sym);
	/* Allocate the array of symbol entries */
 	symtab = malloc(*nb_entries * sizeof(Elf64_Sym *));

	/* Seek the file at the beginning of the symbol table */
	fseek(bin, symtab_hr->sh_offset, SEEK_SET);

	/* Read the symbols, one by one */
	for (i = 0; i < *nb_entries; i++) {
		/* Allocate the symbol structure */
		symtab[i] = malloc(sizeof(Elf64_Sym));
		/* Read the name position in string table of the symbol */
		symtab[i]->st_name = Elf64_read_word_le(bin);
		/* Read info */
		symtab[i]->st_info = Elf_read_byte(bin);
		/* Read other */
		symtab[i]->st_other = Elf_read_byte(bin);
		/* Read index */
		symtab[i]->st_shndx = Elf64_read_half_le(bin);
		/* Read value */
		symtab[i]->st_value = Elf64_read_addr_le(bin);
		/* Read size */
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


void print_sym_type(unsigned char st_type) {
	size_t i, l;
	switch (st_type) {
	case STT_NOTYPE:
		printf("NOTYPE");
		l = 6;
		break;
	case STT_OBJECT:
		printf("OBJECT");
		l = 6;
		break;
	case STT_FUNC:
		printf("FUNC");
		l = 4;
		break;
	case STT_SECTION:
		printf("SECTION");
		l = 7;
		break;
	case STT_FILE:
		printf("FILE");
		l = 4;
		break;
	case STT_LOOS:
		printf("LOOS");
		l = 4;
		break;
	case STT_HIOS:
		printf("HIOS");
		l = 4;
		break;
	case STT_LOPROC:
		printf("LOPROC");
		l = 6;
		break;
	case STT_HIPROC:
		printf("HIPROC");
		l = 6;
		break;
	}
	for (i = l; i < 8; i++) 
		printf(" ");
}


void print_sym_bind(unsigned char st_bind) {
	size_t i, l;
	switch (st_bind) {
	case STB_LOCAL:
		printf("LOCAL");
		l = 5;
		break;
	case STB_GLOBAL:
		printf("GLOBAL");
		l = 6;
		break;
	case STB_WEAK:
		printf("WEAK");
		l = 4;
		break;
	case STB_LOOS:
		printf("LOOS");
		l = 4;
		break;
	case STB_HIOS:
		printf("HIOS");
		l = 4;
		break;
	case STB_LOPROC:
		printf("LOPROC");
		l = 6;
		break;
	case STB_HIPROC:
		printf("HIPROC");
		l = 6;
		break;
	}
	for (i = l; i < 7; i++) 
		printf(" ");
}


void print_sym_idx(Elf64_Half s_idx) {
	switch (s_idx) {
	case SHN_UNDEF:
		printf("UND ");
		break;
	case SHN_ABS:
		printf("ABS ");
		break;
	case SHN_COMMON:
		printf("COM ");
		break;
	default:
		printf("%3u ", s_idx);
	}

	return;
}

void print_symtab_entry(Elf64_Sym *sym, Elf64_Half s_ndx, char *s_name) {
	printf("    ");
	/* Symbol index */
	printf("%2u: ", s_ndx);
	/* Symbol value */
	printf("%016lx  ", sym->st_value);
	/* Symbol size */
	printf("%4i ", (Elf64_Half) sym->st_size);
	/* Symbol type */
	print_sym_type(sym->st_info & 0x0F);
	/* Symbol binding */
	print_sym_bind((sym->st_info & 0xF0) >> 4);
	/* Print index */
	print_sym_idx(sym->st_shndx);
	/* Symbol name */
	printf("%s", s_name);
	printf("\n");

	return;
}

void print_symtab_info(ELF *bin) {
	size_t i;
	char *s_name;
	Elf64_Half strtab_idx = get_section_idx (bin, ".strtab");
	Elf64_Off strtab_offset = bin->shr[strtab_idx]->sh_offset;

	printf("   Num:    Value          Size Type    Bind   Ndx Name\n");

	for (i = 0; i < bin->symtab_num; i++) {
		s_name = get_sym_name(bin->file, bin->symtab[i], strtab_offset);
		print_symtab_entry(bin->symtab[i], i, s_name);
	}

	return;
}
