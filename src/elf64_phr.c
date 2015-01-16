
#include <stdlib.h>

#include "elf64_phr.h"
#include "types.h"
#include "elf64.h"
#include "elf64_read.h"
#include "elf64_write.h"

/* Number of columns for displaying a section type */
#define COLUMN_TYPE_LENGTH	16 


/*
 * Read a program header in binary file. Allocates a program header
 * structure and fill it with read info. 
 * 
 * @param	bin	File where to read the info about program header
 * @param	offset	Offset (in bytes into file) where the program header
 *			to be read starts
 *
 * @req		The file pointed by bin must have been previously opened
 *
 * @ret		Pointer to an allocated program header structure 
 *		filled with read info	
 *
 */
Elf64_Phdr *Elf64_read_phr(FILE *bin, Elf64_Addr offset) {
	/* Allocate a program header */
	Elf64_Phdr *phr = malloc(sizeof(Elf64_Phdr));
	/* Seek to the beginning of the program header */
	fseek(bin, offset, SEEK_SET);
	/* Read type */
	phr->p_type = Elf64_read_word_le(bin);
	/* Read flags */
	phr->p_flags = Elf64_read_word_le(bin);
	/* Read offset (bytes into file) */
	phr->p_offset = Elf64_read_off_le(bin);
	/* Read virtual address */
	phr->p_vaddr = Elf64_read_addr_le(bin);
	/* Read physical address */
	phr->p_paddr = Elf64_read_addr_le(bin);
	/* Read size of the file image of the segment */
	phr->p_filesz = Elf64_read_xword_le(bin);
	/* Read size of the memory image of the segment */
	phr->p_memsz = Elf64_read_xword_le(bin);
	/* Read memory alignment */
	phr->p_align = Elf64_read_xword_le(bin);
	return phr;
} 


/*
 * Read all program headers in binary file. Allocates an array of program headers
 * and fill it with read program headers. 
 * 
 * @param	bin		File where to read the program headers
 * @param	phr_num		Number of program headers in the file
 * @param	phr_off		Offset of the program header table (in bytes 
				into file)
 * @param	phr_entrysize	Number of entries in program header table (in
				bytes into file)
 *
 * @req		The file pointed by bin must have been previously opened
 *
 * @ret		Array of program headers allocated and filled with read info
 *
 */
Elf64_Phdr **Elf64_read_phr_all(FILE *bin, 
				Elf64_Half phr_num, 
				Elf64_Off phr_off, 
				Elf64_Half phr_entrysize) {
	/* Allocate an array of program headers to be filled */
	Elf64_Phdr **phr_tab = malloc(sizeof(Elf64_Phdr *) * phr_num);
	/* Offset of the program header to be read (bytes into file) */
	Elf64_Off offset = 0;
	size_t i;
	/* Read the program headers one by one */
	for (i = 0; i < phr_num; i++) {
		phr_tab[i] = Elf64_read_phr(bin, phr_off + offset);
		offset += phr_entrysize;
	}
	return phr_tab;
}					


/*
 * Write a program header into binary file (erasing the previous one if any). 
 * 
 * @param	bin		File where to write the program header
 * @param	phr		Program header to write
 * @param	offset		Offset in the binary file where to write the program header 
 *				(in bytes into file)
 *
 * @req		The file pointed by bin must have been previously opened
 *
 */
void Elf64_write_phr(FILE *bin, Elf64_Phdr *phr, Elf64_Addr offset) {
	/* Seek to the beginning of the program header */
	fseek(bin, offset, SEEK_SET);
	/* Write type */
	Elf64_write_word_le(bin, phr->p_type);
	/* Write flags */
	Elf64_write_word_le(bin, phr->p_flags);
	/* Write offset (bytes into file) */
	Elf64_write_off_le(bin, phr->p_offset);
	/* Write virtual address */
	Elf64_write_addr_le(bin, phr->p_vaddr);
	/* Write physical address */
	Elf64_write_addr_le(bin, phr->p_paddr);
	/* Write size of the file image of the segment */
	Elf64_write_xword_le(bin, phr->p_filesz);
	/* Write size of the memory image of the segment */
	Elf64_write_xword_le(bin, phr->p_memsz);
	/* Write memory alignment */
	Elf64_write_xword_le(bin, phr->p_align);
	return;
} 


/*
 * Write all program headers in binary file. Overwrite previous ones if any. 
 * 
 * @param	bin		File where to write the program headers
 * @param	phr_tab		Array of program headers to be written
 * @param	phr_num		Number of program headers in phr_tab
 * @param	phr_off		Offset of the program header table (in bytes 
				into file)
 * @param	phr_entrysize	Number of entries in program header table 
 *
 * @req		The file pointed by bin must have been previously opened
 *
 */
void Elf64_write_phr_all(FILE *bin, 
					Elf64_Phdr **phr_tab,
					Elf64_Half phr_num, 
					Elf64_Off phr_off, 
					Elf64_Half phr_entrysize) {
	
	/* Offset of the program header to be read (bytes into file) */
	Elf64_Off offset = 0;
	size_t i;
	/* Read the program headers one by one */
	for (i = 0; i < phr_num; i++) {
		Elf64_write_phr(bin, phr_tab[i], phr_off + offset);
		offset += phr_entrysize;
	}
	return;
}


