// This file is part of mn - simple metronome.
// See LICENSE For copyright and license information.

#include <stdint.h>
#include <stdio.h>

#include <pulse/error.h>
#include <pulse/simple.h>

#include "common.h"

static pa_simple *g_conn = NULL;

static void pa_perror(char const *name, int error)
{
	mn_debug("%s: %s\n", name, pa_strerror(error));
}

int sound_init(void)
{
	if (g_conn)
		return 0;

	int error = 0;
	pa_sample_spec spec;

	spec.format = PA_SAMPLE_U8;
	spec.rate = MN_SAMPLE_RATE;
	spec.channels = 1;

	if (!(g_conn = pa_simple_new(NULL,
				     "mn",
				     PA_STREAM_PLAYBACK,
				     NULL,
				     "mn",
				     &spec,
				     NULL,
				     NULL,
				     &error))) {
		pa_perror("pa_simple_new", error);
		return -1;
	}

	return 0;
}

int sound_play(size_t bufsize, uint8_t const *buf)
{
	int error = 0;

#ifndef NDEBUG
	pa_usec_t latency;

	if ((latency = pa_simple_get_latency(g_conn, &error)) == (pa_usec_t)-1)
		pa_perror("pa_simple_get_latency", error);
	else
		mn_debug("latency=%0.0f usec\t\r", (float)latency);
#endif

	if (pa_simple_write(g_conn, buf, bufsize, &error) < 0) {
		pa_perror("pa_simple_write", error);
		return -1;
	}

	if (pa_simple_drain(g_conn, &error) < 0) {
		pa_perror("pa_simple_drain", error);
		return -1;
	}

	return 0;
}

void sound_off(void)
{
	if (!g_conn)
		return;

	pa_simple_free(g_conn);
}
