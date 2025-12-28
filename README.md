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
cd mono_tone
make
```


## Run

```
./mono_tone/pure_sine.exe [ seconds ] [ frequency ] [ gain ] 
```
<br>

### Defaults

- Duration: 5 seconds

- Frequency: 440 Hz (A4)

- Gain: 0.2

```
./mono_tone/pure_sine.exe
```

### Example

- Duration: 2 seconds

- Frequency: 311.13 Hz (E♭4)

- Gain: 0.5

```
./mono_tone/pure_sine.exe 2 311.13 0.5
```
