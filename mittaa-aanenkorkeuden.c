#include <stdio.h>
#include <string.h>
#include <sndfile.h>

#define	BUFLEN 1000

static void convert_to_text(SNDFILE *infile) {
  float buf[BUFLEN];
  int i, count;

  while ((count = sf_readf_float(infile, buf, BUFLEN)) > 0) {
    for (i = 0; i < count; i++) {
      printf("%f\n", buf[i]);
    }
  }
}

int main (int argc, char **argv) {
  char *infilename;
  SNDFILE	*infile = NULL;
  SF_INFO	sfinfo;

  if (argc != 2) {
    printf("usage: mittaa-aanenkorkeuden file.wav\n");
    return 1;
  }

  infilename = argv[1];
  memset(&sfinfo, 0, sizeof(sfinfo));

  if ((infile = sf_open(infilename, SFM_READ, &sfinfo)) == NULL) {
    printf ("Not able to open input file %s.\n", infilename);
    return 1;
  }

  convert_to_text(infile);
  sf_close(infile);

  return 0;
}

