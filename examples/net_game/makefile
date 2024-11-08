# Compiler
CC = gcc


# Directories
ROOTDIR = ../../
SRCDIR = $(ROOTDIR)/src
ENGINE_SRC = $(SRCDIR)/engine
ENGINE_INCLUDE = $(ROOTDIR)/include
GAME_SRC = $(SRCDIR)/game
UTIL_SRC = $(SRCDIR)/util
OBJDIR = obj
BINDIR = bin

# Flags
CFLAGS = -I$(ENGINE_INCLUDE) -Wall -DJSON_SKIP_WHITESPACE
LDFLAGS = -lncurses  # Link ncurses


# Target
TARGET = $(BINDIR)/net_game

# Source and Object Files
SRC = $(wildcard $(ENGINE_SRC)/*.c) $(wildcard $(GAME_SRC)/*.c) $(wildcard $(UTIL_SRC)/*.c) 
OBJS = $(OBJDIR)/main.o $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Create directories
$(shell mkdir -p $(BINDIR) $(OBJDIR)/engine $(OBJDIR)/game $(OBJDIR)/util) 

# Compile target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule to compile main.c specifically in examples/net_game
$(OBJDIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@  # Ensure ENGINE_INCLUDE is in CFLAGS

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)
