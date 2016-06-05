#include <stdio.h>
#include <string.h>
#include <sndfile.h>
#include <stdint.h> 
#include <signal.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define SAMPLERATE 44100
// 5 guesses in one second
#define	BUFLEN 8820

snd_pcm_t *handle = NULL;
static volatile int running = 1;

// simple low pass filter algorithm
float *lpf(float *buf) {
  static float out[BUFLEN];
  // initialize array
  out[0] = buf[0];

  for (int i = 1; i < BUFLEN; i++) {
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
  float secs = (float) BUFLEN / SAMPLERATE;

  // check if there is enough signal
  float rms = 0;
  for (int j = 0; j < BUFLEN; j++) {
    rms += buf[j] * buf[j];
  }
  rms = sqrt(rms / BUFLEN);
  if (rms < 0.01)
    return -1;
/*
  int c = 0;
  int prev = 0;
  
  for (int j = 0; j < BUFLEN; j++) {
    // tapa 2
    int s = sign(buf[j]);
    if (s != prev)
      c++;
    prev = s;
  }
*/

  for (int i = 1; i < BUFLEN; i++) {
    // tapa 1
    if (buf[i] * buf[i - 1] < 0){
      count++;
    }
  }

  //printf("%f --\n", c/2.0/secs);
  //printf("%f --\n", count/2.0/secs);
  
  // how many cycles
  float cyclecount = count / 2;

  return round(cyclecount / secs);
}

int check_if_wav(char *str) {
  int len = strlen(str);

  // got to have at least 5 characters, eg. j.wav
  if (len < 5)
    return -1;

  const char *last_four = &str[len - 4];
  char *whattolookfor = ".wav";

  if (strncmp(last_four, whattolookfor, 4) != 0)
    return -1;

  return 0;
}

int check_if_device(char *str) {
  int len = strlen(str);

  // got to have at least 4 characters, eg. hw:1
  if (len < 4)
    return -1;

  char *whattolookfor = "hw:";

  if (strncmp(str, whattolookfor, 3) != 0)
    return -1;

  return 0;
}

void usage() {
  fprintf(stderr, "usage: mittaa-aanenkorkeuden file.wav|hw:1,0\n");
  exit(1);
}

// whatto do when ^C
void int_handler(int dummy) {
    // is this the wrong way around
    snd_pcm_close(handle);                                                      
    running = 0;
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

int main (int argc, char **argv) {
  SNDFILE *file = NULL;
  SF_INFO sfinfo;
  char *filename;

  float buf[BUFLEN];
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

  int buffer_frames = BUFLEN; // frame = sekä vasen että oikee kanava

  if (use_wav == 0) {
    // read file to float buffer
    // TODO: only works with mono files
    while ((count = sf_readf_float(file, buf, BUFLEN)) == BUFLEN) {
      // apply simple lowpass filtering to buf
      float *buf2 = lpf(buf);
      printf("%d\n", pitchdetect(buf2));
    }
    sf_close(file);
  } else if (use_soundcard == 0) {
    // catch ctrl+c to quit program nicely
    signal(SIGINT, int_handler);

    // gotta allocate twice as much for stereo
    size_t size = BUFLEN * 2;
    // vai uint16_t  
    int16_t buffer[size];
    memset(buffer, 0, size * sizeof(buffer[0]));

    while (running) { 
      buffer_from_soundcard(&buffer, buffer_frames);
      for(int k = 0; k < buffer_frames; k++) {
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

