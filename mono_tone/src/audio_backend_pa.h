#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Engine Engine;

int audio_backend_run(Engine* engine, int seconds);

#ifdef __cplusplus
}
#endif

