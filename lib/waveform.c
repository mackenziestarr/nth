#include <math.h>
#include "waveform.h"

void generate_sine(Waveform** wave,  float** out) {
    float amplitude = 0;
    if ((*wave)->play) {
        (*wave)->frames++;
        amplitude = (float)(*wave)->frames / (float)(*wave)->duration; 
        if ((*wave)->frames > (*wave)->duration) {
            (*wave)->frames = 0;
            (*wave)->play = 0;
        }
    }
    *(*out)++ = amplitude * sin((*wave)->left_phase);
    *(*out)++ = amplitude * sin((*wave)->right_phase);
    (*wave)->left_phase += (WAVE_CYCLE * (*wave)->frequency) / SAMPLE_RATE;
    (*wave)->right_phase += (WAVE_CYCLE * (*wave)->frequency) / SAMPLE_RATE;
    if ((*wave)->left_phase > WAVE_CYCLE) (*wave)->left_phase -= WAVE_CYCLE;
    if ((*wave)->right_phase > WAVE_CYCLE) (*wave)->right_phase -= WAVE_CYCLE;
} 





