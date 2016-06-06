#include <stdio.h>
#include <math.h>
#include "minunit.h"
#include "../config.h"
#include "../pitchdetect.h"

#define M_PI 3.14159265358979323846

typedef int bool;
enum { false, true };

int tests_run = 0;

bool int_within(int a, int delta) {
    return a >= (a - delta) && a <= (a + delta);
}

bool equal_float(float a, float b, float epsilon) {
    bool ret = fabs(a - b) < epsilon;
    if (!ret) {
        printf("%f != %f\n", a, b);
    }
    return ret;
}

static char *test_lpf() {
    float buf[buflen];
    memset(buf, 0, buflen * sizeof(buf[0]));
    buf[0] = 0.4;
    buf[1] = 0.24;
    buf[2] = -1.11;
    buf[3] = 0.01;
    float *buf2 = lpf(buf);
    mu_assert("lowpass filtteri ei toimi #1", equal_float(buf2[1], 0.64, 0.0000005));
    mu_assert("lowpass filtteri ei toimi #2", equal_float(buf2[2], -0.87, 0.0000005));
    mu_assert("lowpass filtteri ei toimi #3", equal_float(buf2[3], -1.10, 0.0000005));
    return 0;
}

static char *test_pitchdetect_with_sine() {
    float buf[buflen];
    memset(buf, 0, buflen * sizeof(buf[0]));
    int hz = 440;
    for (int i = 0; i < buflen; i++) {
        buf[i] = sin(2 * M_PI * hz * i / samplerate);
    }
    int v = pitchdetect(buf);
    mu_assert("ei tunnistanut oikein", int_within(440, 10));

    hz = 20;
    for (int i = 0; i < buflen; i++) {
        buf[i] = sin(2 * M_PI * hz * i / samplerate);
    }
    v = pitchdetect(buf);
    mu_assert("ei tunnistanut oikein #2", int_within(20, 10));
 
    hz = 1700;
    for (int i = 0; i < buflen; i++) {
        buf[i] = sin(2 * M_PI * 1700 * i / samplerate);
    }
    v = pitchdetect(buf);
    mu_assert("ei tunnistanut oikein #3", int_within(1700, 10));

    return 0;
}

static char *all_tests() {
    mu_run_test(test_lpf);
    mu_run_test(test_pitchdetect_with_sine);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
