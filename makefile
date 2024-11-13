all: compile compile_wasm clean

# Compiler
CC = gcc
EMCC = emcc

# Flags
CFLAGS = -Iinclude -Wall -DJSON_SKIP_WHITESPACE
LDFLAGS = -lncurses  # Link ncurses
EMFLAGS = -s SINGLE_FILE=1 -s ASYNCIFY=1 --pre-js termlib_min.js # Flags for emcc

# Directories
SRCDIR = src
ENGINE_SRC = $(SRCDIR)/engine
GAME_SRC = $(SRCDIR)/game
UTIL_SRC = $(SRCDIR)/util
OBJDIR = obj
BINDIR = bin

# Target
TARGET = $(BINDIR)/game
WEB_TARGET = $(BINDIR)/game.js

# Source and Object Files
SRC = $(SRCDIR)/main.c $(wildcard $(ENGINE_SRC)/*.c) $(wildcard $(GAME_SRC)/*.c) $(wildcard $(UTIL_SRC)/*.c) 
OBJS = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Create directories
$(shell mkdir -p $(BINDIR) $(OBJDIR)/engine $(OBJDIR)/game $(OBJDIR)/util) 

#GCC Compilation
compile: $(TARGET)

# Compile target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# WASM compilation
compile_wasm: $(WEB_TARGET)

# Compile with emcc for WebAssembly directly from .c files
$(WEB_TARGET): $(SRC)
	$(EMCC) $(CFLAGS) $(EMFLAGS) $(SRC) -o $@ 


# Clean
clean:
	rm -rf $(OBJDIR)
