
CC = gcc

BIN = bin
OBJ = obj
ASM = asm
SRC = src
SRCS = $(wildcard $(SRC)/*.c)

OBJS = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

ASMS = $(patsubst $(SRC)/%.c,$(ASM)/%.s,$(SRCS))

EXE = $(BIN)/BernieBot

CFLAGS= -Wall -g

# Make Executable
all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -Wall $(OBJS) -o $@

# Make .o object files in /obj
$(OBJ)/%.o: $(SRC)/%.c $(SRC)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

# Make .s assembly files in /asm
$(ASM)/%.s : $(SRC)/%.c $(SRC)/*.h
	$(CC) $(CFLAGS) -S $< -o $@

asm: $(ASMS)





# Run executable
run: $(EXE)
	./$^



clean:
	rm -r -f bin/* obj/*

veryclean:
	rm -r -f bin/* obj/* asm/*
