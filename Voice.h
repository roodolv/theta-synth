#ifndef __THETASYNTH__VOICE__
#define __THETASYNTH__VOICE__

#include "PolyBLEPOscillator.h"
#include "EnvelopeGenerator.h"
#include "Filter.h"

class Voice
{
public:
	// Voice gives VoiceManager access to all its private members.
	friend class VoiceManager;
	// Constructor
	Voice() :
		mNoteNumber(-1),
		mVelocity(0),
		mFilterEnvelopeAmount(0.0),
		mFilterLFOAmount(0.0),
		mOscillatorOnePitchAmount(0.0),
		mOscillatorTwoPitchAmount(0.0),
		mOscillatorMix(0.5),
		mLFOValue(0.0),
		isActive(false) {
			// Set myself free everytime my volume envelope has fully faded out of RELEASE stage:
			mVolumeEnvelope.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
	}
	// Setters
	inline void setOscillatorOnePitchAmount(double amount) { mOscillatorOnePitchAmount = amount; }
	inline void setOscillatorTwoPitchAmount(double amount) { mOscillatorTwoPitchAmount = amount; }
	inline void setOscillatorMix(double mix) { mOscillatorMix = mix; }
	inline void setFilterEnvelopeAmount(double amount) { mFilterEnvelopeAmount = amount; }
	inline void setFilterLFOAmount(double amount) { mFilterLFOAmount = amount; }
	inline void setLFOValue(double value) { mLFOValue = value; }

	inline void setNoteNumber(int noteNumber) {
		mNoteNumber = noteNumber;
		double frequency = 440.0 * pow(2.0, (mNoteNumber - 69.0) / 12.0);
		mOscillatorOne.setFrequency(frequency);
		mOscillatorTwo.setFrequency(frequency);
	}
	// Just like Oscillator::nextSample gives us the output of an Oscillator,
	// Voice::nextSample gives us the total output of a voice, after volume envelope and filtering.
	double nextSample();
	void setFree();
	// VoiceManager re-uses free voices.
	// So we need a way to reset a voice completely to its initial state.
	void reset();

private:
	PolyBLEPOscillator mOscillatorOne;
	PolyBLEPOscillator mOscillatorTwo;
	EnvelopeGenerator mVolumeEnvelope;
	EnvelopeGenerator mFilterEnvelope;
	Filter mFilter;
	// Each voice is triggered with a certain MIDI note number and a velocity.
	int mNoteNumber;
	int mVelocity;
	// Actually values below are always the same for all voices,
	// but we’re not putting them globally in our plugin class.
	// The reason is that each voice needs access to them on every sample,
	// and there’s no #include "ThetaSynth.h".
	// So it would be quite tedious to give this kind of access.(メインクラスを周辺クラスにインクルードはしたくないらしい)
	double mOscillatorMix;
	double mOscillatorOnePitchAmount;
	double mOscillatorTwoPitchAmount;
	double mFilterEnvelopeAmount;
	double mFilterLFOAmount;
	double mLFOValue;
	// We’ll move this to Voice level, so whenever a voice isn’t active,
	// it’s not processing anything (this includes its envelopes and filter).
	bool isActive;
};

#endif //__THETASYNTH__VOICE__