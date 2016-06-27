static unsigned int samplerate = 44100;

// 5 (8820/44100) guesses in one second
#define buflen 8820

// tulostaako nuotin nimen ja oktaavin esim. C 5
// muuten tulostaa vain taajuuden esim. 440 hz
static int print_pitch = 1;
