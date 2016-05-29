#include <stdio.h>
#include <string.h>
#include <sndfile.h>
#include <math.h>

#define SAMPLERATE 44100
// 5 guesses in one second
#define	BUFLEN 8820

// simple low pass filter algorithm
float *lpf(float *buf) {
  static float out[BUFLEN];
  // initialize array
  out[0] = buf[0];

  for (int i = 1; i < BUFLEN; i++) {
    out[i] = buf[i] + buf[i - 1];
  }

  return out;
}

int pitchdetect(float *buf) {
  // count of zero crossings
  int count = 0;

  // buffer's length in seconds
  float secs = (float) BUFLEN / SAMPLERATE;

  for (int i = 1; i < BUFLEN; i++) {
    if (buf[i] * buf[i - 1] < 0){
      count++;
    }
  }

  // how many cycles
  float cyclecount = count / 2;

  return round(cyclecount / secs);
}

static void convert_to_text(SNDFILE *file) {
  float buf[BUFLEN];
  int count;

  // read file to float buffer
  // TODO: only works with mono files
  while ((count = sf_readf_float(file, buf, BUFLEN)) > 0) {
    // make sure there is enough data to detect pitch from
    if (count == BUFLEN) {
      // apply simple lowpass filtering to buf
      float *buf2 = lpf(buf);
      printf("%d\n", pitchdetect(buf2));
    }
  }
}

int main (int argc, char **argv) {
  char *filename;
  SNDFILE *file = NULL;
  SF_INFO sfinfo;

  if (argc != 2) {
    printf("usage: mittaa-aanenkorkeuden file.wav\n");
    return 1;
  }

  filename = argv[1];
  memset(&sfinfo, 0, sizeof(sfinfo));

  if ((file = sf_open(filename, SFM_READ, &sfinfo)) == NULL) {
    printf("Not able to open input file %s.\n", filename);
    return 1;
  }

  convert_to_text(file);
  sf_close(file);

  return 0;
}

