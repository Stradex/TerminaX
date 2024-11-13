#include <util/file_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __EMSCRIPTEN__

#include <emscripten/emscripten.h>
#include <util/em_util.h>

EM_ASYNC_JS(char*, _get_file_text, (), {
  let data = JSON.stringify(await (await fetch(_stack_arg[0])).json());
  let bufferSize = lengthBytesUTF8(data) + 1;
  let buffer = _malloc(bufferSize);
  stringToUTF8(data, buffer, bufferSize);
  return buffer; 
});

const char *get_file_text(const char *path) {
	init_em_params();
	add_string_em_param(path);
  char* data = _get_file_text();
  printf("imported data: %s\n", data);
  return data;
}
#else
const char *get_file_text(const char *path) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Expected file \"%s\" not found", path);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *buffer = malloc(len + 1);

  if (buffer == NULL) {
    fprintf(stderr, "Unable to allocate memory for file");
    fclose(file);
    return NULL;
  }

  fread(buffer, 1, len, file);
  buffer[len] = '\0';

  return (const char *)buffer;
}
#endif
