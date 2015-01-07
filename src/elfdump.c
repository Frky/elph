
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "binary.h"
#include "header.h"
#include "retcodes.h"

#define DEBUG			1

#define OPCODE_CALL		0xE8
#define OPCODE_RET		0xC3


uint64_t program_header_table;
uint64_t section_header_table;

typedef struct function_list function_list_t;

struct function_list {
	uint32_t addr;
	function_list_t *next;	
};

void find_rets(FILE *bin, uint32_t min, uint32_t max) {
	uint8_t opcode;
	uint32_t offset = min;
	size_t read;

	fseek(bin, offset, 0);
	read = fread(&opcode, 1, 1, bin);
	while (read != 0 && offset < max) {
		if (opcode == OPCODE_RET) {
			printf("RET found at offset %x\n", offset);
		}
		read = fread(&opcode, 1, 1, bin);
		offset += read;
	}
	return;
}

function_list_t *find_calls(FILE *bin) {
	int relative_addr;
	uint8_t opcode;
	uint32_t addr;
	uint32_t offset = 0;
	size_t read = fread(&opcode, 1, 1, bin);
	function_list_t *fns = malloc(sizeof(function_list_t));
	function_list_t *senti = NULL;
	function_list_t *tmp = NULL;

	fseek(bin, 0L, SEEK_SET);
	fns->addr = 0;
	fns->next = NULL;

	while (read != 0) {
		if (opcode == OPCODE_CALL) {
			printf("CALL found at offset %x\n", offset);
			fread(&relative_addr, 4, 1, bin);
			offset += 4;
			addr =  offset + relative_addr;
			senti = fns;
			while (senti->next != NULL && senti->next->addr < addr) {
				senti = senti->next;
			}
			tmp = senti->next;
			senti->next = malloc(sizeof(function_list_t));
			senti = senti->next;
			senti->addr = addr;
			senti->next = tmp;
		}
		read = fread(&opcode, 1, 1, bin);
		offset += read;
	}
	return fns;
}

#if 0



#endif

int main(void) {
	FILE *bin;
	Elf64_Ehdr *ehr;
#if 0
	function_list_t *fns = find_calls(bin);
	function_list_t *senti = fns;
#endif
	bin = fopen("./test", "r");

/*
	fseek(bin, 0L, SEEK_END);
	size = ftell(bin.file);
*/
	ehr = read_header(bin);
#if 0
	while (senti != NULL) {
		min = senti->addr;
		if (senti->next != NULL) {
			max = senti->next->addr + 1;
		} else {
			max = file_size;
		}
		printf("Function addr: %x\n", senti->addr);
		find_rets(bin, min, max);
		senti = senti->next;
	}
#endif
	return 0;
}

