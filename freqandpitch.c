#include <stdio.h>
#include <math.h>

static const char *nimet[12] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

float frequency_to_pitch(int hz) {
  float p = 69 + 12 * log(hz/440.0) / log(2.0);

  return p;
}

int midinumb(float p) {
  int p_rounded = (int) round(p);
  int midinumber = (p_rounded - 21) % 12;

  return midinumber;
}

float get_cents_off(float p) {
  float p_rounded = round(p);
  float cents_off = p - p_rounded; 
  cents_off *= 100;

  return cents_off;
}

int get_octave(int p_rounded, float cents_off) {
  int cents_off_rounded = (int) round(cents_off);
  int octave = (int) floor((p_rounded) / 12.0) - 1;

  return octave;
}

void pretty_print_pitch(int hz) {
  float p = frequency_to_pitch(hz);

  int midinumber = midinumb(p);
  
  float cents_off = get_cents_off(p);

  int p_rounded = (int) round(p);
  int octave = get_octave(p_rounded, cents_off);
  printf("%-3s %d   ", nimet[midinumber], octave);

  int cents_off_rounded = (int) round(cents_off);
  int cents_off_rounded_without_sign = abs(cents_off_rounded);

  if (cents_off > 0)
    printf("+");
  else if (cents_off < 0)
    printf("-");
  else
    printf(" ");
  printf("%3d c", cents_off_rounded_without_sign);
    
  printf("%7d hz\n", (int) hz);
}
