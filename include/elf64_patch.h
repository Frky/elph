
#ifndef __ELF64_PATCH_H__
#define __ELF64_PATCH_H__

#include "types.h"
#include "elf64_shr.h"

void Elf64_patch_binary(ELF *bin, unsigned char *pl, Elf64_Half *pl_size,
		char *out_fname);

#endif
