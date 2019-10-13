// This file is part of mn - simple metronome.
// See LICENSE For copyright and license information.

#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define MAKECHIRP_BUFSIZE 65535

struct params {
	unsigned int sample_rate;
	unsigned int frequency;
	unsigned int n_samples;
	unsigned int amplitude;
};

static void make_chirp(struct params const *p, uint8_t *buf);

int main(void)
{
	struct params p;

	p.sample_rate = 44100;
	p.frequency = 1000;
	p.n_samples = 2205;
	p.amplitude = 80;

	uint8_t buf[MAKECHIRP_BUFSIZE];
	make_chirp(&p, buf);

	if (write(STDOUT_FILENO, buf, p.n_samples) < 0) {
		perror("write");
		return 1;
	}

	return 0;
}

void make_chirp(struct params const *p, uint8_t *buf)
{
	double const multiplier = 2.0 * M_PI * p->frequency;
	double const tau = 0.2 * (double)p->n_samples / (double)p->sample_rate;
	double const amplitude = (double)p->amplitude / 100.0;

	for (size_t sample = 0; sample < p->n_samples; ++sample) {
		double const time = (double)sample / (double)p->sample_rate;

		double const data =
		    amplitude * exp(-time / tau) * sin(multiplier * time);

		buf[sample] = (uint8_t)round(127.5 + 127.5 * data);
	}
}
