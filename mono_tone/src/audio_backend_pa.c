#include "audio_backend_pa.h"
#include "engine.h"
#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Engine* engine;
    long long framesRemaining;
    int channels;
} PaUserData;

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

    if (ud->framesRemaining <= 0) {
        // Fill silence and stop
        memset(out, 0, sizeof(float) * frameCount * (unsigned long)ud->channels);
        return paComplete;
    }

    unsigned long framesToRender = frameCount;
    if ((long long)framesToRender > ud->framesRemaining) {
        framesToRender = (unsigned long)ud->framesRemaining;
    }

    // Render audio
    // This project generates interleaved stereo f32 (LRLR...)
    // If channels != 2, we still write something reasonable.
    if (ud->channels == 2) {
        engine_process_interleaved_stereo_f32(ud->engine, out, (int)framesToRender);
    } else if (ud->channels == 1) {
        // Render stereo to temp then downmix -> mono (simple & safe)
        float* tmp = (float*)alloca(sizeof(float) * framesToRender * 2);
        engine_process_interleaved_stereo_f32(ud->engine, tmp, (int)framesToRender);
        for (unsigned long i = 0; i < framesToRender; ++i) {
            out[i] = 0.5f * (tmp[2*i] + tmp[2*i + 1]);
        }
    } else {
        // Fallback: write silence for unsupported channel counts
        memset(out, 0, sizeof(float) * framesToRender * (unsigned long)ud->channels);
    }

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
}

int audio_backend_run(Engine* engine, int seconds)
{
    if (!engine || seconds <= 0) return 1;

    PaError err = paNoError;
    PaStream* stream = NULL;

    const double sampleRate = 48000.0;   // must match engine_init default in main.c
    const int channels = 2;              // engine renders stereo interleaved
    const unsigned long framesPerBuffer = 256;

    PaUserData ud;
    ud.engine = engine;
    ud.channels = channels;
    ud.framesRemaining = (long long)seconds * (long long)sampleRate;

    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio: Pa_Initialize failed: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    // DEBUG: list all output devices
    int ndev = Pa_GetDeviceCount();
    fprintf(stderr, "PortAudio: device count = %d\n", ndev);

    PaDeviceIndex defOut = Pa_GetDefaultOutputDevice();
    fprintf(stderr, "PortAudio: default output index = %d\n", (int)defOut);

    if (defOut != paNoDevice)
    {
        const PaDeviceInfo* di = Pa_GetDeviceInfo(defOut);
        if (di) {
            fprintf(stderr,
                    "PortAudio: Default output device: %s (maxOutCh=%d)\n",
                    di->name,
                    di->maxOutputChannels);
        }
    }

    for (PaDeviceIndex i = 0; i < ndev; ++i)
    {
        const PaDeviceInfo* di = Pa_GetDeviceInfo(i);
        if (!di) continue;
        fprintf(stderr, "  [%d] %s (maxOutCh=%d)\n", (int)i, 
                di->name, di->maxOutputChannels);
    }

    /*
    // Pa_OpenDefaultStream() was originally used to open the system's
    // default output device.
    // However, on MSYS2 + Windows, the default device reported by PortAudio
    // can be invalid or non-playable (e.g. shown as "?").
    // To ensure audio is routed to a known working device, we explicitly
    // specify the output device using Pa_OpenStream() and PaStreamParameters.

    err = Pa_OpenDefaultStream(&stream,
                              0,                // no input
                              channels,         // output channels
                              paFloat32,        // sample format
                              sampleRate,
                              framesPerBuffer,
                              pa_callback,
                              &ud);
    */

    /*
    // Pa_OpenDefaultStream() was replaced because the default output device
    // reported by PortAudio is unreliable on MSYS2/Windows.
    // We explicitly specify the output device to ensure correct audio routing.
    */

    PaStreamParameters outParams;
    memset(&outParams, 0, sizeof(outParams));

    outParams.device                     = 10; // Realtek Speakers (verified output device)
    outParams.channelCount               = channels;
    outParams.sampleFormat               = paFloat32;
    outParams.suggestedLatency           = \
        Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;
    outParams.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, NULL, &outParams, sampleRate, 
                        framesPerBuffer, paNoFlag, pa_callback, &ud);
    
    if (err != paNoError) {
        fprintf(stderr, "PortAudio: Pa_OpenDefaultStream failed: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio: Pa_StartStream failed: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    // Wait until the callback says we're done
    while ((err = Pa_IsStreamActive(stream)) == 1) {
        Pa_Sleep(20);
    }

    if (err < 0) {
        fprintf(stderr, "PortAudio: Pa_IsStreamActive error: %s\n", Pa_GetErrorText(err));
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return (err < 0) ? 1 : 0;
}

