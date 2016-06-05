#include <stdio.h>
#include <math.h>
#include "config.h"

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
/*
  int c = 0;
  int prev = 0;
  
  for (int j = 0; j < BUFLEN; j++) {
    // tapa 2
    int s = sign(buf[j]);
    if (s != prev)
      c++;
    prev = s;
  }
*/

  for (int i = 1; i < BUFLEN; i++) {
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

void lpf_and_print(float *buf) {
  float *buf2 = lpf(buf);
  
  int hz = pitchdetect(buf2);
  if (hz > 0) {
    printf("\r%d", hz);
  } else {
    printf("\r..........");
  }
  fflush(stdout);
}
