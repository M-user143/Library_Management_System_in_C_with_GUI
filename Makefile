# Makefile for LMS_Gui GTK project

# Compiler
CC = gcc

# Source and output
SRC = LMS_Gui_main.c core_functions.c 
OUT = LMS_Gui.exe

# Compiler and linker flags from pkg-config
CFLAGS  = $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

# Default target
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

# Clean target
clean:
	del /Q $(OUT) 2>nul || true

.PHONY: all clean
