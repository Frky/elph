
#include <stdlib.h>

#include "elf64_write.h"
#include "types.h"
#include "retcodes.h"

/*
 * Write a byte into file
 */
void Elf_write_byte(FILE *bin_file, unsigned char byte) {
	size_t ret = fwrite(&byte, 1, 1, bin_file);
	if (ret != 1) {
		printf("Unexpected EOF (%i)\n", ret);
		exit(EXIT_UNEXPECTED_EOF);
	}
	return;
}


/*
 * Write two bytes into file, little-endian
 *
 * e.g. if half = 0xCAFE, 0xFE will be written 
 * before 0xCA
 *
 */
void Elf64_write_half_le(FILE *bin_file, Elf64_Half half) {
	unsigned char hbyte, lbyte;
	hbyte = (unsigned char) (half >> 8);
	lbyte = (unsigned char) (half & 0xFF);
	Elf_write_byte(bin_file, lbyte);
	Elf_write_byte(bin_file, hbyte);
	return;
}


/*
 * Write four bytes into file, little-endian 
 *
 * e.g. if word = 0xDEADBEEF, the bytes will be written 
 * in the following order: 
 * 0xEF, 0xBE, 0xAD, 0xDE
 *
 */
void Elf64_write_word_le(FILE *bin_file, Elf64_Word word) {
	Elf64_Half hshort, lshort;
	hshort = (Elf64_Half) (word >> 16);
	lshort = (Elf64_Half) (word & 0xFFFF);
	Elf64_write_half_le(bin_file, lshort);
	Elf64_write_half_le(bin_file, hshort);
	return;
}


/*
 * Write eight bytes into file, little-endian 
 *
 * e.g. if word = 0x0123456789ABCDEF, the bytes will be written 
 * in the following order: 
 * 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01
 *
 */
void Elf64_write_xword_le(FILE *bin_file, Elf64_Xword xword) {
	Elf64_Word hword, lword;
	hword = (Elf64_Word) (xword >> 32);
	lword = (Elf64_Word) (xword & 0xFFFFFFFF);
	Elf64_write_word_le(bin_file, lword);
	Elf64_write_word_le(bin_file, hword);
	return;
}


/*
 * Write eight bytes into file, little-endian 
 *
 * e.g. if word = 0x0123456789ABCDEF, the bytes will be written 
 * in the following order: 
 * 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01
 *
 */
void Elf64_write_off_le(FILE *bin_file, Elf64_Off off) {
	Elf64_write_xword_le(bin_file, (Elf64_Xword) off);
	return; 
}


/*
 * Write eight bytes into file, little-endian 
 *
 * e.g. if word = 0x0123456789ABCDEF, the bytes will be written 
 * in the following order: 
 * 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01
 *
 */
void Elf64_write_addr_le(FILE *bin_file, Elf64_Addr addr) {
	Elf64_write_xword_le(bin_file, (Elf64_Xword) addr);
	return;
}
