
#include <stdlib.h>
#include <stdbool.h>
#include "fdetect.h"
#include "types.h"
#include "binary.h"
#include "sym_tab.h"


/*
 * Check if a given entry in symbol table corresponds to a function
 * 
 * @param	sym	Pointer to the symbol structure to check
 *
 * @ret		true iif the symbol corresponds to a function
 *
 */
bool is_func(Elf64_Sym *sym) {
	return (sym->st_info & 0x0F) == STT_FUNC;
}


/*
 * Compute the number of function entries in the symbol table
 * 
 * @param	bin	Binary structure
 *
 * @req		The symbol table must have been previously read
 *
 * @ret		true iif the symbol corresponds to a function
 *
 */
Elf64_Xword get_func_num(ELF *bin) {
	size_t i;
	Elf64_Xword func_num = 0;
	for (i = 0; i < bin->symtab_num; i++) {
		if (is_func(bin->symtab[i]))
			func_num++;
	}
	return func_num;
}


Elf64_Off get_func_offset(ELF *bin, Elf64_Addr f_addr, Elf64_Sym *sym) {
	Elf64_Shdr *shr = bin->shr[sym->st_shndx];
	return shr->sh_offset + (f_addr - shr->sh_addr);
}

/*
 * Get the functions information (such as address and name) 
 * 
 * @param	bin	Binary structure
 *
 * @req		The symbol table must have been previously read
 *
 * @ret		An array of function structures
 *
 */
Elf64_Func **get_func(ELF *bin) {
	size_t i, j = 0;
	/* Array of function structures */
	Elf64_Func **funcs;
	/* index of the string table header */
	Elf64_Half strtab_idx;
	/* Offset of the string table */
	Elf64_Off strtab_offset;

	/* Get the string table information needed to get the names */
 	strtab_idx = get_section_idx (bin, ".strtab");
	strtab_offset = bin->shr[strtab_idx]->sh_offset;

	/* Get the number of functions */
	bin->ftab_num = get_func_num(bin);
	/* Allocate the array of function structures */
 	funcs = malloc(bin->ftab_num * sizeof(Elf64_Func *));
	/* Iterate on the symbol table entries */
	for (i = 0; i < bin->symtab_num; i++) {
		/* For each entry, check if it is a function entry */
		if (is_func(bin->symtab[i])) {
			/* Alocate a new function structure */
			funcs[j] = malloc(sizeof(Elf64_Func));
			/* Get the address of the function */
			funcs[j]->f_addr = (Elf64_Off) bin->symtab[i]->st_value;
			/* Get the offset of the function in file */
			funcs[j]->f_offset = get_func_offset(bin, funcs[j]->f_addr, 
								bin->symtab[i]);
			/* Get the name of the function */
			funcs[j]->f_name = get_sym_name(bin->file, bin->symtab[i], 
						strtab_offset);
			j++;
		}
	}

	return funcs;
}


/*
 * Print information about the functions embedded in the binary
 * The print is done in stdout
 * 
 * @param	funcs	Array of funtion structures
 * @param	f_num	Number of entries in funcs
 *
 * @req		The functions information must have been previously read
 *
 */
void print_func_info(Elf64_Func **funcs, Elf64_Xword f_num) {
	size_t i;
	printf("%lu functions found in this binary:\n", f_num);
	for (i = 0; i < f_num; i++) {
		printf("%s: %08x (at offset 0x%06x)\n", funcs[i]->f_name, (Elf64_Word) funcs[i]->f_addr, (Elf64_Word) funcs[i]->f_offset);
	}
	return;
}
