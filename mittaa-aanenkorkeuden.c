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

int sign(float x) {
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

int pitchdetect(float *buf) {
  // count of zero crossings
  int count = 0;

  // buffer's length in seconds
  float secs = (float) BUFLEN / SAMPLERATE;

  // check if there is enough signal
  float rms = 0;
  for (int j = 0; j < BUFLEN; j++) {
    rms += buf[j] * buf[j];
  }
  rms = sqrt(rms / BUFLEN);
  if (rms < 0.01)
    return -1;

  int c = 0;
  int edellinen = 0;
  for (int i = 1; i < BUFLEN; i++) {
    // tapa 2
    int merkki = sign(buf[i]);
    if (merkki != edellinen)
      c++;

    edellinen = merkki;

    // tapa 1
    if (buf[i] * buf[i - 1] < 0){
      count++;
    }
  }

  //printf("%f --\n", c/2.0/secs);
  //printf("%f --\n", count/2.0/secs);
  
  // how many cycles
  float cyclecount = count / 2;

  return round(cyclecount / secs);
}

static void convert_to_text(SNDFILE *file) {
  float buf[BUFLEN];
  int count;

  // read file to float buffer
  // TODO: only works with mono files
  while ((count = sf_readf_float(file, buf, BUFLEN)) == BUFLEN) {
    // apply simple lowpass filtering to buf
    float *buf2 = lpf(buf);
    printf("%d\n", pitchdetect(buf2));
  }
}

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

int main (int argc, char **argv) {
  char *filename;
  SNDFILE *file = NULL;
  SF_INFO sfinfo;

  // no need for -f and -d switches
  if (argc != 2) {
    printf("usage: mittaa-aanenkorkeuden file.wav|hw:1,0\n");
    return 1;
  }

  filename = argv[1];
  printf("+ %d\n", check_if_wav(filename));
  printf("+ %d\n", check_if_device(filename));

/*
  if (argc != 2) {
    printf("usage: mittaa-aanenkorkeuden -f file.wav\n"
           "   or: mittaa-aanenkorkeuden -d hw:1,0\n");
    return 1;
  }
*/
  memset(&sfinfo, 0, sizeof(sfinfo));

  if ((file = sf_open(filename, SFM_READ, &sfinfo)) == NULL) {
    printf("Not able to open input file %s.\n", filename);
    return 1;
  }

  convert_to_text(file);
  sf_close(file);

  return 0;
}

