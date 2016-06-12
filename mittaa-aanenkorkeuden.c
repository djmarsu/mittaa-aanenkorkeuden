#include <stdio.h>
#include <stdint.h> 
#include <signal.h>
#include <alsa/asoundlib.h>

#include "config.h"
#include "hw.h"
#include "pitchdetect.h"
#include "params.h"

snd_pcm_t *handle = NULL;
static volatile int running = 1;

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

// whatto do when press ^C
void int_handler(int dummy) {
    // is this the wrong way around
    snd_pcm_close(handle);                                                      
    running = 0;
}

int main (int argc, char **argv) {
  FILE *fp;
  struct wavfile header;
  char *filename;

  float buf[buflen];
  int count;
  
  int use_wav = -1;
  int use_soundcard = -1;

  // no need for -f and -d switches
  if (argc != 2) {
    usage();
  }

  filename = argv[1];

  if ((use_wav = ends_with(".wav", filename)) == 0) {
    fp = fopen(filename,"rb");
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

    printf("header.format = %d\n", header.format);

    // read file to float buffer
    int size = buflen * header.channels;
    int16_t buffer[size];
    
    // onkohan tää oikein
    if (header.bytes_in_data < buflen) {
      fprintf(stderr, "wav file was too short\n");
      return 1;
    }

    int err;
    while (fread(buffer, sizeof(buffer), 1, fp)) {
      for(int k = 0; k < buflen; k++) {
        float s = buffer[k * header.channels] / 32767.0;
        buf[k] = s;
      }
      // apply simple lowpass filtering to buf
      float *buf2 = lpf(buf);
      printf("%d\n", pitchdetect(buf2));
    }
    fclose(fp);

  } else if ((use_soundcard = starts_with("hw:", filename)) == 0) {
    prepare_soundcard(filename);

    // gotta allocate twice as much for stereo
    // one frame contains both left and right channel
    int size = buflen * 2;
    int16_t buffer[size];
    memset(buffer, 0, size * sizeof(buffer[0]));
    
    // catch ctrl+c to quit program nicely
    signal(SIGINT, int_handler);
    
    while (running) { 
      buffer_from_soundcard(buffer, buflen);
      for(int k = 0; k < buflen; k++) {
        printf("k = %d\n", k);
        // pick every other sample because it is stereo
        float s = (float)buffer[k * 2] / INT16_MAX;
        buf[k] = s;
      }
      // apply lowpass filtering and print the result
      lpf_and_print(buf);
    }
  } else {
    usage();
  }

  return 0;
}
