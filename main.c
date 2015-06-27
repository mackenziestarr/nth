#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>
#include <portaudio.h>
#include "lib/cli.h"
#include "lib/waveform.h"

#define SAMPLE_RATE (44100)
#define WAVE_CYCLE (M_PI * 2)

typedef struct ADSR {
    bool trigger;
    bool retrigger;
    unsigned long duration;
    unsigned long progress;
    float amplitude;
} ADSR;

typedef struct Wave {
    float frequency;
    double phase;
} Wave;

typedef struct paData {
    ADSR adsr;
    Wave wave;
} paData;

static int poll = 1;
void handle(int noop) {
    poll = 0;
}

float* buildScale(int notes, int range) {
    float* frequencies = malloc(range * sizeof(float));
    double ratio = pow(2.0, 1.0 / notes);
    double base = 330.0;
    for (int i=0; i < range; i++) {  
        frequencies[i] = base;
        base *= ratio;
    }
    return (float*) frequencies;  
}

float getFrequency(char key, float* scale) {
    char* keys = "asdfghjkl;'";
    int size = sizeof(keys) - 1;
    for (int i=0; i < size; i++) {
        if (*keys++ == key) return *(scale + i);
    }
    return *scale;
}
    
void end(PaError error) {
    Pa_Terminate();
    if (error < 0) fprintf(stderr, "portaudio error: %s", Pa_GetErrorText(error));
}

float linearEase(ADSR* adsr, float ms) {
    int edge = ms * SAMPLE_RATE;   
    float peak = 0.2;
    if (adsr->progress < edge) {
        return peak * (adsr->progress / (float)edge);
    }
    else if ((adsr->duration - adsr->progress) < edge) {
        return peak * ((adsr->duration - adsr->progress)/(float)edge);            
    }
    else return peak;
}

int audio(const void *input, void *output, unsigned long frames,
          const PaStreamCallbackTimeInfo* time,
          PaStreamCallbackFlags status,
          void *_data) 
{
    paData *data = (paData*) _data;    
    float *out = (float*)output;
    for (int i=0; i<frames; i++) {
        if (data->adsr.trigger) {
            *(out)++ = data->adsr.amplitude * sin(data->wave.phase);
            data->wave.phase += (WAVE_CYCLE * data->wave.frequency) / SAMPLE_RATE;
            if (data->wave.phase > WAVE_CYCLE) data->wave.phase -= WAVE_CYCLE;
            data->adsr.progress++;
            if (data->adsr.progress == data->adsr.duration) {
                data->adsr.progress = 0;
                data->adsr.trigger = false;
            }        
            data->adsr.amplitude = linearEase(&data->adsr, 0.1);
        } else {
            *(out)++ = 0;
        }
    }
  
    return paContinue;
}


int main() {
    signal(SIGINT, handle);

    PaError error = paNoError;
    error = Pa_Initialize();
    if (error != paNoError) end(error);
   
    PaStream* stream = NULL;
    
    paData data = {
        .adsr.trigger = false,
        .adsr.retrigger = false,
        .adsr.progress = 0,
        .adsr.duration = (unsigned long) (0.3 * SAMPLE_RATE),
        .adsr.amplitude = 0.0,
        .wave.phase = 0,
        .wave.frequency = 1000.0
    };
            
    error = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 1024, audio, &data);

    if (error != paNoError) end(error); 
    
    Pa_StartStream(stream);
    
    raw_tty(NB_ENABLE);
    
    int keycode = 0;
    char key;
    float* scale = buildScale(12, 12);
    while(poll) {
        keycode = keypress();
        if (keycode != 0) {
            key = fgetc(stdin);
            data.wave.frequency = getFrequency(key, scale);
            if (data.adsr.trigger) data.adsr.retrigger = true;
            data.adsr.trigger = true;
            fprintf(stderr, "key: %c\r", key);
        }
    };
    free(scale);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    end(error);
    raw_tty(NB_DISABLE);    
    return 0;
}








