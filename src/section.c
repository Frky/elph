
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "elf64_read.h"
#include "section.h"
#include "types.h"

#define COLUMN_LENGTH	16

Elf64_Shdr *read_section_header(FILE *bin, Elf64_Off offset) {
	
	/* Seek to the beginning of the section header */
	fseek(bin, offset, SEEK_SET);

	Elf64_Shdr *shr = malloc(sizeof(Elf64_Shdr));

	/* Read section name index in setion name table */
	shr->sh_name = Elf64_read_word_le(bin);

	/* Read section type */
	shr->sh_type = Elf64_read_word_le(bin);

	/* Read section flags */
	shr->sh_flags = Elf64_read_xword_le(bin);

	/* Read section addr in memory */
	shr->sh_addr = Elf64_read_addr_le(bin);

	/* Read section offset in file */
	shr->sh_offset = Elf64_read_off_le(bin);

	/* Read section size */
	shr->sh_size = Elf64_read_xword_le(bin);

	/* Read link to next section */
	shr->sh_link = Elf64_read_word_le(bin);

	/* Read section information */
	shr->sh_info = Elf64_read_word_le(bin);

	/* Read address alignment boundary */
	shr->sh_addralign = Elf64_read_xword_le(bin);

	/* Read size of entries in this section */
	shr->sh_entsize = Elf64_read_xword_le(bin);

	return shr;
}


Elf64_Shdr **read_shr_all(FILE *bin, Elf64_Half shr_num, Elf64_Off shr_off, Elf64_Half shr_entrysize) {
	Elf64_Shdr **shr_tab = malloc(sizeof(Elf64_Shdr *) * shr_num);
	Elf64_Off offset = 0;
	size_t i;
	for (i = 0; i < shr_num; i++) {
		shr_tab[i] = read_section_header(bin, shr_off + offset);
		offset += shr_entrysize;
	}
	return shr_tab;
}


char *get_section_name(FILE *bin, Elf64_Shdr *shr, Elf64_Off strtab_offset) {
	unsigned int offset = strtab_offset + shr->sh_name;
	char *buffer = malloc(COLUMN_LENGTH * sizeof(char));
	char *section_name;
	int name_size = 0;

  	fseek(bin, offset, SEEK_SET);
    	do { 
        	buffer[name_size] = fgetc(bin);
        	name_size++;
    	} while (buffer[name_size -1] != '\0' && name_size < COLUMN_LENGTH);

	section_name = malloc(name_size * sizeof(char));
	strcpy(section_name, buffer);
	return section_name;
}


Elf64_Half get_section_idx(ELF *bin, char *section_name) {
	Elf64_Half idx = 0;
	Elf64_Off stroff = bin->shr[bin->ehr->e_shstrndx]->sh_offset;
	while (strcmp(section_name, get_section_name(bin->file, bin->shr[idx], stroff)) != 0)
		idx++;
	return idx;
}


void print_section_name(char *section_name) {
	int i, l = strlen(section_name) - 1;
    	printf("%s", section_name);
	for (i = l; i < COLUMN_LENGTH; i++) 
		printf(" ");
	printf("  ");
}


void print_section_type(Elf64_Word type) {
	int i, l;
	switch (type) {
	case SHT_NULL:
		printf("NULL");
		l = 4;
		break;
	case SHT_PROGBITS:
		printf("PROGBITS");
		l = 8;
		break;
	case SHT_SYMTAB:
		printf("SYMTAB");
		l = 6;
		break;
	case SHT_STRTAB:
		printf("STRTAB");
		l = 6;
		break;
	case SHT_RELA:
		printf("RELA");
		l = 4;
		break;
	case SHT_HASH:
		printf("HASH");
		l = 4;
		break;
	case SHT_DYNAMIC:
		printf("DYNAMIC");
		l = 7;
		break;
	case SHT_NOTE:
		printf("NOTE");
		l = 4;
		break;
	case SHT_NOBITS:
		printf("NOBITS");
		l = 6;
		break;
	case SHT_REL:
		printf("REL");
		l = 3;
		break;
	case SHT_SHLIB:
		printf("SHLIB");
		l = 5;
		break;
	case SHT_DYNSYM:
		printf("DYNSYM");
		l = 6;
		break;
	case SHT_LOOS:
		printf("LOOS");
		l = 4;
		break;
	case SHT_HIOS:
		printf("HIOS");
		l = 4;
		break;
	case SHT_LOPROC:
		printf("LIPROC");
		l = 6;
		break;
	case SHT_HIPROC:
		printf("HIPROC");
		l = 6;
		break;
    default:
        l = 0;
	}
	for (i = l + 1; i < COLUMN_LENGTH; i++) 
		printf(" ");
	printf("  ");
}


void print_section_flags(Elf64_Xword flags) {
	if ((flags & 0x1) != 0)
		printf("W");
	else
		printf(" ");
	if ((flags & 0x2) != 0)
		printf("A");
	else
		printf(" ");
	if ((flags & 0x4) != 0)
		printf("X");
	else
		printf(" ");
	printf("   ");
}


void print_shr_info(Elf64_Shdr *shr, ELF *bin, int shr_index) {
	/* Section index */
	printf("  [%2i] ", shr_index);

	/* Section name */
	print_section_name(get_section_name(bin->file, shr, bin->shr[bin->ehr->e_shstrndx]->sh_offset));

	print_section_type(shr->sh_type);

	/* Section address */
	printf("%016lx  ", shr->sh_addr);

	/* Section offset */
	printf("%08x  ", (unsigned int) shr->sh_offset);

	printf("\n");

	/* Section size */
	printf("       %016lx  ", shr->sh_size);

	/* Entry size */
	printf("%016lx  ", shr->sh_entsize);

	/* FLAGS */
	print_section_flags(shr->sh_flags);

	/* Link */
	printf("%4u  ", shr->sh_link);

    	/* Info */
	printf("%4u  ", shr->sh_info);

 	/* Align */
	printf("   %lu  ", shr->sh_addralign);

	printf("\n");
}


void print_shr_info_all(ELF *bin) {

	size_t i;

	printf("There are %d section headers, starting at offset 0x%lx:\n", 
		bin->ehr->e_shnum, bin->ehr->e_shoff);
	printf("\n");
	printf("Section Headers:\n");
	printf("  [Nr] Name              Type             Address           Offset\n");
	printf("       Size              EntSize          Flags  Link  Info  Align\n");

	for (i = 0; i < bin->ehr->e_shnum; i++) {
		print_shr_info(bin->shr[i], bin, i);
	}
}

