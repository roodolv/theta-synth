#include "PolyBLEPOscillator.h"

// PolyBLEP by Tale
// (slightly modified)
// http://www.kvraudio.com/forum/viewtopic.php?t=375517
double PolyBLEPOscillator::poly_blep(double t) {
	// mPhaseIncrementは1サンプルあたりの角速度
	double dt = mPhaseIncrement / twoPI;
	// 0 <= t < 1
	// The first if branch means that we’re at the beginning of a period:
	if (t < dt) {
		t /= dt;
		return t+t - t*t - 1.0;
	}
	// -1 < t < 0
	// The else if means that we’re right before the end:
	else if (t > 1.0 - dt) {
		t = (t - 1.0) / dt;
		return t*t + t+t + 1.0;
	}
	// 0 otherwise
	else return 0.0;
}

// Override Oscillator's nextSample():
double PolyBLEPOscillator::nextSample() {
	double value = 0.0;
	double t = mPhase / twoPI;

	if (mOscillatorMode == OSCILLATOR_MODE_SINE) {
		// We don't have to worry about aliasing (because SINE has no harmonics)
		value = naiveWaveformForMode(OSCILLATOR_MODE_SINE);
	} else if (mOscillatorMode == OSCILLATOR_MODE_SAW) {
		value = naiveWaveformForMode(OSCILLATOR_MODE_SAW);
		// layering the output of poly_blep on top
		// generate()'s SAW is downwards, so the "-=" means layer on top
		value -= poly_blep(t);
	}
	else {
		// We'll make a square wave for both the square and the triangle wave:
		value = naiveWaveformForMode(OSCILLATOR_MODE_SQUARE);
		// Adding two PolyBLEPs (line1, line2)
		// One at the start of a period, and the other offset by 0.5 (i.e. half a period).
		value += poly_blep(t);
		// fmod(x,y) returns x%y
		value -= poly_blep(fmod(t + 0.5, 1.0));

		if (mOscillatorMode == OSCILLATOR_MODE_TRIANGLE) {
			// Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
			// It adds the previous output, but multiplied by a value slightly below 1.
			// This prevents the offset from accumulating.
			value = mPhaseIncrement * value + (1 - mPhaseIncrement) * lastOutput;
			lastOutput = value;
		}
	}
	mPhase += mPhaseIncrement;
	while(mPhase >= twoPI) {
		mPhase -= twoPI;
	}
	return value;
}