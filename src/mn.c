// This file is part of mn - simple metronome.
// See LICENSE for copyright and license information.

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#if SIZE_MAX < UINT32_MAX
#error "Unsupported platform"
#endif

#define MN_MIN_BPM 15
#define MN_MAX_BPM 480
#define MN_MAX_TICK_SIZE 65535

#ifndef MN_TICK_FILE_PATH
#define MN_TICK_FILE_PATH "tick.u8"
#endif

ssize_t read_tick(char const *path, size_t bufsize, uint8_t *buf);
ssize_t make_loop(size_t tick_n_samples,
		  uint8_t const *tick,
		  int bpm,
		  uint8_t **buf);

int sound_init(void);
int sound_play(size_t bufsize, uint8_t const *buf);
void sound_off(void);

static int parse_bpm(char const *bpm);
static int set_signal_handler(void);

int g_terminate = 0;
static char const *c_tick_file = MN_TICK_FILE_PATH;

int main(int argc, char **argv)
{
	int rv = 1;

	if (argc < 2) {
		printf("usage: %s BPM\n", argv[0]);
		return rv;
	}

	int bpm;

	if ((bpm = parse_bpm(argv[1])) < 0) {
		mn_error("Unable to parse BPM value %s.", argv[1]);
		return rv;
	} else if (bpm < MN_MIN_BPM) {
		bpm = MN_MIN_BPM;
		mn_warn("Given BPM value is too low, playing at %d BPM.", bpm);
	} else if (bpm > MN_MAX_BPM) {
		bpm = MN_MAX_BPM;
		mn_warn("Given BPM value is too high, playing at %d BPM.", bpm);
	}

	uint8_t tick[MN_MAX_TICK_SIZE];
	ssize_t tick_n_samples;

	if ((tick_n_samples = read_tick(c_tick_file, MN_MAX_TICK_SIZE, tick)) ==
	    -1) {
		mn_error("Unable to read %s.", c_tick_file);
		return rv;
	}

	uint8_t *buf = NULL;
	ssize_t bufsize;

	if ((bufsize = make_loop(tick_n_samples, tick, bpm, &buf)) == -1) {
		mn_error("Unable to create loop.");
		return rv;
	}

	if (sound_init() == -1) {
		mn_error("Unable to connect to PulseAudio server.");
		goto cleanup;
	}

	if (set_signal_handler() == -1) {
		mn_error("Unable to set signal handler.");
		goto cleanup;
	}

	while (!g_terminate) {
		if (sound_play((size_t)bufsize, buf) == -1) {
			mn_error("Unable to play sound");
			goto cleanup;
		}
	}

	mn_debug("Stopping.");

	rv = 0;

cleanup:
	free(buf);
	sound_off();
	return rv;
}

static int parse_bpm(char const *bpm)
{
	errno = 0;
	long rv = strtol(bpm, NULL, 10);

	if (errno != 0) {
		return -1;
	}

	if (rv > INT_MAX)
		return INT_MAX;

	if (rv < INT_MIN)
		return INT_MIN;

	return (int)rv;
}

static void sighandler(int signum)
{
	switch (signum) {
	case SIGINT: // fall-through
	case SIGTERM:
		g_terminate = 1;
		break;
	default:
		break;
	}
}

static int set_signal_handler(void)
{
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		mn_perror("sigaction");
		return -1;
	}

	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		mn_perror("sigaction");
		return -1;
	}

	return 0;
}
