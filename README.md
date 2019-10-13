# mn

A simple command-line metronome.

Plays a sound at a regular interval via PulseAudio.

# Installation

```shell
	make PREFIX=/usr/local clean install
```

You can of course change the install directory by specifying a different prefix,
but make sure that PREFIX is an absolute path, and don't forget to run `clean`
before `install`.

# Changing the sound

The sound played by the metronome is generated at build time and stored in
unsigned 8-bit format in the `tick.u8` file.
It is a sine wave with exponential decay, and you can easily tune its parameter
to your liking by editing `makechirp.c`.
