#ifdef __EMSCRIPTEN__

#include <util/em_util.h>
#include <emscripten/emscripten.h>

void init_em_params() {
	EM_ASM(
		_stack_arg = [];
	);
}

void add_int_em_param(int arg) {
	EM_ASM({
		_stack_arg.push($0);
	}, arg);
}
void add_char_em_param(char arg) {
	EM_ASM({
		_stack_arg.push($0);
	}, arg);
}

void add_string_em_param(const char* arg) {
	EM_ASM({
		_stack_arg.push(UTF8ToString($0));
	}, arg);
}

#endif
