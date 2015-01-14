
#include "types.h"
#include "elf64.h"

#define PT_NULL		0x0
#define PT_LOAD		0x1
#define PT_DYNAMIC	0x2
#define PT_INTERP	0x3
#define PT_NOTE		0x4
#define PT_SHLIB	0x5
#define PT_PHDR		0x6
#define PT_LOOS		0x60000000
#define PT_HIOS		0x6FFFFFFF
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7FFFFFFF

#define PF_X		0x1
#define PF_W		0x2
#define PF_R		0x4
#define PF_MASKOS	0x00FF0000
#define PF_PASKPROC	0xFF000000

struct Elf64_Phdr_s {
	Elf64_Word p_type; 	/* Type of segment */
	Elf64_Word p_flags; 	/* Segment attributes */
	Elf64_Off p_offset; 	/* Offset in file */
	Elf64_Addr p_vaddr; 	/* Virtual address in memory */
	Elf64_Addr p_paddr; 	/* Reserved */
	Elf64_Xword p_filesz; 	/* Size of segment in file */
	Elf64_Xword p_memsz; 	/* Size of segment in memory */
	Elf64_Xword p_align; 	/* Alignment of segment */
};

Elf64_Phdr **Elf64_read_phr_all(FILE *bin, 
					Elf64_Half phr_num, 
					Elf64_Off phr_off, 
					Elf64_Half phr_entrysize);
					
void Elf64_write_phr_all(FILE *bin, 
					Elf64_Phdr **phr_tab,
					Elf64_Half phr_num, 
					Elf64_Off phr_off, 
					Elf64_Half phr_entrysize);

Elf64_Phdr *Elf64_get_pnote(ELF *bin);
Elf64_Phdr *Elf64_get_pcode(ELF *bin);

void Elf64_print_phr_info(ELF *bin);
