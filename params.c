#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// check if commandline argument (string) ends with string ".wav"
int ends_with(char *end, char *str) {
  int len = strlen(str);
  int endlen = strlen(end);
  const char *end_of_str = &str[len - endlen];

  if (strncmp(end_of_str, end, endlen) != 0)
    return -1;

  return 0;
}

// check if commmandline argument (string) starts with string "hw:"
int starts_with(char *start, char *str) {
  int startlen = strlen(start);  

  if (strncmp(str, start, startlen) != 0)
    return -1;

  return 0;
}

void usage() {
  fprintf(stderr, "usage: mittaa-aanenkorkeuden file.wav|hw:1,0\n");
  exit(1);
}
