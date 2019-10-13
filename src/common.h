// This file is part of mn - simple metronome.
// See LICENSE For copyright and license information.

#ifndef MN_COMMON_HEADER_GUARD_
#define MN_COMMON_HEADER_GUARD_

#define MN_SAMPLE_RATE 44100ul // Hz

void mn_debug(char const *format, ...) __attribute__((format(printf, 1, 2)));
void mn_warn(char const *format, ...) __attribute__((format(printf, 1, 2)));
void mn_error(char const *format, ...) __attribute__((format(printf, 1, 2)));
void mn_perror(char const *name);

#endif
