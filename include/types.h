
#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off; 	
typedef uint16_t Elf64_Half; 	
typedef uint32_t Elf64_Word;	
typedef int32_t  Elf64_Sword; 	
typedef uint64_t Elf64_Xword; 	
typedef int64_t  Elf64_Sxword; 	

/* Defined in binary.h */
typedef struct ELF_s ELF;

/* Defined in elf64_hdr.h */
typedef struct Elf64_Ehdr_s Elf64_Ehdr;

/* Defined in elf64_shr.h */
typedef struct Elf64_Shdr_s Elf64_Shdr;

/* Defined in sym_tab.h */
typedef struct Elf64_Sym_s Elf64_Sym;

/* Defined in fdetect.h */
typedef struct Elf64_Func_s Elf64_Func;

/* Defined in elf64_phr.h */
typedef struct Elf64_Phdr_s Elf64_Phdr;

#endif
