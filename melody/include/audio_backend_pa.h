#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "engine.h"

/* unactive the lines for the single_tone pjt
typedef struct Engine Engine;

int audio_backend_run(Engine* engine, int seconds);
*/

// Simple PortAudio backend instance (once stream per instance)
typedef struct AudioBackendPa AudioBackendPa;

// Create/start an output stream and begin audio callback
// Returns 0 on success
int audio_backend_start(AudioBackendPa** out_backend, Engine* engine);

int audio_backend_stop(AudioBackendPa* backend);

// Convenience sleep (milliseconds) using PortAudio sleep
void audio_backend_sleep_ms(int ms);

#ifdef __cplusplus
}
#endif

