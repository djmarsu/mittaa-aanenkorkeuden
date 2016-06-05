#ifndef HW_H
#define HW_H

void prepare_soundcard(char *device);
void buffer_from_soundcard(int16_t *buffer, int buffer_frames);

#endif
