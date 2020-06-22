CC=gcc
CFLAGS=-I$(SDIR) -L$(SDIR)
BDIR=.
SDIR=src/
IDIR=$(SDIR)
LDIR=$(SDIR)
MKDIR_P = mkdir -p
ifdef OS
	RM=cmd //C del //Q
	RSYNC=cmd //C copy //V
	FixPath = $(subst /,\,$1)
else
	RM=rm -f
	RSYNC=rsync -rupE
	FixPath = $1
endif

LIBS=-lm -lOpenCL

_DEPS = main.h worker.h common.h assert.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o worker.o assert.o
OBJ = $(patsubst %,$(SDIR)/%,$(_OBJ))

$(SDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all mkdirs copycl clean

all: mkdirs cln22 copycl clean

cln22: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

mkdirs:
	$(MKDIR_P) $(BDIR)/work
	$(MKDIR_P) $(BDIR)/log

copycl:
	$(RSYNC) $(call FixPath,$(SDIR)/kernel22.cl $(BDIR))

clean:
	$(RM) $(call FixPath,$(SDIR)/*.o)