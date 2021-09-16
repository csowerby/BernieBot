
CC = gcc

BIN = bin
OBJ = obj
ASM = asm
SRC = src

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
ASMS = $(patsubst $(SRC)/%.c,$(ASM)/%.s,$(SRCS))

EXE = $(BIN)/BernieBot

CFLAGS= -Wall
DEFAULTFLAGS := -Wall -g
SLOWFLAGS := -Wall -g -O0
FASTFLAGS := -Wall -O2 -DNDEBUG

# Run executable
run: $(EXE)
	./$^

# Make Executable

all: CFLAGS = $(DEFAULTFLAGS)
all: $(EXE)

fast: clean
fast: CFLAGS = $(FASTFLAGS)
fast: $(EXE)

debug: clean
debug: CFLAGS = $(SLOWFLAGS)
debug: $(EXE)


#------- Compile Everything ------
$(EXE): $(OBJS)
	$(CC) -Wall $(OBJS) -o $@

# Make .o object files in /obj
$(OBJ)/%.o: $(SRC)/%.c $(SRC)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

# Make .s assembly files in /asm
$(ASM)/%.s : $(SRC)/%.c $(SRC)/*.h
	$(CC) -Wall -O3 -S $< -o $@

asm: $(ASMS)


#--

# --------- Clean Up ----------
clean:
	rm -r -f bin/* obj/*

veryclean:
	rm -r -f bin/* obj/* asm/*
