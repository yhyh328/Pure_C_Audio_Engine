#include "engine.h"
#include "audio_backend_pa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void usage() {
    printf("====example====\n");
    printf("    ./wave_generator [ sine | square | triangle | sawtooth ]\n");
    printf("===============\n");
}

static void result(int seconds, float freq, float gain, const Engine* e) {
    printf("seconds=%d, freq=%.2fHz, gain=%.3f, partials=%d, alpha=%3f\n", 
            seconds, freq, gain, e->numPartials, e->harmAlpha);
}

int main(int argc, char** argv)
{
    
    int seconds = 5; float freq = 440.0f; float gain = 0.2f;
    
    if (1 == argc)  {
        fprintf(stderr, "need to seet mode\n");
        usage();
        return 1;
    }

    char* mode = argv[1];
    if (strcmp(mode, "sine")     != 0  &&
        strcmp(mode, "square")   != 0  &&
        strcmp(mode, "sawtooth") != 0) 
    {
        fprintf(stderr, "unvalid mode\n");
        usage();
        return 1;
    }
 
    if (3 <= argc) seconds = atoi(argv[2]);
    if (4 <= argc) freq = (float)atof(argv[3]);
    if (5 <= argc) gain = (float)atof(argv[4]);
    
    Engine e;

    // The backend opens the audio stream at 49 kHz,
    // then the sample rate is synchronized in engine_init()
    engine_init(&e, 48000.0f);
    engine_set_freq(&e, freq);
    engine_set_gain(&e, gain);
    
   

    // sine wave
    engine_set_num_partials(&e, 1); // only fundamental
    engine_set_mask_partial(&e, 1, 1.0f); // fundamental amplitude = 1

    if (strcmp(mode, "sine") == 0) { 
        result(seconds, freq, gain, &e);
        return audio_backend_run(&e, seconds);
    }
   
    // composite wave - to concrete square, triangle and saw-tooth wave
    engine_set_num_partials(&e, 32);  // make 31 harmonics (root + 31 = 32)
    engine_set_brightness(&e, 0.15f); // 
   

    engine_set_norm_mode(&e, NORM_SUM);

    // choose spectrum
    if (strcmp(mode, "square") == 0)   { engine_set_spectrum_odd_even(&e, 1.0f, 0.0f); }
    if (strcmp(mode, "sawtooth") == 0) { engine_set_spectrum_odd_even(&e, 1.0f, 1.0f); }


    // unique spectrum
    // engine_set_spectrum_odd_even(&e, 0.0f, 1.0f); // even-only (perceived octave up)

    // TODO: 실제로 hs.amp[]를 사용해서 additive로 샘플을 만들어야 소리가 바뀜.
    // (지금 Engine은 단일 tone이라 hs가 반영되지 않음)    

    result(seconds, freq, gain, &e);
    return audio_backend_run(&e, seconds);
}

