# ArduinoAudioInterface
Read audio signals and process them

- INPUT
	Mono signal input
	Can read up to ~500Hz reliably
	Lowpass Filter cancells out noice above ~800Hz
	Sampling rate of 10^-6 per second
	
- THROUGHPUT
	FFT on input signal
	Signal seperation into 128 bins
	Divided into 3 overlapping frequency bands
	Bands weight defined by skalars in head
	Optional Colour Switching
	
- OUTPUT
	3 x PWM Signal linear association with Bandintensity
	-> 4 inputs:
			Works for common Annode and common Cathode, but check output, tested with common Cathode
	-> 4 outputs:
			-"-
	
Requires extra powersource for LED and Arduino
