# ./pure_sine [seconds] [freqHz] [gain]
# 볼륨 0.5로 -> ./pure_sine 5 440 0.5


CC=gcc
CFLAGS=-O2 -Wall -Wextra -std=c11 -Iinclude -I/ucrt64/include
LIBS=-lportaudio -lm

SRC=src/main.c src/engine.c src/audio_backend_pa.c
OUT=pure_sine

all:$(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBS)

clean:
	rm -f $(OUT)

