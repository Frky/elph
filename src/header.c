
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"
#include "binary.h"
#include "retcodes.h"

const unsigned char ELF_MAGIC_NUMBER[4] = {0x7F, 0x45, 0x4C, 0x46}; 

bool read_magic(Elf64_Ehdr *ehr, FILE *bin) {
	size_t i;
	bool magic_elf = true;

#if DEBUG_HEADER
	printf("Magic number: ");
#endif
	for (i = 0; i < 4; i++) {
		ehr->e_ident[i] = Elf_read_byte(bin);
		if (ehr->e_ident[i] != ELF_MAGIC_NUMBER[i])
			magic_elf = false;		 
#if DEBUG_HEADER
		printf("%x", ehr->e_ident[i]);
#endif
	}
#if DEBUG_HEADER
	printf("\n");
#endif
	return magic_elf;
}

bool read_elf_class(Elf64_Ehdr *ehr, FILE *bin) {
	ehr->e_ident[EI_CLASS] = Elf_read_byte(bin);
#if DEBUG_HEADER
	printf("Class: %x\n", ehr->e_ident[EI_CLASS]);
#endif
	return ehr->e_ident[EI_CLASS] == ELF_CLASS_32 
			|| ehr->e_ident[EI_CLASS] == ELF_CLASS_64;
}

bool read_interp(Elf64_Ehdr *ehr, FILE *bin) {
	ehr->e_ident[EI_DATA] = Elf_read_byte(bin);
#if DEBUG_HEADER
	printf("Interpretation: %x\n", ehr->e_ident[EI_DATA]);
#endif
	return ehr->e_ident[EI_DATA] == ELF_LE 
			|| ehr->e_ident[EI_DATA] == ELF_BE;
}

bool read_elf_version_one_byte(Elf64_Ehdr *ehr, FILE *bin) {
	ehr->e_ident[EI_VERSION] = Elf_read_byte(bin);
#if DEBUG_HEADER
	printf("Version: %x\n", ehr->e_ident[EI_VERSION]);
#endif
	return ehr->e_ident[EI_VERSION] == ELF_VERSION;
}

void read_os_abi(Elf64_Ehdr *ehr, FILE *bin) {
	/* Read OS ABI, unused */
	ehr->e_ident[EI_OSABI] = Elf_read_byte(bin); 
#if DEBUG_HEADER
	printf("OS ABI: %x\n", ehr->e_ident[EI_OSABI]);
#endif
	/* Read OS ABI version, unused */
	ehr->e_ident[EI_ABIVERSION] = Elf_read_byte(bin);
#if DEBUG_HEADER
	printf("ABI version: %x\n", ehr->e_ident[EI_ABIVERSION]);
#endif
	return;
}

void read_pad(Elf64_Ehdr *ehr, FILE *bin) {
	size_t i;
	/* Read PAD, currently unused */
	for (i = 0; i < 7; i++) 
		ehr->e_ident[EI_PAD + i] = Elf_read_byte(bin);
	return;
}

bool read_type_of_bin(Elf64_Ehdr *ehr, FILE *bin) {
	ehr->e_type = Elf64_read_half_le(bin);
#if DEBUG_HEADER
	printf("Type of binary: %x\n", ehr->e_type);
#endif
	return ehr->e_type == ELF_RELOCATABLE 
			|| ehr->e_type == ELF_EXECUTABLE
			|| ehr->e_type == ELF_SHARED 
			|| ehr->e_type == ELF_CORE;
}

bool read_architecture(Elf64_Ehdr *ehr, FILE *bin) {
	ehr->e_machine = Elf64_read_half_le(bin);
#if DEBUG_HEADER
	printf("Architecture: %x\n", ehr->e_machine);
#endif
	return ehr->e_machine == ELF_SPARC
			|| ehr->e_machine == ELF_X86
			|| ehr->e_machine == ELF_MIPS
			|| ehr->e_machine == ELF_POWERPC
			|| ehr->e_machine == ELF_ARM
			|| ehr->e_machine == ELF_SUPERH
			|| ehr->e_machine == ELF_IA_64
			|| ehr->e_machine == ELF_X86_64
			|| ehr->e_machine == ELF_AARCH_64;
}

