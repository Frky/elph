
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "elf64_shr.h"
#include "types.h"
#include "elf64_read.h"
#include "elf64_write.h"

/* Number of columns for displaying a section name and type.
   Used to padd strings with " " for displaying alignment. */
#define COLUMN_NAME_LENGTH	18
#define COLUMN_TYPE_LENGTH	16


/*
 * Read a section header in binary file. Allocates a section header
 * structure and fill it with read info. 
 * 
 * @param	bin	File where to read the info about section header
 * @param	offset	Offset (in bytes into file) where the section header
 *			to be read starts
 *
 * @req		The file pointed by bin must have been previously opened
 *
 * @ret		Pointer to an allocated section header structure 
 *		filled with read info	
 *
 */
Elf64_Shdr *read_shr(FILE *bin, Elf64_Off offset) {
	/* Seek to the beginning of the section header */
	fseek(bin, offset, SEEK_SET);
	/* Allocate a section header */
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


/*
 * Read all section headers in binary file. Allocates an array of section headers
 * and fill it with read section headers. 
 * 
 * @param	bin		File where to read the section headers table
 * @param	shr_num		Number of section headers in the file
 * @param	shr_off		Offset of the section header table (in bytes 
				into file)
 * @param	shr_entrysize	Number of entries in section header table (in
				bytes into file)
 *
 * @req		The file pointed by bin must have been previously opened
 *
 * @ret		Array of section headers allocated and filled with read info
 *
 */
Elf64_Shdr **read_shr_all(FILE *bin, Elf64_Half shr_num, Elf64_Off shr_off, Elf64_Half shr_entrysize) {
	/* Allocate an array of section headers to be filled */
	Elf64_Shdr **shr_tab = malloc(sizeof(Elf64_Shdr *) * shr_num);
	/* Offset of a given section header (bytes into file) */
	Elf64_Off offset = 0;
	size_t i;
	/* Read the section headers one by one */
	for (i = 0; i < shr_num; i++) {
		shr_tab[i] = read_shr(bin, shr_off + offset);
		/* The offset of the next section header is current_offset 
		   plus the size of a section header (that was just read */
		offset += shr_entrysize;
	}
	return shr_tab;
}


void Elf64_write_shr(FILE *bin, Elf64_Shdr *shr, Elf64_Off offset) {
	/* Seek to the beginning of the section header */
	fseek(bin, offset, SEEK_SET);
	/* Write section name index in section name table */
	Elf64_write_word_le(bin, shr->sh_name);
	/* Write section type */
	Elf64_write_word_le(bin, shr->sh_type);
	/* Write section flags */
	Elf64_write_xword_le(bin, shr->sh_flags);
	/* Write section addr in memory */
	Elf64_write_addr_le(bin, shr->sh_addr);
	/* Write section offset in file */
	Elf64_write_off_le(bin, shr->sh_offset);
	/* Write section size */
 	Elf64_write_xword_le(bin, shr->sh_size);
	/* Write link to next section */
	Elf64_write_word_le(bin, shr->sh_link);
	/* Write section information */
	Elf64_write_word_le(bin, shr->sh_info);
	/* Write address alignment boundary */
	Elf64_write_xword_le(bin, shr->sh_addralign);
	/* Write size of entries in this section */
	Elf64_write_xword_le(bin, shr->sh_entsize);
	return;
}


void Elf64_write_shr_all(FILE *bin, Elf64_Shdr **shr_tab, Elf64_Half shr_num, 
				Elf64_Off shr_off, Elf64_Half shr_entrysize) {
	size_t i;
	/* Offset of a given section header (bytes into file) */
	Elf64_Off offset = 0;
	/* Write the section headers one by one */
	for (i = 0; i < shr_num; i++) {
		Elf64_write_shr(bin, shr_tab[i], shr_off + offset);
		/* The offset of the next section header is current_offset 
		   plus the size of a section header (that was just read */
		offset += shr_entrysize;
	}
	return;
}


/*
 * Find the name of a section in the string table. If the name if of more 
 * than COLUMN_NAME_LENGTH - 1 chars, it is truncated.
 * 
 * @param	bin		File where to read the info about section header
 * @param	shr		Section header structure whose name is requested
 * @param	strtab_offser	Offset of the string table where to read the 
				secton name (in bytes into file)
 *
 * @req		The file pointed by bin must have been previously opened
 *
 * @ret		A string containing the section name, terminated by \0
 *
 */
char *get_section_name(FILE *bin, Elf64_Shdr *shr, Elf64_Off strtab_offset) {
	/* compute the offset of the string to read :
 	shr->sh_name gives the offset in str table */
	unsigned int offset = strtab_offset + shr->sh_name;
	/* Allocate a buffer to read the name until next \0 */
	char *buffer = malloc(COLUMN_NAME_LENGTH * sizeof(char));
	/* String that will finally contain the section name */
	char *section_name;
	/* Length of the section name */
	int name_size = 0;

	/* Seek the file to the first byte of the name */
  	fseek(bin, offset, SEEK_SET);
	/* Read chars one by one to buffer, stopping when 
	we read \0 or we reached the maximum length */
    	do { 
        	buffer[name_size] = fgetc(bin);
        	name_size++;
    	} while (buffer[name_size -1] != '\0' && name_size < COLUMN_NAME_LENGTH - 2);
	
	/* Allocation of the string (of size nam_size + 1 to be able
	to add \0 if we stopped because of the MAX length */
	section_name = malloc((name_size + 1) * sizeof(char));
	/* Adding \0 at the end of the buffer (in case we stopped because of
	max length) */
	buffer[name_size] = '\0';
	/* Copy the buffer into good length buffer */
	strcpy(section_name, buffer);
	/* Free buffer */
	free(buffer);
	return section_name;
}


/*
 * Find section header index from the name
 * 
 * @param	bin	Binary structure
 * @param	s_name	Name of the section to be found
 *
 * @req		The file pointed by bin->file must have been previously opened
 *
 * @ret		The index of the section if found, 0xFFFF otherwise
 *
 */
Elf64_Half get_section_idx(ELF *bin, char *s_name) {
	Elf64_Half idx = 0;
	Elf64_Off stroff = bin->shr[bin->ehr->e_shstrndx]->sh_offset;
	while (idx < bin->ehr->e_shnum) {
		if (strcmp(s_name, get_section_name(bin->file, 
						bin->shr[idx], 
						stroff)) == 0)
			return idx;
		idx++;
	}
	return 0xFFFF;
}


/*
 * Print section name on COLUMN_NAME_LENGTH columns, filled with white spaces
 * The print is done on stdout.
 * 
 * @param	s_name	Name of the section to be printed
 *
 */
void print_section_name(char *s_name) {
	int i, l = strlen(s_name);
    	printf("%s", s_name);
	for (i = l; i < COLUMN_NAME_LENGTH; i++) 
		printf(" ");
}


/*
 * Print section type on COLUMN_TYPE_LENGTH columns, filled with white spaces
 * The print is done on stdout.
 * 
 * @param	s_type	Type of the section to be printed
 *
 */
void print_section_type(Elf64_Word s_type) {
	int i, l;
	switch (s_type) {
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
	for (i = l; i < COLUMN_TYPE_LENGTH; i++) 
		printf(" ");
}


/*
 * Print section flags on three columns plus a blank separator
 * The print is done on stdout.
 * 
 * @param	s_flags	Flags to be printed
 *
 */
void print_section_flags(Elf64_Xword flags) {
	/* Checking Writable flag */
	if ((flags & 0x1) != 0)
		printf("W");
	else
		printf(" ");
	/* Checking Allocatable flag */
	if ((flags & 0x2) != 0)
		printf("A");
	else
		printf(" ");
	/* Checking eXecutable flag */
	if ((flags & 0x4) != 0)
		printf("X");
	else
		printf(" ");
	printf(" ");
}


/*
 * Print a section header info. Information is printed on 80 columns and 
 * consists of section index, name, addr, offset in file, size, flags
 * and some others.
 * The print is done on stdout.
 * 
 * @param	shr		Pointer to the section to print info about
 * @param	bin		Binary structure
 * @param	shr_index	Index of the section header in section headers
 *				table
 *
 */
void print_shr_info(Elf64_Shdr *shr, ELF *bin, int shr_index) {
	/* Section index */
	printf("  [%2i] ", shr_index);

	/* Section name */
	print_section_name(get_section_name(bin->file, shr, bin->shr[bin->ehr->e_shstrndx]->sh_offset));

	print_section_type(shr->sh_type);

	/* Section address */
	printf("%08x ", (Elf64_Word) shr->sh_addr);

	/* Section offset */
	printf("%06x ", (Elf64_Word) shr->sh_offset);

	/* Section size */
	printf("%06x ", (Elf64_Word) shr->sh_size);

	/* Entry size */
	printf("%02x ", (unsigned char) shr->sh_entsize);

	/* FLAGS */
	print_section_flags(shr->sh_flags);

	/* Link */
	printf("%2u ", shr->sh_link);

    	/* Info */
	printf("%3u ", shr->sh_info);

 	/* Align */
	printf("%2u", (unsigned char) shr->sh_addralign);

	printf("\n");
}


/*
 * Print all section headers info. 
 * The print is done on stdout.
 * 
 * @param	bin		Binary structure
 *
 */
void print_shr_info_all(ELF *bin) {
	size_t i;
	printf("There are %d section headers, starting at offset 0x%lx:\n", 
		bin->ehr->e_shnum, bin->ehr->e_shoff);
	printf("\n");
	printf("Section Headers:\n");
	printf("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al\n");

	for (i = 0; i < bin->ehr->e_shnum; i++) {
		print_shr_info(bin->shr[i], bin, i);
	}
}

