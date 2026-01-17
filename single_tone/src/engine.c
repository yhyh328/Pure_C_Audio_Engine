#include "engine.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

// optional soft clip (cheap and safe)
static float softclip(float x)
{
    // tanh is OK here
    // if you want even cheaper: x/(1+fabs(x))
    return tanhf(x);
}

// Build harmonic target amplitudes
// base decay * user mask -> normalization
static void hs_build_target(Engine* e)
{
    // Call hs_build_target() only when parameters change

    const int N = e->numPartials; // number of harmonics
    if (N < 0) return;
    // --------------------------------------------------
    // 1) base amplitude * mask
    // --------------------------------------------------
    for (int i = 0; i < N; i++) {
        // Exponential decay for higher harmonics
        // i = 0 is the fundamental
        float base = expf(-e->harmAlpha * (float)i);
        // Mask controls whether this harmonic is used
        float m = e->harmMask[i];
        // For safety
        if (m < 0.0f) m = 0.0f;
        // Raw target amplitude for this harmonic
        e->harmTarget[i] = base * m;
    }
    // --------------------------------------------------
    // 2) Normalize to keep level stable
    // --------------------------------------------------
    if (e->harmNormMode == NORM_SUM) { // NORM_SUM
        // Sum of all harmonic amplitudes
        float sum = 0.0f;
        for (int i = 0; i < N; i++) { sum += e->harmTarget[i]; }
        // Normalize so total sum becomes 1
        if (sum > 1e-12f) { // 0.000000000001
            float inverse = 1.0f / sum;
            for (int i = 0; i < N; i++) { e->harmTarget[i] *= inverse; }
        }
    } else { // NORM_RMS
        // Sum of squared amplitudes (energy)
        float e2 = 0.0f;
        for (int i = 0; i < N; i++) { e2 += e->harmTarget[i] * e->harmTarget[i]; }
        // RMS (average perceived Level)
        float rms = sqrtf(e2 / (float)N);
        // Normalize so RMS becomes 1
        if (rms > 1e-12f) { // 0.000000000001
            float inverse = 1.0f / rms;
            for (int i = 0; i < N; i++) { e->harmTarget[i] *= inverse; }
        }
    }
    e->harmDirty = 0;
}

static void hs_smooth_step_block(Engine* e)
{
    // Call hs_smooth_step() at every audio block(e.g., every 64 or 128 samples)
    const int N = e->numPartials;
    float a = e->harmSmooth;  // 0..1
    float b = 1.0f - a;
    for (int i = 0; i < N; i++) { e->harmAmp[i] = a * e->harmAmp[i] + b * e->harmTarget[i]; }
}


void engine_init(Engine* e, float sampleRate)
{

    memset(e, 0, sizeof(*e));

    e->sampleRate = sampleRate;
    
    e->freqHz     = 440.0f;
    e->gain       = 0.2f;
    e->targetGain = 0.2f;

    // gain smoothing: smaller = smoother (but slower response)
    // 0.001 ~ 0.01 is recommended
    e->gainAlpha = 0.002f;

    // additive defaults
    e->numPartials = 32;
    if (e->numPartials > ENGINE_MAX_PARTIALS) e->numPartials = ENGINE_MAX_PARTIALS;

    // harmonic shaping defaults
    e->harmAlpha    = 0.15f;  // brightness control (smaller = brighter)
    e->harmSmooth   = 0.90f;  // spectral smoothing per block
    e->harmNormMode = NORM_SUM;

    // default mask: all ones (sawtooth-like if combined with exp decay)
    for (int i = 0; i < ENGINE_MAX_PARTIALS; i++) {
        e->harmMask[i]   = 1.0f;
        e->harmAmp[i]    = 0.0f;
        e->harmTarget[i] = 0.0f;
        e->phase[i]      = 0.0f;
    }
    
    e->harmDirty = 1;
    hs_build_target(e);
    // start amp at target to avoid fade-in
    for (int i = 0; i < e->numPartials; i++) e->harmAmp[i] = e->harmTarget[i];
}

void engine_set_freq(Engine* e, float freqHz) {
    if (freqHz < 1.0f) freqHz = 1.0f; 
    e->freqHz = freqHz;
}

void engine_set_gain(Engine* e, float gain01) {
    e->targetGain = clampf(gain01, 0.0f, 1.0f);
}

void engine_set_num_partials(Engine* e, int N)
{
    if (N < 1) N = 1;
    if (N > ENGINE_MAX_PARTIALS) N = ENGINE_MAX_PARTIALS;
    e->numPartials = N; e->harmDirty   = 1;
}

void engine_set_brightness(Engine* e, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    e->harmAlpha = alpha; e->harmDirty = 1;
}

void engine_set_norm_mode(Engine* e, int mode)
{
    e->harmNormMode = (mode == NORM_RMS) ? NORM_RMS : NORM_SUM;
    e->harmDirty = 1;
}

void engine_set_spectrum_odd_even(Engine* e, float oddGain, float evenGain)
{
    // sawtooth-like spectrum
    //      -> hs_mask_odd_even(h, 1, 1);
    // square-like spectrum
    //      -> hs_mask_odd_even(h, 1, 0);
    // perceived octave up
    // even harmonics only (perceived octave up: no fundamental, 2*f0 becomes dominant)
    //      -> hs_mask_odd_even(h, 0, 1);
    for (int i = 0; i < e->numPartials; i++) {
        int n = i + 1;
        float g  = (n % 2) ? oddGain : evenGain;
        if (g < 0.0f) g = 0.0f;
        e->harmMask[i] = g;
    }
    e->harmDirty = 1;
}

void engine_set_mask_partial(Engine* e, int harmonicNumber, float gain)
{
    int i = harmonicNumber - 1; // 1..N -> 0..N-1
    if (i < 0 || i >= e->numPartials) return;
    if (gain < 0.0f) gain = 0.0f;
    e->harmMask[i] = gain;
    e->harmDirty = 1;
}

void engine_process_interleaved_stereo_f32(Engine* e, float* outLR, int nframes)
{
    const float sr       = e->sampleRate;

    // rebuild target only when spectrum parameters changed
    if (e->harmDirty) hs_build_target(e);

    // smooth spectral amps once per block
    hs_smooth_step_block(e);

    float gain           = e->gain;
    const float target   = e->targetGain;
    const float a        = e->gainAlpha;

    for (int i = 0; i < nframes; ++i)
    {
       
       // 1) smooth gain (prevent zipper noise)
       gain += (target - gain) * a;
       
       // 2) sine oscillator: sum partials
       float s = 0.0f;
       for (int p = 0; p < e->numPartials; p++) {
            int n = p + 1;            
            float f = e->freqHz * (float)n;
            if (0.5f * sr <= f) break; // stop above Nyquist
            // phase increment per sample (normalized phase: 0..1)
            float phaseInc = (e->freqHz * (float)n) / sr;     
            float phase = e->phase[p];

            s += sinf(2.0f * (float)M_PI * phase) * e->harmAmp[p];

            phase += phaseInc;
            if (phase >= 1.0f) phase -= 1.0f;
            e->phase[p] = phase;
       }    
       
       // 3) apply gain + optional soft clip
       float y = softclip(s * gain);

       // interleaved stereo: LRLRLR..
       // Write the mono signal equally to left and right channels
       // to produce a centered stereo image
       outLR[2*i + 0] = y; // L
       outLR[2*i + 1] = y; // R

    }
    
    e->gain  = gain;
    
}