bool read_elf_version_four_bytes(Elf64_Ehdr *ehr, FILE *bin) {
	ehr->e_version = Elf64_read_word_le(bin);
#if DEBUG_HEADER
	printf("ELF Version: %x\n", ehr->e_version);
#endif
	return ehr->e_version == ELF_VERSION;
}

Elf64_Ehdr *read_header(FILE *bin_file) {
	
	Elf64_Ehdr *ehr = malloc(sizeof(Elf64_Ehdr));

	fseek(bin_file, 0L, SEEK_SET);

	/* Check if it is an ELF binary */
	if (!read_magic(ehr, bin_file)) {
		printf("Not an ELF binary\n");
		exit(EXIT_NOT_ELF);
	}
	
	/* Get ELF class: 32 or 64 bits ? */
	if (!read_elf_class(ehr, bin_file)) {
		printf("Unknown ELF class: %x\n", ehr->e_ident[EI_CLASS]);
		exit(EXIT_UNKNOWN_ELF_CLASS);
	}	

	/* Get word interpretation: LE/BE ? */
	if (!read_interp(ehr, bin_file)) {
		printf("Unknown interpretation: %x\n", ehr->e_ident[EI_DATA]);
		exit(EXIT_UNKNOWN_ELF_INTERP);
	}	

	/* Read ELF version (should be 1) */
	if (!read_elf_version_one_byte(ehr, bin_file)) {
		printf("Unknown ELF version\n");
		exit(EXIT_UNKNOWN_ELF_VERSION);
	}

	/* Read OS ABI: unused */
	read_os_abi(ehr, bin_file);

	/* Read PAD: unused */
	read_pad(ehr, bin_file);
	
	/* Read type of binary (shared, reloc, etc.) */
	if (!read_type_of_bin(ehr, bin_file)) {
		printf("Unknown type of binary: %x\n", ehr->e_type);
		exit(EXIT_UNKNOWN_ELF_TYPE);
	}	

	/* Read the target architecture */
	if (!read_architecture(ehr, bin_file)) {
		printf("Unknown target architecture: %x\n", ehr->e_machine);
		exit(EXIT_UNKNOWN_ELF_ARCHITECTURE);
	}

	/* Read ELF version (should be 1) */
	if (!read_elf_version_four_bytes(ehr, bin_file)) {
		printf("Unknown ELF version: %x\n", ehr->e_version);
		exit(EXIT_UNKNOWN_ELF_VERSION);
	}

	/* Read the entry point of the program */
	ehr->e_entry = Elf64_read_addr_le(bin_file);
#if DEBUG_HEADER
	printf("Entry point: %lx\n", ehr->e_entry);
#endif

	/* Read the program header table offset */
	ehr->e_phoff = Elf64_read_addr_le(bin_file);

	/* Read the section header table offset */
	ehr->e_shoff = Elf64_read_addr_le(bin_file);

	/* Read the flags */
	ehr->e_flags = Elf64_read_word_le(bin_file);

	/* Read the ELF header size */
	ehr->e_ehsize = Elf64_read_half_le(bin_file);

	/* Read the size of a program table entry */
	ehr->e_phentsize = Elf64_read_half_le(bin_file);

	/* Read the number of entries in section table */
	ehr->e_phnum = Elf64_read_half_le(bin_file);

	/* Read the size of a section table entry */
	ehr->e_shentsize = Elf64_read_half_le(bin_file);

	/* Read the number of entries in program table */
	ehr->e_shnum = Elf64_read_half_le(bin_file);

	/* Read section name string table index */
	ehr->e_shstrndx = Elf64_read_half_le(bin_file);

#if PRINT_ELF_INFO
	print_binary_info(ehr);
#endif

	return ehr;
}

void print_os_abi(unsigned char osabi) {
	switch (osabi) {
	case ELF_OS_ABI_V:
		printf("UNIX - System V");
		break;
	case ELF_OS_ABI_HPUX:
		printf("UNIX - HP-UX");
		break;
	case ELF_OS_ABI_NETBSD:
		printf("UNIX - NetBSD");
		break;
	case ELF_OS_ABI_LINUX:
		printf("UNIX - GNU");
		break;
	case ELF_OS_ABI_SOLARIS:
		printf("UNIX - Solaris");
		break;
	case ELF_OS_ABI_AIX:
		printf("UNIX - AIX");
		break;
	case ELF_OS_ABI_IRIX:
		printf("UNIX - IRIX");
		break;
	case ELF_OS_ABI_FREEBSD:
		printf("UNIX - FreeBSD");
		break;
	case ELF_OS_ABI_OPENBSD:
		printf("UNIX - OpenBSD");
		break;
	}
}

