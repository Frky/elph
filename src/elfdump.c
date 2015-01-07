
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>

#include "binary.h"
#include "header.h"
#include "section.h"
#include "retcodes.h"

#define	FIND_FUNCTIONS	0

static int PRINT_HEADER_INFO = 0;
static int PRINT_SECTION_HEADERS = 0;

#if FIND_FUNCTIONS

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

#endif


void parse_args(int argc, char **argv) {

	int c;

	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"debug", no_argument, 		&DEBUG_HEADER, 1},
			{"header", no_argument, 	&PRINT_HEADER_INFO, 1},
			{"sections", no_argument, 		&PRINT_SECTION_HEADERS, 1},
			/* These options don't set a flag.
			   We distinguish them by their indices. */
#if 0
			{"file",    required_argument, 0, 'f'},
#endif
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "dhS",
				long_options, &option_index);

		switch (c) {
		case 'd':
			DEBUG_HEADER = 1;
			break;
		case 'h':
			PRINT_HEADER_INFO = 1;
			break;
		case 'S':
			PRINT_SECTION_HEADERS = 1;
			break;
		}
		/* Detect the end of the options. */
		if (c == -1)
			break;
	}

	/* Check if a file is given as final argument */
	if (optind != argc - 1)
	{
		
		printf("Usage: %s path/to/executable\n", argv[0]);
		exit(EXIT_NO_FILE_ARGUMENT);
	}
	
	return;
}


int main(int argc, char **argv) {
	ELF *bin = malloc(sizeof(ELF));

	parse_args(argc, argv);

#if FIND_FUNCTIONS
	function_list_t *fns = find_calls(bin);
	function_list_t *senti = fns;
#endif

	bin->file = fopen(argv[argc - 1], "r");
	bin->ehr = read_header(bin->file);
	bin->shr = read_shr_all(bin->file, bin->ehr->e_shnum, 
			bin->ehr->e_shoff, bin->ehr->e_shentsize);

	if (PRINT_HEADER_INFO)
		print_header_info(bin->ehr);

	if (PRINT_SECTION_HEADERS)
		print_shr_info_all(bin);

#if FIND_FUNCTIONS
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

