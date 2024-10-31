# Compiler
CC = gcc

# Flags
CFLAGS = -Iinclude -Wall
LDFLAGS = -lncurses  # Link ncurses

# Directories
SRCDIR = src
ENGINE_SRC = $(SRCDIR)/engine
GAME_SRC = $(SRCDIR)/game
UTIL_SRC = $(SRCDIR)/util
OBJDIR = obj
BINDIR = bin

# Target
TARGET = $(BINDIR)/game

# Source and Object Files
SRC = $(SRCDIR)/main.c $(wildcard $(ENGINE_SRC)/*.c) $(wildcard $(GAME_SRC)/*.c) $(wildcard $(UTIL_SRC)/*.c) 
OBJS = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Create directories
$(shell mkdir -p $(BINDIR) $(OBJDIR)/engine $(OBJDIR)/game $(OBJDIR)/util) 

# Compile target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)