/*
 * Find the note segment in an array of program headers.
 * The method used here is exclusively based on PT_TYPE.
 * If no note segment is found, a null pointer is returned.
 * 
 * @param	bin		Binary structure (program headers are in bin->phr)
 *
 * @req		The elf header and program header table must have been read previously 
 *		(meaning that bin->ehr and bin->phr must have been allocated and filled)
 *
 */
Elf64_Phdr *Elf64_get_pnote(ELF *bin) {
	size_t i;
	Elf64_Phdr *phr;
	/* Iteration on program headers */
	for (i = 0; i < bin->ehr->e_phnum; i++) {
		phr = bin->phr[i];
		/* Check type of program header */
		if (phr->p_type == PT_NOTE) {
			/* If PT_NOTE, return pointer */
			return phr;
		}
	}
	return NULL;
}


/*
 * Find the code segment in an array of program headers.
 * The method used here is based on flags: it returns the first
 * program header to be readable and executable.
 * If no code segment is found, a null pointer is returned.
 * 
 * @param	bin		Binary structure (program headers are in bin->phr)
 *
 * @req		The elf header and program header table must have been read previously 
 *		(meaning that bin->ehr and bin->phr must have been allocated and filled)
 *
 */
Elf64_Phdr *Elf64_get_pcode(ELF *bin) {
	size_t i;
	Elf64_Phdr *phr;
	/* Iteration on program headers */
	for (i = 0; i < bin->ehr->e_phnum; i++) {
		phr = bin->phr[i];
		/* Check readable and executable flags */
		if ((phr->p_flags & 0x04) != 0 && (phr->p_flags & 0x01) != 0) {
			/* If both flags are set, it is likely to be code segment */
			return phr;
		}
	}
	return NULL;
}


/*
 * Print a program header type on COLUMN_TPE_LENGTH
 * columns, filled with blank spaces. 
 * The print is done on stdout.
 * 
 * @param	p_type	 Program type to be printed
 *
 */
void Elf64_print_ph_type(Elf64_Word p_type) {
	int i, l;
	switch (p_type) {
	case PT_NULL:
		printf("NULL");
		l = 4;
		break;
	case PT_LOAD:
		printf("LOAD");
		l = 4;
		break;
	case PT_DYNAMIC:
		printf("DYNAMIC");
		l = 7;
		break;
	case PT_INTERP:
		printf("INTERP");
		l = 6;
		break;
	case PT_NOTE:
		printf("NOTE");
		l = 4;
		break;
	case PT_SHLIB:
		printf("SHLIB");
		l = 5;
		break;
	case PT_PHDR:
		printf("PHDR");
		l = 4;
		break;
	case PT_LOOS:
		printf("LOOS");
		l = 4;
		break;
	case PT_HIOS:
		printf("HIOS");
		l = 4;
		break;
	case PT_LOPROC:
		printf("LOPROC");
		l = 6;
		break;
	case PT_HIPROC:
		printf("HIPROC");
		l = 6;
		break;
    	default:
        	l = 0;
	}
	for (i = l; i < COLUMN_TYPE_LENGTH; i++) 
		printf(" ");
}


/*
 * Print a program header flags. Flags consist of
 * three letters/spaces (RWE stading for Read, Write,
 * Execute). 
 * The print is done on stdout.
 * 
 * @param	p_flags	 Program flags to be printed
 *
 */
void Elf64_print_ph_flags(Elf64_Xword p_flags) {
	/* Checking Writable flag */
	if ((p_flags & 0x1) != 0)
		printf("R");
	else
		printf(" ");
	/* Checking Allocatable flag */
	if ((p_flags & 0x2) != 0)
		printf("W");
	else
		printf(" ");
	/* Checking eXecutable flag */
	if ((p_flags & 0x4) != 0)
		printf("E");
	else
		printf(" ");
	printf(" ");
}


/*
 * Print a program header info. 
 * The print is done on stdout.
 * 
 * @param	phr	Pointer to the program header to be printed
 *
 */
void Elf64_print_phr(Elf64_Phdr *phr) {
	printf("  ");
	/* Print type */
	Elf64_print_ph_type(phr->p_type);
	/* Print offset */
	printf("%06x ", (Elf64_Word) phr->p_offset);
	/* Print virtual address */
	printf("%08x ", (Elf64_Word) phr->p_vaddr);
	/* Print physical address */
	printf("%08x ", (Elf64_Word) phr->p_paddr);
	/* Print file size */
	printf("%06x ", (Elf64_Word) phr->p_filesz);
	/* Print memory size */
	printf("%06x ", (Elf64_Word) phr->p_memsz);
	/* Print flags */
	Elf64_print_ph_flags(phr->p_flags);
	/* Print memory align */
	printf("%2u", (unsigned char) phr->p_align);
	printf("\n");
	return;
}


/*
 * Print all program headers info. 
 * The print is done on stdout.
 * 
 * @param	bin	Binary structure
 *
 */
void Elf64_print_phr_info(ELF *bin) {
	size_t i;
	printf("There are %d program headers, starting at offset 0x%lx:\n", 
		bin->ehr->e_phnum, bin->ehr->e_phoff);
	printf("\n");
	printf("Program Headers:\n");
	printf("  Type            Off    VirtAddr PhysAddr FSize  MSize  Flg Al\n");
	for (i = 0; i < bin->ehr->e_phnum; i++) {
		Elf64_print_phr(bin->phr[i]);
	}
	return;
}
