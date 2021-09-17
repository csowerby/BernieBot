
CC = gcc

BIN = bin
OBJ = obj
ASM = asm
SRC = src

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
ASMS = $(patsubst $(SRC)/%.c,$(ASM)/%.s,$(SRCS))

EXE = $(BIN)/BernieBot
FAST-EXE = $(BIN)/fast

CFLAGS= -Wall
LFAGS= -Wall

DEFAULTFLAGS = -Wall -g
SLOWFLAGS = -Wall  -O0
FASTFLAGS = -Wall -O3 -DNDEBUG
DEBUGFLAGS =  -Wall -g -O0

# Make Executable


all: | slowFlags $(EXE)

fast: | fastFlags clean $(EXE)

debug: udDB
	lldb $(EXE)

# update DEBUG - updates the compiled file so I can edit within lldb session
udDB: | debugFlags $(EXE)
#-----------------

debugFlags:
	echo "Making debuggable executable..."
	$(eval CFLAGS = $(DEBUGFLAGS))


slowFlags:
	echo "Making slow executable..."


fastFlags:
	echo "Making fast executable..."
	$(eval CFLAGS = $(FASTFLAGS))

#------------------

# Run executable
run: $(EXE)
	./bin/BernieBot

#------- Compile Everything ------
#Link .o files into executable
$(EXE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $@

# Make .o object files in /obj
$(OBJ)/%.o: $(SRC)/%.c $(SRC)/*.h
	$(CC) $(CFLAGS) -c $< -o $@



# --------- Clean Up ----------
clean:
	rm -r -f bin/* obj/*

veryclean:
	rm -r -f bin/* obj/* asm/*


#---------- Other shit --------
# Make .s assembly files in /asm
$(ASM)/%.s : $(SRC)/%.c $(SRC)/*.h
	$(CC) -Wall -O3 -S $< -o $@

asm: $(ASMS)



# --------- Key in case I forget stuff -------
# $@: Target of current rule
# $^: All pre-requisites
# $<: First pre-requisite of the rule
# $?: All out of date pre-requisites
#
# Use touch shell command to update time stamps of files
