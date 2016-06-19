#include <stdio.h>
#include <string.h>
#include <math.h>
#include "config.h"

// simple low pass filter algorithm
float *lpf(float *buf) {
  static float out[buflen];
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

int count_zerocrossings(float *buf) {
  int count = 0;
  int prev = 0;
  for (int j = 0; j < buflen; j++) {
    int s = sign(buf[j]);
    if (s != prev)
      count++;
    prev = s;
  }
  return count;
}

float rms_volume(float *buf) {
  float rms = 0;
  for (int j = 0; j < buflen; j++) {
    rms += buf[j] * buf[j];
  }
  rms = sqrt(rms / buflen);
  return rms;
}

// zero-crossing algorithm
int pitchdetect(float *buf) {
  // buffer's length in seconds
  float secs = (float) buflen / samplerate;

  // check if there is enough signal
  float rms = rms_volume(buf);
  if (rms < 0.01)
    return -1;

  int count = count_zerocrossings(buf); 
  
  float cyclecount = count / 2;

  return round(cyclecount / secs);
}

/*
void lpf_and_print(float *buf) {
  float *buf2 = lpf(buf);

  int hz = pitchdetect(buf);
  if (hz > 0) {
    printf("\r%d", hz);
  } else {
    printf("\r..........");
  }
  fflush(stdout);
}
*/
