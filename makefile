CC      = g++
LD 		= g++
CFLAGS  = -Wall -g -std=c++20
LDFLAGS = 

IDIR = inc
INC = JsonEntity.hpp Array.hpp Object.hpp Value.hpp string.hpp
INC_PATH = $(addprefix $(IDIR)/, $(INC))

ODIR = obj
OBJ = JsonEntity.o Array.o Object.o Value.o main.o string.o
OBJ_PATH = $(addprefix $(ODIR)/, $(OBJ))

BDIR = bin
BIN = main.exe
BIN_PATH = $(addprefix $(BDIR)/, $(BIN))

SDIR = src

main: $(OBJ_PATH) $(INC_PATH)
	$(LD) $(LDFLAGS) -o bin/main.exe $(OBJ_PATH) 

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean run

clean:
	rm -f $(BIN_PATH)
	rm -f $(OBJ_PATH)

run: main
	cmd /c .\bin\main
	

