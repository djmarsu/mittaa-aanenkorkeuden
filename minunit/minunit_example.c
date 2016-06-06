#include "minunit.h"

#include "../config.h"
#include "../pitchdetect.h"

#define M_PI 3.14159265358979323846

void test_setup() {
}

void test_teardown() {
}

MU_TEST(test_low_pass_filter) {
    float buf[buflen];
    memset(buf, 0, buflen * sizeof(buf[0]));
    buf[0] = 0.4;
    buf[1] = 0.24;
    buf[2] = -1.11;
    buf[3] = 0.01;
    float *buf2 = lpf(buf);
    mu_assert_float_eq(0.64, buf2[1]);
    mu_assert_float_eq(-0.87, buf2[2]);
    mu_assert_float_eq(-1.10, buf2[3]);
}

MU_TEST(test_pitchdetect_with_sine_wave) {
    float buf[buflen];
    memset(buf, 0, buflen * sizeof(buf[0]));
    int hz = 440;
    for (int i = 0; i < buflen; i++) {
        buf[i] = sin(2 * M_PI * hz * i / samplerate);
    }
    int v = pitchdetect(buf);
    mu_assert_int_within(440, v, 10);

    hz = 20;
    for (int i = 0; i < buflen; i++) {
        buf[i] = sin(2 * M_PI * hz * i / samplerate);
    }
    v = pitchdetect(buf);

    mu_assert_int_within(20, v, 10);
 
    hz = 1700;
    for (int i = 0; i < buflen; i++) {
        buf[i] = sin(2 * M_PI * 1700 * i / samplerate);
    }
    v = pitchdetect(buf);

    mu_assert_int_within(1700, v, 10);
}

MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
  MU_RUN_TEST(test_low_pass_filter);
  MU_RUN_TEST(test_pitchdetect_with_sine_wave);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return 0;
}

