#include <stdio.h>
#include <portaudio.h>
#include "waveform.h"

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
    PaError error = paNoError;
    error = Pa_Initialize();
    if (error != paNoError) end(error);
   
    PaStream* stream = NULL;
    Waveform waveform = {0, 0};
    error = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, 1024, audio, &waveform);

    if (error != paNoError) end(error); 
    
    Pa_StartStream(stream);

    Pa_Sleep(5 * 1000);
    
    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    end(error);
    return 0;
}
