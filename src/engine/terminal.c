#include <engine/terminal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

void update_terminal(Terminal* currentTerminal) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	currentTerminal->cols = w.ws_col;
	currentTerminal->rows = w.ws_row;
}

Terminal* get_current_terminal(void) {
	Terminal* t = calloc(1, sizeof(Terminal)); 
	update_terminal(t);
	return t;
}
