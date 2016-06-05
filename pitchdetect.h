#ifndef PITCHDETECT_H
#define PITCHDETECT_H

float *lpf(float *buf);
int sign(float x);
int pitchdetect(float *buf);

#endif
