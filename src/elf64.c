
#include <stdio.h>
#include <stdlib.h>

#include "elf64.h"
#include "elf64_hdr.h"
#include "elf64_shr.h"
#include "elf64_phr.h"
#include "sym_tab.h"

ELF *elf64_read(char *e_fname) {
	ELF *bin = malloc(sizeof(ELF));
	// TODO check existence of file 
	bin->file = fopen(e_fname, "r");

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