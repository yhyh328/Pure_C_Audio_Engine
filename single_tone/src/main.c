#include "engine.h"
#include "audio_backend_pa.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int seconds = 5;
    float freq = 440.0f;
    float gain = 0.2f;

    if (2 <= argc) seconds = atoi(argv[1]);
    if (3 <= argc) freq = (float)atof(argv[2]);
    if (4 <= argc) gain = (float)atof(argv[3]);
    
    Engine e;

    // The backend opens the audio stream at 49 kHz,
    // then the sample rate is synchronized in engine_init()
    engine_init(&e, 48000.0f);
    engine_set_freq(&e, freq);
    engine_set_gain(&e, gain);

    printf("seconds=%d, freq=%.2fHz, gain=%.3f\n",
           seconds, freq, gain);

    return audio_backend_run(&e, seconds);

}

