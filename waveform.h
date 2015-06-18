#include <math.h>
#define SAMPLE_RATE (44100)
#define WAVE_CYCLE (M_PI * 2)

typedef struct Waveform {
    double left_phase;
    double right_phase;
    unsigned long frames;
    unsigned long duration;
} Waveform;

void generate_sine(Waveform** wave, float frequency, float** out);