void print_type(unsigned char type) {
	switch (type) {
	case ELF_RELOCATABLE:
		printf("REL (Relocatable file)");
		break;
	case ELF_EXECUTABLE:
		printf("EXEC (Executable file)");
		break;
	case ELF_SHARED:
		printf("DYN (Shared object file)");
		break;
	case ELF_CORE:
		printf("CORE (Core file)");
		break;
	}
	return;
}

void print_machine(unsigned char e_machine) {
	switch (e_machine) {
	case ELF_SPARC:
		printf("Sparc");
		break;
	case ELF_X86:
		printf("Intel 80386");
		break;
	case ELF_MIPS:
		printf("MIPS R3000");
		break;
	case ELF_POWERPC:
		printf("PowerPC");
		break;
	case ELF_ARM:
		printf("ARM");
		break;
	case ELF_SUPERH:
		printf("Renesas / SuperH SH");
		break;
	case ELF_IA_64:
		printf("Intel IA-64");
		break;
	case ELF_X86_64:
		printf("Advanced Micro Devices X86-64");
		break;
	case ELF_AARCH_64:
		printf("AArch64");
		break;
	}
}

#define PRINT_VALUE_OFFSET	37

void print_line_entry(char *entry_name) {
	size_t i;
	size_t l = strlen(entry_name);
	printf("  %s", entry_name);
	for (i = l + 2; i < PRINT_VALUE_OFFSET; i++)
		printf(" ");
	return;
}

void print_binary_info(Elf64_Ehdr *ehr) {

	size_t i;

	printf("ELF Header:\n");
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++) {
		printf("%02x ", ehr->e_ident[i]);
	}
	printf("\n");

	print_line_entry("Class:");
	printf("ELF%i\n", (ehr->e_ident[EI_CLASS]== ELF_CLASS_32)?32:64); 

	print_line_entry("Data:");
	if (ehr->e_ident[EI_DATA] == ELF_LE) 
		printf("2's complement, little endian\n");
	else
		printf("2's complement, big endian\n");

	print_line_entry("Version:");
	printf("%d ", ehr->e_ident[EI_VERSION]);
	if (ehr->e_ident[EI_VERSION] == 1)
		printf("(current)\n");
	else
		printf("(unknown)\n");

	print_line_entry("OS/ABI:");
	print_os_abi(ehr->e_ident[EI_OSABI]);
	printf("\n");

	print_line_entry("ABI Version:");
	printf("%x\n", ehr->e_ident[EI_ABIVERSION]);

	print_line_entry("Type:");
	print_type(ehr->e_type);
	printf("\n");

	print_line_entry("Machine:");
	print_machine(ehr->e_machine);
	printf("\n");

	print_line_entry("Version:");
	printf("0x%x\n", ehr->e_version);

	print_line_entry("Entry point address:");
	printf("0x%lx\n", ehr->e_entry);

	print_line_entry("Start of program headers:");
	printf("%lu (bytes into file)\n", ehr->e_phoff);

	print_line_entry("Start of section headers:");
	printf("%lu (bytes into file)\n", ehr->e_shoff);

	print_line_entry("Flags:");
	printf("0x%x\n", ehr->e_flags);

	print_line_entry("Size of this header:");
	printf("%hu (bytes)\n", ehr->e_ehsize);

	print_line_entry("Size of program headers:");
	printf("%hu (bytes)\n", ehr->e_phentsize);

	print_line_entry("Number of program headers:");
	printf("%hu\n", ehr->e_phnum);

	print_line_entry("Size of section headers:");
	printf("%hu (bytes)\n", ehr->e_shentsize);

	print_line_entry("Number of section headers:");
	printf("%hu\n", ehr->e_shnum);

	print_line_entry("Section header string table index:");
	printf("%hu\n", ehr->e_shstrndx);

	return;
}
