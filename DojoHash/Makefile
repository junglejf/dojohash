CC = gcc
ifdef ComSpec
  SEP2=\\
  MD = mkdirp.bat
  RM = del /F /Q
  RD = rd /S /Q
  CREATE = @(echo ^@echo off & echo ^IF EXIST %%1 GOTO END & echo ^MKDIR %%1 & echo ^:END)> mkdirp.bat
else
  SEP2=/
  MD = mkdir -p
  RM = rm -rf
  RD = rm -rf
  CREATE = 
endif

SEP=$(strip $(SEP2))


ODIR = .$(SEP)build
IDIR = .

CFLAGS=-I$(IDIR)


_DEPS = encadeamento_exterior.h cliente.h lista_clientes.h compartimento_hash.h lista_compartimentos.h ufftest.h
DEPS = $(patsubst %,$(IDIR)$(SEP)%,$(_DEPS))

_OBJ = encadeamento_exterior.o cliente.o lista_clientes.o compartimento_hash.o lista_compartimentos.o test_encadeamento_exterior.o
OBJ = $(patsubst %,$(ODIR)$(SEP)%,$(_OBJ))

$(ODIR)$(SEP)%.o: $(IDIR)$(SEP)%.c $(DEPS)
	${CREATE}
	${MD} $(ODIR)
	${CC} -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	${CC} -o $(ODIR)$(SEP)$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	${RM} *~ mkdirp.bat
	${RD} $(ODIR)$(SEP)
