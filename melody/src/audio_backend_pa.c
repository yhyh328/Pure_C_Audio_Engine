#include "audio_backend_pa.h"
#include "engine.h"

#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Engine* engine;
    // long long framesRemaining;
    int channels;
} PaUserData;

// new structure for melody pjt
struct AudioBackendPa {
    PaStream* stream;
    PaUserData ud;
    int pa_initialized;
};

static int pa_callback(const void* input,
                       void* output,
                       unsigned long frameCount,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void* userData)
{
    (void)input;
    (void)timeInfo;
    (void)statusFlags;

    PaUserData* ud = (PaUserData*)userData;
    float* out = (float*)output;

   
    /* We don't need to set remaining frames and frames to render directly
    if (ud->framesRemaining <= 0) {
        // Fill silence and stop
        memset(out, 0, sizeof(float) * frameCount * (unsigned long)ud->channels);
        return paComplete;
    }

    unsigned long framesToRender = frameCount;
    if ((long long)framesToRender > ud->framesRemaining) {
        framesToRender = (unsigned long)ud->framesRemaining;
    }
    */


    // Render audio
    // This project generates interleaved stereo f32 (LRLR...)
    // If channels != 2, we still write something reasonable.
    
    if (ud->channels == 2) {
        engine_process_interleaved_stereo_f32(ud->engine, out, (int)frameCount);
    } 

    else if (ud->channels == 1) {
        // Render stereo to temp then downmix -> mono (simple & safe)
        float* tmp = (float*)alloca(sizeof(float) * frameCount * 2);
        engine_process_interleaved_stereo_f32(ud->engine, tmp, (int)frameCount);
        for (unsigned long i = 0; i < frameCount; ++i) {
            out[i] = 0.5f * (tmp[2*i] + tmp[2*i + 1]);
        }
    } 

    else {
        // Fallback: write silence for unsupported channel counts
        memset(out, 0, sizeof(float) * frameCount * (unsigned long)ud->channels);
    }

    // Continuous stream: we do NOT stop automatically here.
    // main thread will stop stream excplicitly.
    return paContinue;
    
    /* We don't need to set frames to render directly
    
    // If we rendered fewer frames than requested, pad the rest with silence
    
    if (framesToRender < frameCount) {
        memset(out + framesToRender * (unsigned long)ud->channels,
               0,
               sizeof(float) * (frameCount - framesToRender) * (unsigned long)ud->channels);
        ud->framesRemaining = 0;
        return paComplete;
    }

    ud->framesRemaining -= (long long)framesToRender;
    return (ud->framesRemaining <= 0) ? paComplete : paContinue;
    */
}

static int pa_print_error(PaError err, const char* where)
{
    if (err != paNoError) {
        fprintf(stderr, "[PortAudio] %s: %s\n", where, Pa_GetErrorText(err));
        return 1;
    }
    return 0;
}

int audio_backend_start(AudioBackendPa** out_backend, Engine* engine)
{
    if (!out_backend || !engine) return 1;

    *out_backend = NULL;

    AudioBackendPa* b = (AudioBackendPa*)calloc(1, sizeof(AudioBackendPa));
    if (!b) return 1;

    b->ud.engine   = engine;
    b->ud.channels = 2;

    PaError err = Pa_Initialize();
    if (pa_print_error(err, "Pa_Initialize")) { free(b); return 1; }
    b->pa_initialized = 1;

    // Use engine's sample rate if available; otherwise keep a fixed value.
    // (If Engine has sample rate field, this will compile; if not, change to 48000.0)
    const double sampleRate             = (double)engine->sampleRate;
    const unsigned long framesPerBuffer = 256;

    err = Pa_OpenDefaultStream(
        &b->stream,
        0, // no input
        b->ud.channels,
        paFloat32,
        sampleRate,
        framesPerBuffer,
        pa_callback,
        &b->ud
    );

    if (pa_print_error(err, "Pa_OpenDefaultStream")) {
        Pa_Terminate();
        free(b);
        return 1;
    }

    err = Pa_StartStream(b->stream);
    if (pa_print_error(err, "Pa_StartStream")) {
        Pa_CloseStream(b->stream);
        Pa_Terminate();
        free(b);
        return 1;
    }

    *out_backend = b;

    printf("Pa_OpenDefaultStream err=%d\n", (int)err);
    printf("Pa_StartStream err=%d\n", (int)err);


    return 0;

}

int audio_backend_stop(AudioBackendPa* backend)
{
    if (!backend) return 0;
    
    PaError err;

    if (backend->stream) {
        err = Pa_StopStream(backend->stream);
        pa_print_error(err, "Pa_StopStream");
        
        err = Pa_CloseStream(backend->stream);
        pa_print_error(err, "Pa_StopStream");

        backend->stream = NULL;
    }

    if (backend->pa_initialized) {
        err = Pa_Terminate();
        pa_print_error(err, "Pa_Terminate");
        backend->pa_initialized = 0;
    }

    free(backend);
    return 0;

}

void audio_backend_sleep_ms(int ms)
{
    if (ms < 0) ms = 0;
    Pa_Sleep((long)ms);
}

