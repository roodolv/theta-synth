#include "Voice.h"

double Voice::nextSample() {
	if(!isActive) return 0.0;

	double oscillatorOneOutput = mOscillatorOne.nextSample();
	double oscillatorTwoOutput = mOscillatorTwo.nextSample();
	double oscillatorSum = ((1 - mOscillatorMix) * oscillatorOneOutput) + (mOscillatorMix * oscillatorTwoOutput);

	double volumeEnvelopeValue = mVolumeEnvelope.nextSample();
	double filterEnvelopeValue = mFilterEnvelope.nextSample();

	// Prepare for filtering:
	mFilter.setCutoffMod(filterEnvelopeValue * mFilterEnvelopeAmount + mLFOValue * mFilterLFOAmount);

	// Prepare for pitch modulation:
	mOscillatorOne.setPitchMod(mLFOValue * mOscillatorOnePitchAmount);
	mOscillatorTwo.setPitchMod(mLFOValue * mOscillatorTwoPitchAmount);

	// Core Audio Processing
	return mFilter.process(oscillatorSum * volumeEnvelopeValue * mVelocity / 127.0);
}

void Voice::setFree() {
	isActive = false;
}

void Voice::reset() {
	// Reset all parameters for VoiceManager to re-use Voice(s):
	mNoteNumber = -1;
	mVelocity = 0;
	mOscillatorOne.reset();
	mOscillatorTwo.reset();
	mVolumeEnvelope.reset();
	mFilterEnvelope.reset();
	mFilter.reset();
}