
#include <stdlib.h>

#include "elf64_read.h"
#include "types.h"
#include "retcodes.h"


/*
 * Read a byte in file at current position
 *
 */
unsigned char Elf_read_byte(FILE *bin_file) {
	unsigned char byte;
	if (fread(&byte, 1, 1, bin_file) != 1) {
		printf("Unexpected EOF\n");
		exit(EXIT_UNEXPECTED_EOF);
	}
#if DEBUG_READING
	printf("BYTE read: %x\n", byte);
#endif
	return byte;
}


/*
 * Read two bytes in file at current position, little-endian
 *
 * e.g. if the file contains 0xFE 0xCA, returns 0xCAFE
 *
 */
Elf64_Half Elf64_read_half_le(FILE *bin_file) {
	unsigned char hbyte, lbyte;
	Elf64_Half half;
	hbyte = Elf_read_byte(bin_file);
	lbyte = Elf_read_byte(bin_file);
	half = ((Elf64_Half) lbyte << 8) | (Elf64_Half) hbyte;
#if DEBUG_READING
	printf("HALF read: %x\n", half);
#endif
	return half;
}


/*
 * Read four bytes in file at current position, little-endian
 *
 * e.g. if the file contains 0xEF 0XBE 0XAD 0xDE, 
 * returns 0xDEADBEEF
 *
 */
Elf64_Word Elf64_read_word_le(FILE *bin_file) {
	Elf64_Half hshort, lshort;
	Elf64_Word word;
	hshort = Elf64_read_half_le(bin_file);
	lshort = Elf64_read_half_le(bin_file);
	word = ((Elf64_Word) lshort << 16) | (Elf64_Word) hshort;
#if DEBUG_READING
	printf("WORD read: %x\n", word);
#endif
	return word;
}


/*
 * Read eight bytes in file at current position, little-endian
 *
 * e.g. if the file contains 0xEF 0XCD 0XAB 0x89 0x67 0x45 0x23 0x01, 
 * returns 0x0123456789ABCDEF
 *
 */
Elf64_Xword Elf64_read_xword_le(FILE *bin_file) {
	Elf64_Word hword = 0, lword = 0;
	Elf64_Addr addr;
	hword = Elf64_read_word_le(bin_file);
	lword = Elf64_read_word_le(bin_file);
	addr = ((Elf64_Addr) lword << 32) | (Elf64_Addr) hword;
#if DEBUG_READING
	printf("ADDR read: %lx\n", addr);
#endif
	return addr;
}


/*
 * Read eight bytes in file at current position, little-endian
 *
 * e.g. if the file contains 0xEF 0XCD 0XAB 0x89 0x67 0x45 0x23 0x01, 
 * returns 0x0123456789ABCDEF
 *
 */
Elf64_Off Elf64_read_off_le(FILE *bin_file) {
	return (Elf64_Off) Elf64_read_xword_le(bin_file);
}


/*
 * Read eight bytes in file at current position, little-endian
 *
 * e.g. if the file contains 0xEF 0XCD 0XAB 0x89 0x67 0x45 0x23 0x01, 
 * returns 0x0123456789ABCDEF
 *
 */
Elf64_Addr Elf64_read_addr_le(FILE *bin_file) {
	return (Elf64_Addr) Elf64_read_xword_le(bin_file);
}
