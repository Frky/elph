
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define JMP_ENTRY_PL_SIZE	6


unsigned char *get_payload(char *pl_fname, Elf64_Half *pl_size) {
	unsigned char *pl;
	FILE *pl_file = fopen(pl_fname, "r");
	fseek(pl_file, 0L, SEEK_END);
	*pl_size = ftell(pl_file);
	fseek(pl_file, 0L, SEEK_SET);
	pl = malloc(*pl_size * sizeof(unsigned char));
	fread(pl, sizeof(unsigned char), *pl_size, pl_file);
	fclose(pl_file);
	return pl;
}
	

unsigned char *Elf64le_jmp_entry_pl(Elf64_Addr ep) {
	unsigned char *pl = malloc(JMP_ENTRY_PL_SIZE * sizeof(char)); 
	pl[0] = 0x68;
	pl[1] = (unsigned char) (ep & 0xFF);
	pl[2] = (unsigned char) ((ep >> 8) & 0xFF);
	pl[3] = (unsigned char) ((ep >> 16) & 0xFF);
	pl[4] = (unsigned char) ((ep >> 24) & 0xFF);
	pl[5] = 0xC3;
	return pl;
}


unsigned char *Elf64le_add_jmp_entry_pl(unsigned char *pl, Elf64_Addr ep, Elf64_Half *pl_size) {
	unsigned char *payload = malloc((*pl_size + JMP_ENTRY_PL_SIZE) * sizeof(char));
	unsigned char *jmp_entry_pl = Elf64le_jmp_entry_pl(ep); 
	Elf64_Half i;
	for (i = 0; i < *pl_size; i++) 
		payload[i] = pl[i];
	for (; i < *pl_size + JMP_ENTRY_PL_SIZE; i++) 
		payload[i] = jmp_entry_pl[i - *pl_size];
	*pl_size += JMP_ENTRY_PL_SIZE;
	free(pl);
	free(jmp_entry_pl);
	return payload;
}


unsigned char *pad_payload(unsigned char *pl, Elf64_Half *pl_size, Elf64_Half target_size, unsigned char pad) {
	unsigned char *payload = malloc(target_size * sizeof(unsigned char));
	size_t i;
	for (i = 0; i < *pl_size; i++)
		payload[i] = pl[i];
	for (; i < target_size; i++) 
		payload[i] = pad;
	*pl_size = target_size;
	free(pl);
	return payload;
}

