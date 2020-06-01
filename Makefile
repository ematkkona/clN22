CC=gcc
CFLAGS=-I${IDIR}
BDIR=.
SDIR=src
ODIR=src/obj
IDIR=${SDIR}
LDIR=${SDIR}
MKDIR_P = mkdir -p
RSYNC = rsync -rupE

LIBS=-lm -lOpenCL

_DEPS = main.h worker.h common.h assert.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o worker.o assert.o
OBJ = $(patsubst %,$(SDIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all mkdirs copycl clean

all: mkdirs cln22 copybin clean

cln22: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

mkdirs:
	${MKDIR_P} ${BDIR}/work
	${MKDIR_P} ${BDIR}/log
	${MKDIR_P} ${ODIR}

copycl:
	${RSYNC} ${SDIR}/kernel22.cl ${BDIR}

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~