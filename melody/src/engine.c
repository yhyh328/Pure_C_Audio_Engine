#include "engine.h"
#include <math.h>

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

void engine_init(Engine* e, float sampleRate)
{
    e->sampleRate = sampleRate;
    e->phase      = 0.0f;
    e->freqHz     = 440.0f;
    e->gain       = 0.2f;
    e->targetGain = 0.2f;

    // gain smoothing: smaller = smoother (but slower response)
    // 0.001 ~ 0.01 is recommended
    e->gainAlpha = 0.002f;
}

void engine_set_freq(Engine* e, float freqHz) {
    if (freqHz < 1.0f) freqHz = 1.0f;
    e->freqHz = freqHz;
}

void engine_set_gain(Engine* e, float gain01) {
    e->targetGain = clampf(gain01, 0.0f, 1.0f);
}

void engine_process_interleaved_stereo_f32(Engine* e,
                                           float* outLR,
                                           int nframes)
{
    const float sr       = e->sampleRate;
    // phase increment per sample (normalized phase: 0..1)
    const float phaseInc = e->freqHz / sr;     
    
    float phase          = e->phase;
    float gain           = e->gain;

    const float target   = e->targetGain;
    const float a        = e->gainAlpha;

    for (int i = 0; i < nframes; ++i)
    {
       
       // 1) smooth gain (prevent zipper noise)
       gain += (target - gain) * a;
       
       // 2) sine oscillator
       float s = sinf(2.0f * (float)M_PI * phase);
       
       // 3) apply gain + optional soft clip
       float y = softclip(s * gain);

       // interleaved stereo: LRLRLR..
       // Write the mono signal equally to left and right channels
       // to produce a centered stereo image
       outLR[2*i + 0] = y; // L
       outLR[2*i + 1] = y; // R

       // advance phase
       phase += phaseInc;
       if (phase >= 1.0f) phase -= 1.0f;

    }
    
    e->phase = phase;
    e->gain  = gain;
    
}

