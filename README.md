# Pure_C_Audio_Engine
- A minimal pure C audio engine using PortAudio, focused on core DSP concepts such as oscillators, gain smoothing, and real-time audio callbacks.

## Tested Environment
- OS: Windows 10 / 11
- Toolchain: MSYS2 (UCRT64)
- Shell: bash
- Compiler: gcc
- Audio backend: PortAudio

## Setup
```
./install_libraries.sh
cd single_tone
make
```


## Run

```
./single_tone/wave_generator.exe [mode] [ seconds ] [ frequency ] [ gain ]

Adds support for adjusting single_tone generation with the following options:
- mode: selects waveform style (e.g., sine, square, sawtooth)
- seconds: duration of the tone playback
- frequency: frequency in Hz
- gain: amplitude scaling factor

This update allows users to generate customized pure sine tones
from the command-line executable.

```
<br>

### Defaults

- Mode: sine

- Duration: 5 seconds

- Frequency: 440 Hz (A4)

- Gain: 0.2

```
./single_tone/wave_generator.exe sine
```

### Example

- Mode: square

- Duration: 2 seconds

- Frequency: 311.13 Hz (E♭4)

- Gain: 0.5

```
./single_tone/wave_generator.exe square 2 311.13 0.5
```
