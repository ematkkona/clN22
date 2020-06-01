IDIR=.
CC=gcc
CFLAGS=-Wall -I.

ODIR=obj
LDIR=.

LIBS=-lm -lOpenCL

_DEPS = main.h worker.h wintime.h classert.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o worker.o assert.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
        $(CC) -c -o $@ $< $(CFLAGS)

cln22: $(OBJ)
        $(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
        rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~