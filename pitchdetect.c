#include <stdio.h>
#include <string.h>
#include <math.h>
#include "config.h"

// simple low pass filter algorithm
float *lpf(float *buf) {
  float *out = buf;
  //float out[buflen] = {0.0};
  // initialize array
  out[0] = buf[0];

  for (int i = 1; i < buflen; i++) {
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
  float secs = (float) buflen / samplerate;

  // check if there is enough signal
  float rms = 0;
  for (int j = 0; j < buflen; j++) {
    rms += buf[j] * buf[j];
  }
  rms = sqrt(rms / buflen);
  if (rms < 0.01)
    return -1;

  int c = 0;
  int prev = 0;
  
  for (int j = 0; j < buflen; j++) {
    // tapa 2
    int s = sign(buf[j]);
    if (s != prev)
      c++;
    prev = s;
  }

  float cyclecount = c / 2;

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