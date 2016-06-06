#include <alsa/asoundlib.h>

static unsigned int samplerate = 44100;

// 5 guesses in one second
#define buflen 8820
