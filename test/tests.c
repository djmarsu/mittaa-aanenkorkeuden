#include "minunit.h"

#include "../config.h"
#include "../pitchdetect.h"
#include "../params.h"

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

MU_TEST(test_count_zerocrossings_works) {
    float a[buflen] = {0.0};
    a[0] = -0.31;
    a[1] = 1.3;
    a[2] = -0.32;
    a[3] = 1.11;
    mu_assert_int_eq(5, count_zerocrossings(a));
}

MU_TEST(test_sign_works) {
    mu_assert_int_eq(1, sign(0.1));
    mu_assert_int_eq(0, sign(0.0));
    mu_assert_int_eq(-1, sign(-0.3));
}

MU_TEST(test_ends_with_works) {
    mu_assert_int_eq(0, ends_with(".wav", "mkrlm.wav"));
    mu_assert_int_eq(-1, ends_with(".wav", "mekmk.mp3"));
}

MU_TEST(test_starts_with_works) {
    mu_assert_int_eq(0, starts_with("hw:", "hw:0,1"));
    mu_assert_int_eq(-1, starts_with("hw:", "jep"));
}

MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
  MU_RUN_TEST(test_low_pass_filter);
  MU_RUN_TEST(test_pitchdetect_with_sine_wave);
  MU_RUN_TEST(test_count_zerocrossings_works);
  MU_RUN_TEST(test_sign_works);
  MU_RUN_TEST(test_ends_with_works);
  MU_RUN_TEST(test_starts_with_works);

}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return 0;
}

