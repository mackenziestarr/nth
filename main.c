#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/select.h>
#include <portaudio.h>
#include <termios.h>
#include <unistd.h>
#include "waveform.h"

#define NB_ENABLE (0)
#define NB_DISABLE (1)

static int run = 1;
void handle(int noop) {
    run = 0;
}

int keypress () {
    fd_set buffer;
    struct timeval time;
    // poll
    time.tv_sec = 0;
    time.tv_usec = 0;
    FD_ZERO(&buffer);
    FD_SET(0, &buffer);
    select(1, &buffer, NULL, NULL, &time);
    return FD_ISSET(0, &buffer);
}

void raw_tty(int state) {
    struct termios ttystate;
    // get state
    tcgetattr(0, &ttystate);
    if (state == NB_ENABLE) {
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_lflag &= ~ECHO;
        ttystate.c_cc[VMIN] = 1;
    } 
    else if (state == NB_DISABLE) {
        ttystate.c_lflag |= ICANON;
        ttystate.c_lflag |= ECHO;
    }
    tcsetattr(0, TCSANOW, &ttystate);
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
    
    raw_tty(NB_ENABLE);
    
    int keycode = 0;
    char key;
    while(run) {
        keycode = keypress();
        if (keycode != 0) {
            key = fgetc(stdin);
            fprintf(stderr, "you pressed %c\n", key);
        }
    };
    
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    end(error);
    raw_tty(NB_DISABLE);    
    return 0;
}
