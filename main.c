/*
2018 David DiPaola
licensed under CC0 (public domain, see https://creativecommons.org/publicdomain/zero/1.0/)
*/

#include <stdio.h>

#include <alsa/asoundlib.h>

int
main() {
	int exitcode = 0;
	int status;

	printf("getting default sound playback stream..." "\n");
	snd_pcm_t * pcm = NULL;
	status = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (status < 0) {
		printf("ERROR: %s" "\n", snd_strerror(status));
		exitcode = 1;
		goto cleanup;
	}

	printf("setting parameters..." "\n");
	const unsigned int channels   =     1;
	const unsigned int rate       = 44100;
	const          int resample   =     1;
	const unsigned int latency_us =   100;
	status = snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, resample, latency_us);
	if (status < 0) {
		printf("ERROR: %s" "\n", snd_strerror(status));
		exitcode = 1;
		goto cleanup;
	}

	printf("getting number of frames in stream..." "\n");
	snd_pcm_sframes_t snd_frames = snd_pcm_avail(pcm);
	if (snd_frames < 0) {
		printf("ERROR: %s" "\n", snd_strerror(status));
		exitcode = 1;
		goto cleanup;
	}
	printf("\t" "number of frames: %li" "\n", snd_frames);

	printf("reading frames from file..." "\n");
	FILE * file = fopen("sample.raw", "r+");
	if (!file) {
		fprintf(stderr, "ERROR: fopen()");
		exitcode = 1;
		goto cleanup;
	}
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	short * buffer = malloc(file_size);
	size_t buffer_length = file_size / sizeof(*buffer);
	fread(buffer, file_size, 1, file);
	fclose(file);
	file = NULL;

	printf("writing %zi frames to stream..." "\n", buffer_length);
	size_t i = 0;
	while (i < buffer_length) {
		snd_pcm_sframes_t written = snd_pcm_writei(pcm, &(buffer[i]), (buffer_length - i));
		if (written < 0) {
			while (snd_pcm_recover(pcm, written, 0) != 0) {
				fprintf(stderr, "\t" "INFO: recovery failed" "\n");
			}
			continue;
		}

		i += written;
	}

	cleanup:
	printf("closing sound playback stream..." "\n");
	status = snd_pcm_close(pcm);
	if (status < 0) {
		printf("ERROR: %s" "\n", snd_strerror(status));
		return 1;
	}
	pcm = NULL;

	return exitcode;
}

