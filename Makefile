# 2018 David DiPaola
# licensed under CC0 (public domain, see https://creativecommons.org/publicdomain/zero/1.0/)

BIN = audio_helloworld

_CFLAGS = -Wall -Wextra \
	$(CFLAGS)
_LIBS = \
	-lasound

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	rm -rf $(BIN) sample.raw

sample.raw: sample.ogg
	ffmpeg -v 0 -i $< -f s16le -acodec pcm_s16le -ac 1 -ar 44100 $@

$(BIN): main.c sample.raw
	$(CC) $(_CFLAGS) $< $(_LIBS) -o $@

