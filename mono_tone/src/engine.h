#pragma once

// ensure C linkage when this header is included in C++ code
#ifdef __cplusplus
extern "C" { 
#endif

typedef struct Engine {
    float sampleRate;

    // oscillator
    float phase;  // 0..1
    float freqHz;
    
    // gain smoothing
    float gain;       // current gain
    float targetGain; // desired gain
    float gainAlpha;  // smoothing coefficient (0..1, small)
} Engine;

void engine_init(Engine* e, float sampleRate);
void engine_set_freq(Engine* e, float freqHz);
void engine_set_gain(Engine* e, float gain01);
void engine_process_interleaved_stereo_f32(
    Engine* e,
    float* outLR,
    int nframes
);

#ifdef __cplusplus
}
#endif


