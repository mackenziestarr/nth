#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <portaudio.h>
#include "waveform.h"

static int run = 1;
void handle(int noop) {
    run = 0;
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
        generate_sine(&wave, 1000, &out);   
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
    Waveform waveform = {0, 0, 0, duration};
    
    error = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, 1024, audio, &waveform);

    if (error != paNoError) end(error); 
    
    Pa_StartStream(stream);

    while(run);
    
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    end(error);
    
    return 0;
}
