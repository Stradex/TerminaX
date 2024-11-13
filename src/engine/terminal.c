
#include <engine/terminal.h>

#ifndef __EMSCRIPTEN__
#include <sys/ioctl.h>
#else
#include <util/webcurses.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void update_terminal(Terminal* currentTerminal) {
#ifndef __EMSCRIPTEN__
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	currentTerminal->cols = w.ws_col;
	currentTerminal->rows = w.ws_row;
#else
	currentTerminal->cols = getmaxx(NULL);
	currentTerminal->rows = getmaxy(NULL);
#endif

}

Terminal* get_current_terminal(void) {
	Terminal* t = calloc(1, sizeof(Terminal)); 
	update_terminal(t);
	return t;
}
