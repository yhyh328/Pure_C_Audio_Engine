#include "engine.h"
#include "audio_backend_pa.h"
#include "note.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    // int seconds = 1;
    int dur_ms = 200;
    // float freq = 440.0f;
    float gain = 1.0f;

    if (2 <= argc) dur_ms = atoi(argv[1]);
    if (3 <= argc) gain   = (float)atof(argv[2]);
    
    /*
    Engine e;

    // The backend opens the audio stream at 49 kHz,
    // then the sample rate is synchronized in engine_init()
    engine_init(&e, 48000.0f);
    engine_set_freq(&e, freq);
    engine_set_gain(&e, gain);
    */

    Engine e;
    engine_init(&e, 48000.0f);

    // Start stream ONCE
    AudioBackendPa* backend = NULL;
    if (audio_backend_start(&backend, &e) != 0)
    {
        fprintf(stderr, "Failed to start audio backend.\n");
        return 1;
    }

    float tones[] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, 
                      NOTE_REST, NOTE_G4, NOTE_REST, NOTE_A4, 
                      NOTE_REST, NOTE_REST, NOTE_B4, NOTE_C5 };
    int numTones = sizeof(tones) / sizeof(float);

    for (int i = 0; i < numTones; ++i) {
        
        if (tones[i] == NOTE_REST) 
        { 
            engine_set_gain(&e, 0.0f);
            printf("dur_ms=%d, REST_NOTE\n", dur_ms);
        } 

        else 
        {
            float tone = midi_to_hz(tones[i]);
            engine_set_freq(&e, tone);
            engine_set_gain(&e, gain);
            printf("dur_ms=%d, tone=%.2fHz, gain=%.3f\n", dur_ms, tone, gain);
        }

        // Keep playing this tone for dur_ms (sub-second OK)
        audio_backend_sleep_ms(dur_ms);
    }

    // Stop stream ONCE
    audio_backend_stop(backend);
    return 0;   
}

