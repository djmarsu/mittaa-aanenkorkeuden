#include <stdio.h>
#include <string.h>
#include <stdint.h> 
#include <signal.h>
#include <math.h>
#include <alsa/asoundlib.h>

#include "config.h"

extern snd_pcm_t *handle;

// määrittelee äänikortille parametrit mitä käytetään
void prepare_soundcard(char *device) {
  int err;

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
  if ((err = snd_pcm_hw_params_set_rate_near(handle, hw_params, &samplerate, 0)) < 0) {
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
}

// hakee äänikortilta buffer_frames kokoisen määrän bufferia ja tallentaa sen
// parametrina välitettyyn taulukkoon buffer
void buffer_from_soundcard(int16_t *buffer, int buffer_frames) {
  int err;
  if ((err = snd_pcm_readi(handle, buffer, buffer_frames)) != buffer_frames) {
    fprintf(stderr, "read from audio interface failed (%s)\n",
             snd_strerror (err));
    exit(1);
  }
}
