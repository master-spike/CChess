# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = gcc                        # compiler to use

ODIR = build
SDIR = src

LINKERFLAG = -lm

_OBJS = bitboard.o evaluation.o minimax.o main.o patterns.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
OUT = $(ODIR)/main

all: ${OUT}

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -g -c $(INC) -o $@ $< -O2

$(OUT): ${OBJS}
	${CC} -g ${LINKERFLAG} $^ -o $@ -O2

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)
