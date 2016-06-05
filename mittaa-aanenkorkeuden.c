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

  int c = 0;
  int edellinen = 0;
  for (int i = 1; i < BUFLEN; i++) {
    // tapa 2
    int merkki = sign(buf[i]);
    if (merkki != edellinen)
      c++;

    edellinen = merkki;

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

int prepare_soundcard(char *device) {
  int err;

  unsigned int rate = 44100;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

  if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf(stderr, "cannot open audio device %s (%s)\n", 
             device,
             snd_strerror (err));
    exit(1);
  }
 
  // allocate hw_params  
  if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
    fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit(1);
  }
        
  // initializes hw_params 
  if ((err = snd_pcm_hw_params_any(handle, hw_params)) < 0) {
    fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit(1);
  }

  // sets hw_params access
  if ((err = snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf(stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
    exit(1);
  }

  // sets hw_params format
  if ((err = snd_pcm_hw_params_set_format(handle, hw_params, format)) < 0) {
    fprintf(stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit(1);
  }

  // sets sample rate
  if ((err = snd_pcm_hw_params_set_rate_near(handle, hw_params, &rate, 0)) < 0) {
    fprintf(stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit(1);
  }
  
  // sets channels to stereo
  if ((err = snd_pcm_hw_params_set_channels(handle, hw_params, 2)) < 0) {
    fprintf(stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit(1);
  }

  // sets hw_params
  if ((err = snd_pcm_hw_params(handle, hw_params)) < 0) {
    fprintf(stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit(1);
  }

  // frees hw_params
  snd_pcm_hw_params_free(hw_params);
  
  // prepares audio interface
  if ((err = snd_pcm_prepare(handle)) < 0) {
    fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit(1);
  }

  return 0;
}

void usage() {
  fprintf(stderr, "usage: mittaa-aanenkorkeuden file.wav|hw:1,0\n");
  exit(1);
}

void int_handler(int dummy) {
    // is this the wrong way around
    snd_pcm_close(handle);                                                      
    running = 0;
}

int main (int argc, char **argv) {
  SNDFILE *file = NULL;
  SF_INFO sfinfo;
  char *filename;

  // gotta allocate twice as much for stereo
  size_t size = BUFLEN*2;
  // vai uint16_t  
  int16_t buffer[size];

  memset(buffer, 0, size * sizeof(buffer[0]));
  
  float flo[BUFLEN];

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
      printf("Not able to open input file %s.\n", filename);
      return 1;
    }
  } else if ((use_soundcard = check_if_device(filename)) == 0) {
    prepare_soundcard(filename);
  } else {
    usage();
  }

  int buffer_frames = BUFLEN; // frame = sekä vasen että oikee kanava
  int err;

  if (use_wav == 0) {//riittääkö if(use_wav){
    //  static void convert_to_text(SNDFILE *file) {
    float buf[BUFLEN];
    int count;

    // read file to float buffer
    // TODO: only works with mono files
    while ((count = sf_readf_float(file, buf, BUFLEN)) == BUFLEN) {
      // apply simple lowpass filtering to buf
      float *buf2 = lpf(buf);
      printf("%d\n", pitchdetect(buf2));
    }
    sf_close(file);
  } else if (use_soundcard == 0) {
    signal(SIGINT, int_handler);
    while (running) { 
      if ((err = snd_pcm_readi(handle, buffer, buffer_frames)) != buffer_frames) {
        fprintf(stderr, "read from audio interface failed (%s)\n",
                 snd_strerror (err));
        exit(1);
      }
  
      for(int k = 0; k < buffer_frames; k++) {
        // pick every other sample because it is stereo
        float s = (float)buffer[k * 2]/INT16_MAX;
        flo[k] = s;
      }
      float *buf2 = lpf(flo);
      printf("piste %d\n", pitchdetect(buf2)); 
    }
  }

  return 0;
}

