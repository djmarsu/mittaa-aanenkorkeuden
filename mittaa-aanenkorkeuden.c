#include <stdio.h>
#include <sndfile.h>
#include <stdint.h> 
#include <signal.h>
#include <alsa/asoundlib.h>

#include "config.h"
#include "hw.h"
#include "pitchdetect.h"
#include "params.h"

snd_pcm_t *handle = NULL;
static volatile int running = 1;

// whatto do when press ^C
void int_handler(int dummy) {
    // is this the wrong way around
    snd_pcm_close(handle);                                                      
    running = 0;
}

int main (int argc, char **argv) {
  SNDFILE *file = NULL;
  SF_INFO sfinfo;
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

  if ((use_wav = check_if_wav(filename)) == 0) {
    memset(&sfinfo, 0, sizeof(sfinfo));

    if ((file = sf_open(filename, SFM_READ, &sfinfo)) == NULL) {
      fprintf(stderr, "Not able to open input file %s.\n", filename);
      return 1;
    }
  } else if ((use_soundcard = check_if_device(filename)) == 0) {
    prepare_soundcard(filename);
  } else {
    usage();
  }

  if (use_wav == 0) {
    // read file to float buffer
    size_t size = buflen * sfinfo.channels;
    float buffer[size];
    if (sfinfo.frames < buflen) {
      fprintf(stderr, "wav file was too short\n");
      return 1;
    }

    while ((count = sf_readf_float(file, buffer, buflen)) == buflen) {
      for(int k = 0; k < size; k++) {
        float s = buffer[k * sfinfo.channels];
        buf[k] = s;
      }
      // apply simple lowpass filtering to buf
      float *buf2 = lpf(buf);
      printf("%d\n", pitchdetect(buf2));
    }
    sf_close(file);
  } else if (use_soundcard == 0) {
    // gotta allocate twice as much for stereo
    // one frame contains both left and right channel
    size_t size = buflen * 2;
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
  }

  return 0;
}
