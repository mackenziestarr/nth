#include <math.h>
#include "waveform.h"

void generate_sine(Waveform** wave, float frequency, float** out) {
    *(*out)++ = sin((*wave)->left_phase);
    *(*out)++ = sin((*wave)->right_phase);
    (*wave)->left_phase += (WAVE_CYCLE * frequency) / SAMPLE_RATE;
    (*wave)->right_phase += (WAVE_CYCLE * frequency) / SAMPLE_RATE;
    if ((*wave)->left_phase > WAVE_CYCLE) (*wave)->left_phase -= WAVE_CYCLE;
    if ((*wave)->right_phase > WAVE_CYCLE) (*wave)->right_phase -= WAVE_CYCLE;
} 





