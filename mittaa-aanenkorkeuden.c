#include <stdio.h>
#include <stdint.h> 
#include <signal.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include "config.h"
#include "hw.h"
#include "pitchdetect.h"
#include "params.h"
#include "freqandpitch.h"

snd_pcm_t *handle = NULL;
static volatile int running = 1;

static volatile int use_wav = -1;
static volatile int use_soundcard = -1;

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

// käsittelee keskeytyksen
// whatto do when press ^C
void int_handler(int dummy) {
    if (use_soundcard == 0) {
      snd_pcm_close(handle);
    }

    // ei tarvitse erikseen fclosea käyttää jos use_soundcard = 0,
    // sillä fclose kutsutaan mainin lopussa joka tapauksessa, riittää 
    // riittää siis että asetetaan running:n arvoksi 0

    running = 0;
}

int main (int argc, char **argv) {
  FILE *fp;
  struct wavfile header;
  char *filename;

  float buf[buflen];
  int channels;

  // no need for commandline -f and -d switches (file/device)
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

    // tarkitaa että wav-tiedosto varmasti on wav
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
  } else if ((use_soundcard = starts_with("hw:", filename)) == 0) {
    prepare_soundcard(filename);
    channels = 2; // pakko olla mun äänikortilla
  } else {
    usage();
  }

  // gotta allocate twice as much for stereo
  // one frame contains both left and right channel
  int size = buflen * channels;
  int16_t buffer[size];

  // catch ctrl+c to quit program nicely
  signal(SIGINT, int_handler);
            
  while (running) {
    if (use_soundcard == 0) {
      // read from alsa device to float buffer
      buffer_from_soundcard(buffer, buflen);
    } else if (use_wav == 0) {
      // read file to float buffer
      if ((fread(buffer, sizeof(buffer), 1, fp)) != 1) {
        break;
      }
    }

    // muuttaa taulukon arvot välille [-1, 1] jotta niille voidaan tehdä 
    // signaalinkäsittelyjuttuja
    for (int k = 0; k < buflen; k++) {
      float s = (float) buffer[k * channels] / INT16_MAX;
      buf[k] = s;
    }
 
    // apply simple lowpass filtering to buf
    float *buf2 = lpf(buf);
    int freq = pitchdetect(buf2);

    if (print_pitch == 1) {
      pretty_print_pitch(freq);
    } else {
      printf("%d\n", freq);
    }
  }

  fclose(fp);

  return 0;
}
