#pragma once

// ensure C linkage when this header is included in C++ code
#ifdef __cplusplus
extern "C" { 
#endif

#ifndef ENGINE_MAX_PARTIALS
#define ENGINE_MAX_PARTIALS 64
#endif

typedef enum { NORM_SUM = 0, NORM_RMS = 1 } NormMode;

typedef struct Engine {
    float sampleRate;

    // oscillator
    float freqHz;
    float gain;       // current gain
    float targetGain; // desired gain
    float gainAlpha;  // smoothing coefficient (0..1, small)

    // additive oscillator phases (0..1)
    int numPartials;                    // number of harmonic partials (>=1, <= ENGINE_MAX_PARTIALS)
    float phase[ENGINE_MAX_PARTIALS];   // per-partial phase (0..1)

    // HarmonicShaper-like state (no malloc, fixed arrays)
    float     harmAmp[ENGINE_MAX_PARTIALS];     // current (smoothed)
    float     harmTarget[ENGINE_MAX_PARTIALS];  // computed target
    float     harmMask[ENGINE_MAX_PARTIALS];    // user mask
    float     harmAlpha;                        // brightness (exp decay)
    float     harmSmooth;                       // spectral smoothing (0..1)
    NormMode  harmNormMode;                     // 0: SUM, 1: RMS
    int       harmDirty;                        // if 1 -> rebuild target
} Engine;

// init / setters
void engine_init(Engine* e, float sampleRate);
void engine_set_freq(Engine* e, float freqHz);
void engine_set_gain(Engine* e, float gain01);

// additive / harmonic controls
void engine_set_num_partials(Engine* e, int N);
void engine_set_brightness(Engine* e, float alpha);
void engine_set_norm_mode(Engine* e, int mode);
void engine_set_spectrum_odd_even(Engine* e, float oddGain, float evenGain);
void engine_set_mask_partial(Engine* e, int harmonicNumber, float gain);

// audio render
void engine_process_interleaved_stereo_f32(Engine* e, float* outLR, int nframes);

#ifdef __cplusplus
}
#endif


