
#ifndef __ELF64_PAYLOAD_H__
#define __ELF64_PAYLOAD_H__

#include "types.h"

#define JMP_ENTRY_PL_SIZE	6

unsigned char *get_payload(char *pl_fname, Elf64_Half *pl_size);
unsigned char *Elf64le_add_jmp_entry_pl(unsigned char *pl, Elf64_Addr ep, Elf64_Half *pl_size);
unsigned char *pad_payload(unsigned char *pl, Elf64_Half *pl_size, Elf64_Half target_size, unsigned char pad);

#endif
