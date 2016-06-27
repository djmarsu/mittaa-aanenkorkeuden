#define _BSD_SOURCE
#include <stdio.h>
#include <stdint.h> 
#include <string.h>
#include <sys/time.h>

#include "../../config.h"
#include "../../pitchdetect.h"
#include "../../params.h"
#include "../../freqandpitch.h"

struct wavfile {
  char id[4];
  int totallength;
  char wavefmt[8];
  int format;
  short pcm;
  short channels;
  int frequency;
  int bytes_per_second;
  short bytes_by_capture;
  short bits_per_sample;
  char data[4];
  int bytes_in_data;
};

int main (int argc, char **argv) {
  FILE *fp;
  struct wavfile header;
  char *filename;

  float buf[buflen];
  int channels;

  int use_wav = -1;

  // no need for -f and -d switches
  if (argc != 2) {
    usage();
  }

  filename = argv[1];

  if ((use_wav = ends_with(".wav", filename)) == 0) {
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Not able to open input file %s.\n", filename);
        return 1;
    }

    // read header
    if (fread(&header, sizeof(header), 1, fp) < 1) {
        fprintf(stderr, "Can't read file header\n");
        return 1;
    }

    if (strncmp(header.id, "RIFF", 4) != 0 ||
        strncmp(header.data, "data", 4) != 0 ||
        strncmp(header.wavefmt, "WAVEfmt", 7) != 0) {
        fprintf(stderr, "file is not wav\n"); 
        return 1;
    }

    channels = header.channels;

    int frames = header.bytes_in_data / channels / 2;
    if (frames < buflen) {
      fprintf(stderr, "wav file was too short\n");
      return 1;
    }
  } else {
    usage();
  }

  int size = buflen * channels;
  int16_t buffer[size];

  int c = 0;
  long int time_in_usec = 0;
  while (1) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);

    if (use_wav == 0) {
      if ((fread(buffer, sizeof(buffer), 1, fp)) != 1) {
        break;
      }
    }
    for (int k = 0; k < buflen; k++) {
      float s = (float) buffer[k * channels] / INT16_MAX;
      buf[k] = s;
    }
 
    float *buf2 = lpf(buf);
    int freq = pitchdetect(buf2);

    printf("%d\n", freq);
    gettimeofday(&tval_after, NULL);

    timersub(&tval_after, &tval_before, &tval_result);
    float s = (float) buflen/ samplerate;
    printf("Time elapsed to read %d samples (%f seconds): %ld.%06ld\n", buflen, s, (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

    time_in_usec += 1000 * (long int)tval_result.tv_sec;
    time_in_usec += (long int)tval_result.tv_usec;
    c++;
  }

  double average_time = time_in_usec / c;
  printf("In average: %ld %d %lf\n", time_in_usec, c, average_time);

  fclose(fp);

  return 0;
}
