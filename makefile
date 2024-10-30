# Compiler
CC = gcc

# Flags
CFLAGS = -Iinclude -Wall
LDFLAGS = -lncurses  # Link ncurses

# Directories
SRCDIR = src
ENGINE_SRC = $(SRCDIR)/engine
OBJDIR = obj
BINDIR = bin

# Target
TARGET = $(BINDIR)/game

# Source and Object Files
SRC = $(SRCDIR)/main.c $(wildcard $(ENGINE_SRC)/*.c)
OBJS = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Create directories
$(shell mkdir -p $(BINDIR) $(OBJDIR)/engine)

# Compile target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)
