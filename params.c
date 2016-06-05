#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_if_wav(char *str) {
  int len = strlen(str);

  // got to have at least 5 characters, eg. j.wav
  if (len < 5)
    return -1;

  const char *last_four = &str[len - 4];
  char *whattolookfor = ".wav";

  if (strncmp(last_four, whattolookfor, 4) != 0)
    return -1;

  return 0;
}

int check_if_device(char *str) {
  int len = strlen(str);

  // got to have at least 4 characters, eg. hw:1
  if (len < 4)
    return -1;

  char *whattolookfor = "hw:";

  if (strncmp(str, whattolookfor, 3) != 0)
    return -1;

  return 0;
}

void usage() {
  fprintf(stderr, "usage: mittaa-aanenkorkeuden file.wav|hw:1,0\n");
  exit(1);
}
