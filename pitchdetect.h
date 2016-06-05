#ifndef PITCHDETECT_H
#define PITCHDETECT_H

float *lpf(float *buf);
int sign(float x);
int pitchdetect(float *buf);
void lpf_and_print(float *buf);

#endif
