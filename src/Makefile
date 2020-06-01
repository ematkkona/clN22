IDIR=.
CC=gcc
CFLAGS=-Wall -I.

ODIR=obj
LDIR=.
MKDIR_P = mkdir -p
RSYNC = rsync -rupE
OUT_DIR = ../

LIBS=-lm -lOpenCL

_DEPS = main.h worker.h common.h assert.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o worker.o assert.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
        $(CC) -c -o $@ $< $(CFLAGS)

cln22: $(OBJ)
        $(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: copybin

copybin:
        ${RSYNC} cln22 ${OUT_DIR}

.PHONY: directories

directories: ${OUT_DIR}
        ${OUT_DIR}:
         ${MKDIR_P} cln22 ${OUT_DIR}
    
.PHONY: clean

clean:
        rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
