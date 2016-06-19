#ifndef FREQANDPITCH_H
#define FREQANDPITCH_H

float frequency_to_pitch(int hz);
int midinumb(float p);
float get_cents_off(float p);
int get_octave(int p_rounded, float cents_off);
void pretty_print_pitch(int hz);

#endif
