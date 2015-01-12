
SRCDIR=src
OBJDIR=obj
HDRDIR=include

EXEC=elfdump
CC=gcc
CFLAGS=-W -Wall -Wextra -I $(HDRDIR) -g
LDFLAGS=

_DEPS=binary.h elf64_read.h header.h section.h sym_tab.h fdetect.h elf64_program_header.h 
_SRC=elfdump.c elf64_read.c header.c section.c sym_tab.c fdetect.c elf64_program_header.c
_OBJ=$(_SRC:.c=.o)
DEPS=$(patsubst %,$(HDRDIR)/%,$(_DEPS))
OBJ=$(patsubst %,$(OBJDIR)/%,$(_OBJ))
SRC=$(patsubst %,$(SRCDIR)/%,$(_SRC))

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJDIR)/$(EXEC).o: $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf $(OBJDIR)/*.o

mproper: clean
	rm -rf $(EXEC)
