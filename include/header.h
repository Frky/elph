
#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdbool.h>
#include "binary.h"
#include "types.h"

#define PRINT_ELF_INFO		1
#define DEBUG_HEADER		0

#define EI_CLASS		0x04
#define EI_DATA			0x05
#define EI_VERSION		0x06
#define EI_OSABI		0x07
#define EI_ABIVERSION		0x08
#define EI_PAD			0x09
#define EI_NIDENT		0x10

#define ELF_CLASS_32		0x01
#define ELF_CLASS_64		0x02

#define ELF_LE			0x01
#define ELF_BE			0x02

#define ELF_VERSION		0x01 

#define ELF_OS_ABI_V		0x00
#define ELF_OS_ABI_HPUX		0x01
#define ELF_OS_ABI_NETBSD	0x02
#define ELF_OS_ABI_LINUX	0x03
#define ELF_OS_ABI_SOLARIS	0x06
#define ELF_OS_ABI_AIX		0x07
#define ELF_OS_ABI_IRIX		0x08
#define ELF_OS_ABI_FREEBSD	0x09
#define ELF_OS_ABI_OPENBSD	0x0C

#define ELF_RELOCATABLE		0x01
#define ELF_EXECUTABLE		0x02
#define ELF_SHARED		0x03
#define ELF_CORE		0x04

#define ELF_SPARC		0x02
#define ELF_X86			0x03
#define ELF_MIPS		0x08
#define ELF_POWERPC		0x14
#define ELF_ARM			0x28
#define ELF_SUPERH		0x2A
#define ELF_IA_64		0x32
#define ELF_X86_64		0x3E
#define ELF_AARCH_64		0xB7

extern const unsigned char ELF_MAGIC_NUMBER[4];

typedef struct {
	unsigned char 	e_ident[EI_NIDENT];
	Elf64_Half	e_type;
	Elf64_Half	e_machine;
	Elf64_Word	e_version;
	Elf64_Addr	e_entry;
	Elf64_Off	e_phoff;
	Elf64_Off	e_shoff;
	Elf64_Word	e_flags;
	Elf64_Half	e_ehsize;
	Elf64_Half	e_phentsize;
	Elf64_Half	e_phnum;
	Elf64_Half	e_shentsize;
	Elf64_Half	e_shnum;
	Elf64_Half	e_shstrndx;
} Elf64_Ehdr;

Elf64_Ehdr *read_header(FILE *bin_file);
void print_binary_info(Elf64_Ehdr *ehr);

#endif
