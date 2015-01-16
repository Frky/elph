
#include <stdio.h>
#include <stdlib.h>

#include "elf64.h"
#include "elf64_hdr.h"
#include "elf64_shr.h"
#include "elf64_phr.h"
#include "sym_tab.h"


/*
 * Read a binary file and parse useful information, such as ELF header, section
 * headers, program headers, symbol table, etc.
 * This function opens the binary file and do not close it.
 *
 * @param   e_fname Name of the binary file to read
 *
 * @ret     ELF structure containing read information and a pointer 
 *          on the binary file, previously opened
 *
 */
ELF *elf64_read(char *e_fname) {
	ELF *bin = malloc(sizeof(ELF));
	// TODO check existence of file 
	bin->file = fopen(e_fname, "r");

	fseek(bin->file, 0L, SEEK_END);
	bin->size = ftell(bin->file);

	/* Read elf header */	
	bin->ehr = read_header(bin->file);
	/* Read section header table */
	bin->shr = read_shr_all(bin->file, bin->ehr->e_shnum, 
			bin->ehr->e_shoff, bin->ehr->e_shentsize);
	/* Read program header table */
	bin->phr = Elf64_read_phr_all(bin->file, bin->ehr->e_phnum, 
			bin->ehr->e_phoff, bin->ehr->e_phentsize);
	/* Get symbol table index */
	bin->symtab_idx = get_section_idx(bin, ".symtab");
	/* Read symbol table */
	bin->symtab = read_sym_tab(bin->file, bin->shr[bin->symtab_idx], 
							&(bin->symtab_num));
	/* Get dynamic symbol table index */
	bin->dynsym_idx = get_section_idx(bin, ".dynsym");
	/* Read dynamic symbol table */
	bin->dynsym = read_sym_tab(bin->file, bin->shr[bin->dynsym_idx], 
							&(bin->dynsym_num));
	return bin;
}
