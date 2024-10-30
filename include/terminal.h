#ifndef TERMINAL_H
#define TERMINAL_H

typedef struct {
	int rows;
	int cols;
} Terminal;

Terminal* get_current_terminal();
void update_terminal(Terminal* currentTerminal);
#endif
