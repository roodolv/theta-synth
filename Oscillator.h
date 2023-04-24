#ifndef __THETASYNTH__OSCILLATOR__
#define __THETASYNTH__OSCILLATOR__

#include <math.h>

class Oscillator {
public:
	enum OscillatorMode {
		OSCILLATOR_MODE_SINE = 0,
		OSCILLATOR_MODE_SAW,
		OSCILLATOR_MODE_SQUARE,
		OSCILLATOR_MODE_TRIANGLE,
		kNumOscillatorModes
	};
	void setMode(OscillatorMode mode);
	void setFrequency(double frequency);
	void setSampleRate(double sampleRate);
	void setPitchMod(double amount);
	virtual double nextSample();
	// This lets the waveform start from the beginning everytime a voice starts to play.
	void reset() { mPhase = 0.0; }
	Oscillator() :
		mOscillatorMode(OSCILLATOR_MODE_SINE),
		// acos(0.0)はcosが0.0になるようなラジアン角を返す、つまりpi/2なのでPI=2*(pi/2)
		mPI(2 * acos(0.0)),
		twoPI(2 * mPI),
		mFrequency(440.0),
		mPhase(0.0),
		mPitchMod(0.0) { updateIncrement(); }

protected:
	OscillatorMode mOscillatorMode;
	// Make sure you put twoPI below the declaration of mPI,
	// because variables are initialized in the order in which they are declared.
	const double mPI;
	const double twoPI;
	double mFrequency;
	double mPhase;
	// Different components shouldn't be run at different sample rates, so make this "static":
	static double mSampleRate;
	double mPhaseIncrement;
	void updateIncrement();
	double mPitchMod;
	// Right now, nextSample calculates the waveform and increments the mPhase.
	// We have to separate these two unrelated things. Add this protected member function:
	// Naive in this case means that it generates the waveforms in a naive and incorrect way:
	double naiveWaveformForMode(OscillatorMode mode);
};

#endif //__THETASYNTH__OSCILLATOR__