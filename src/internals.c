// This file is part of mn - simple metronome.
// See LICENSE For copyright and license information.

#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

#define MN_N_BEATS 4 // Number of beats in the loop

static size_t const c_max_msg_size = 1024;

static void print(char const *prefix, char const *format, va_list ap)
{
	char buffer[c_max_msg_size];
	vsnprintf(buffer, c_max_msg_size, format, ap);
	fprintf(stderr, "%s%s\n", prefix, buffer);
}

void mn_debug(char const *format, ...)
{
#ifndef NDEBUG
	va_list ap;
	va_start(ap, format);
	print("", format, ap);
	va_end(ap);
#else
	(void)format;
#endif
}

void mn_warn(char const *format, ...)
{
	va_list ap;
	va_start(ap, format);
	print("Warning - ", format, ap);
	va_end(ap);
}

void mn_error(char const *format, ...)
{
	va_list ap;
	va_start(ap, format);
	print("ERROR - ", format, ap);
	va_end(ap);
}

void mn_perror(char const *name)
{
#ifndef NDEBUG
	perror(name);
#else
	(void)name;
#endif
}

ssize_t read_tick(char const *path, size_t bufsize, uint8_t *buf)
{
	int fd;

	if ((fd = open(path, O_RDONLY)) == -1) {
		mn_perror("open");
		return -1;
	}

	ssize_t bytes_read;

	if ((bytes_read = read(fd, buf, bufsize)) == -1)
		mn_perror("read");

	return bytes_read;
}

ssize_t make_loop(size_t tick_n_samples,
		  uint8_t const *tick,
		  int bpm,
		  uint8_t **buf)
{
	assert(buf != NULL);

	size_t samples_per_beat = MN_SAMPLE_RATE * (60 / bpm);
	size_t n_samples = samples_per_beat * MN_N_BEATS;
	*buf = malloc(n_samples * sizeof(uint8_t));

	if (!(*buf)) {
		mn_perror("malloc");
		return -1;
	}

	uint8_t silence = 127;

	for (size_t beat = 0; beat < MN_N_BEATS; ++beat)
		for (size_t sample = 0; sample < samples_per_beat; ++sample) {
			(*buf)[beat * samples_per_beat + sample] =
			    (sample < tick_n_samples) ? tick[sample] : silence;
		}

	return n_samples;
}
