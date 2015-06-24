#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <portaudio.h>
#include "lib/cli.h"
#include "lib/waveform.h"


static int run = 1;
void handle(int noop) {
    run = 0;
}

     
float* buildScale(int notes, int range) {
    float* frequencies = malloc(range * sizeof(float));
    double ratio = pow(2.0, 1.0 / notes);
    double base = 330.0;
    for (int i=0; i < range; i++) {  
        fprintf(stderr, "%f\n", base);
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

int audio(const void *input, void *output, unsigned long frames,
          const PaStreamCallbackTimeInfo* time,
          PaStreamCallbackFlags status,
          void *waveform) 
{
    Waveform *wave = (Waveform*)waveform;    
    float *out = (float*)output;
    for (int i=0; i<frames; i++) {
        generate_sine(&wave, &out);   
    } 
    return paContinue;
}

int main() {
    signal(SIGINT, handle);

    PaError error = paNoError;
    error = Pa_Initialize();
    if (error != paNoError) end(error);
   
    PaStream* stream = NULL;
    
    unsigned long duration = (unsigned long)(0.5 * SAMPLE_RATE); 
    Waveform waveform = {0, 1000.0, 0, 0, 0, duration};
    
    error = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, 1024, audio, &waveform);

    if (error != paNoError) end(error); 
    
    Pa_StartStream(stream);
    
    raw_tty(NB_ENABLE);
    
    int keycode = 0;
    char key;
    float* scale = buildScale(12, 12);
    while(run) {
        keycode = keypress();
        if (keycode != 0) {
            key = fgetc(stdin);
            waveform.frequency = getFrequency(key, scale);
            waveform.play = 1;
            fprintf(stderr, "you pressed %c\n", key);
        }
    };
    free(scale);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    end(error);
    raw_tty(NB_DISABLE);    
    return 0;
}








