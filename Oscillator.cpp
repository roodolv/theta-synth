#include "Oscillator.h"

// static member initialization:
// The sample rate is now static, so all Oscillators use the same one.
double Oscillator::mSampleRate = 44100.0;


void Oscillator::setMode(OscillatorMode mode){
	mOscillatorMode = mode;
}

// We’ll call mOscillator.setFrequency() and mOscillator.generate()
// to fill the audio buffer with a tone at the right frequency.
void Oscillator::setFrequency(double frequency){
	mFrequency = frequency;
	updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate){
	mSampleRate = sampleRate;
	updateIncrement();
}

void Oscillator::setPitchMod(double amount) {
	mPitchMod = amount;
	updateIncrement();
}

void Oscillator::updateIncrement(){
	// We want pitchMod in terms of frequency, something like "plus 491.3 Hz"
	// pow(x,y) returns x^y, fabs(x) returns |x|, mPitchMod is -1 to 1.
	// fabs(mPitchMod) returns 0 to 1, and pow(~) returns 2^0 to 2^14(16384).
	// 2^0 is equal to 1, this is why the "-1" at the end:
	double pitchModAsFrequency = pow(2.0, fabs(mPitchMod) * 14.0) - 1;
	if(mPitchMod < 0) {
		pitchModAsFrequency = -pitchModAsFrequency;
	}
	// We can't go higher than this NYQUIST FREQUENCY(half the sample rate),
	// or else we'll get aliasing noise:
	double calculatedFrequency = fmin(fmax(mFrequency + pitchModAsFrequency, 0), mSampleRate / 2.0);
	//円運動の角速度ω = 2 * pi * f を更に1サンプルあたりの角速度に直す
	mPhaseIncrement = calculatedFrequency * 2 * mPI / mSampleRate;
}


// We’ll call this function ON EVERY SAMPLE to get audio data from the oscillator.
double Oscillator::nextSample() {
	double value = naiveWaveformForMode(mOscillatorMode);
	mPhase += mPhaseIncrement;
	while(mPhase >= twoPI) {
		mPhase -= twoPI;
	}
	return value;
}

double Oscillator::naiveWaveformForMode(OscillatorMode mode) {
	double value;
	//  We’re just generating a single value instead of filling an entire buffer.
	switch(mode) {
		case OSCILLATOR_MODE_SINE:
			value = sin(mPhase);
			break;
		case OSCILLATOR_MODE_SAW:
			// The saw wave is now upwards (instead of downwards):
			// value = 1.0 - (2.0 * mPhase / twoPI) is downwards
			value = (2.0 * mPhase / twoPI) - 1.0; // is upwards
			break;
		case OSCILLATOR_MODE_SQUARE:
			if(mPhase <= mPI) {
				value = 1.0;
			} else {
				value = -1.0;
			}
			break;
		case OSCILLATOR_MODE_TRIANGLE:
			value = -1.0 + (2.0 * mPhase / twoPI);
			value = 2.0 * (fabs(value) - 0.5);
			break;
		default:
			break;
	}
	return value;
}
