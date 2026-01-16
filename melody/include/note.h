#pragma once

#include <math.h>

#define NOTE_REST (-1.0f)

typedef enum {
    NOTE_A0  = 21, NOTE_As0 = 22, NOTE_B0  = 23,

    NOTE_C1  = 24, NOTE_Cs1 = 25, NOTE_D1  = 26, NOTE_Ds1 = 27, NOTE_E1  = 28,
    NOTE_F1  = 29, NOTE_Fs1 = 30, NOTE_G1  = 31, NOTE_Gs1 = 32,
    NOTE_A1  = 33, NOTE_As1 = 34, NOTE_B1  = 35,

    NOTE_C2  = 36, NOTE_Cs2 = 37, NOTE_D2  = 38, NOTE_Ds2 = 39, NOTE_E2  = 40,
    NOTE_F2  = 41, NOTE_Fs2 = 42, NOTE_G2  = 43, NOTE_Gs2 = 44,
    NOTE_A2  = 45, NOTE_As2 = 46, NOTE_B2  = 47,

    NOTE_C3  = 48, NOTE_Cs3 = 49, NOTE_D3  = 50, NOTE_Ds3 = 51, NOTE_E3  = 52,
    NOTE_F3  = 53, NOTE_Fs3 = 54, NOTE_G3  = 55, NOTE_Gs3 = 56,
    NOTE_A3  = 57, NOTE_As3 = 58, NOTE_B3  = 59,

    NOTE_C4  = 60, NOTE_Cs4 = 61, NOTE_D4  = 62, NOTE_Ds4 = 63, NOTE_E4  = 64,
    NOTE_F4  = 65, NOTE_Fs4 = 66, NOTE_G4  = 67, NOTE_Gs4 = 68,
    NOTE_A4  = 69, NOTE_As4 = 70, NOTE_B4  = 71,

    NOTE_C5  = 72, NOTE_Cs5 = 73, NOTE_D5  = 74, NOTE_Ds5 = 75, NOTE_E5  = 76,
    NOTE_F5  = 77, NOTE_Fs5 = 78, NOTE_G5  = 79, NOTE_Gs5 = 80,
    NOTE_A5  = 81, NOTE_As5 = 82, NOTE_B5  = 83,

    NOTE_C6  = 84, NOTE_Cs6 = 85, NOTE_D6  = 86, NOTE_Ds6 = 87, NOTE_E6  = 88,
    NOTE_F6  = 89, NOTE_Fs6 = 90, NOTE_G6  = 91, NOTE_Gs6 = 92,
    NOTE_A6  = 93, NOTE_As6 = 94, NOTE_B6  = 95,

    NOTE_C7  = 96, NOTE_Cs7 = 97, NOTE_D7  = 98, NOTE_Ds7 = 99, NOTE_E7  = 100,
    NOTE_F7  = 101,NOTE_Fs7 = 102,NOTE_G7  = 103,NOTE_Gs7 = 104,
    NOTE_A7  = 105,NOTE_As7 = 106,NOTE_B7  = 107,

    NOTE_C8  = 108
} MidiNote;


static inline float midi_to_hz(int note) 
{
    return 440.0f * powf(2.0f, (note - 69) / 12.0f);
}
