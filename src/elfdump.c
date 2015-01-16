
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "elf64.h"
#include "elf64_hdr.h"
#include "elf64_shr.h"
#include "elf64_phr.h"
#include "sym_tab.h"
#include "elf64_patch.h"
#include "fdetect.h"
#include "retcodes.h"
#include "elf64_payload.h"

static int PRINT_HEADER_INFO = 0;
static int PRINT_SECTION_HEADERS = 0;
static int PRINT_PROGRAM_HEADERS = 0;
static int PRINT_SYMBOLS = 0;
static int PRINT_FUNCS = 0;

static int PATCH = 0;
static int JMP_ENTRY = 0;

char *pl_fname;
char *out_fname = NULL;

void parse_args(int argc, char **argv) {

	int c;

	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"debug", no_argument, 		&DEBUG_HEADER, 1},
			{"header", no_argument, 	&PRINT_HEADER_INFO, 1},
			{"sections", no_argument, 	&PRINT_SECTION_HEADERS, 1},
			{"segments", no_argument, 	&PRINT_PROGRAM_HEADERS, 1},
			{"symbols", no_argument, 	&PRINT_SYMBOLS, 1},
			{"functions", no_argument, 	&PRINT_FUNCS, 1},
			{"payload", required_argument, 	&PATCH, 1},
			{"output", required_argument, 	0, 0},
			/* These options don't set a flag.
			   We distinguish them by their indices. */
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "dhslSfp:jo:",
				long_options, &option_index);

		switch (c) {
		case 'd':
			DEBUG_HEADER = 1;
			break;
		case 'h':
			PRINT_HEADER_INFO = 1;
			break;
		case 's':
			PRINT_SYMBOLS = 1;
			break;
		case 'S':
			PRINT_SECTION_HEADERS = 1;
			break;
		case 'l':
			PRINT_PROGRAM_HEADERS = 1;
			break;
		case 'f':
			PRINT_FUNCS = 1;
			break;
		case 'j':
			JMP_ENTRY = 1;
			break;
		case 'p':
			PATCH = 1;
			pl_fname = malloc(strlen(optarg)+1);
			strcpy(pl_fname, optarg);
			break;
		case 'o':
			out_fname = malloc(strlen(optarg)+1);
			strcpy(out_fname, optarg);
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

	parse_args(argc, argv);

	if (out_fname == NULL) {
		out_fname = malloc(8 * sizeof(char));
		strcpy(out_fname, "patched");
	}

	ELF *bin = elf64_read(argv[argc - 1]);
	bin->ftab = get_func(bin);

	if (PRINT_HEADER_INFO)
		print_header_info(bin->ehr);

	if (PRINT_SECTION_HEADERS)
		print_shr_info_all(bin);

	if (PRINT_PROGRAM_HEADERS)
		Elf64_print_phr_info(bin);

	if (PRINT_SYMBOLS)
		print_sym_info(bin);

	if (PRINT_FUNCS)
		print_func_info(bin->ftab, bin->ftab_num);

	if (PATCH) {
		Elf64_Half pl_size;
		unsigned char *pl = get_payload(pl_fname, &pl_size);
		if (JMP_ENTRY)
			pl = Elf64le_add_jmp_entry_pl(pl, bin->ehr->e_entry, 
									&pl_size);
		Elf64_patch_binary(bin, pl, &pl_size, out_fname);
	}

	fclose(bin->file);

	return 0;
}

