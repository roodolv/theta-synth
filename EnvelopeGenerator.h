#ifndef __THETASYNTH__ENVELOPE__GEN__
#define __THETASYNTH__ENVELOPE__GEN__

#include <cmath>
#include "GallantSignal.h"
using Gallant::Signal0;

class EnvelopeGenerator {
public:
	// EG is a finite state machine (finite automaton):
	enum EnvelopeStage {
		ENVELOPE_STAGE_OFF = 0,
		ENVELOPE_STAGE_ATTACK,	//1
		ENVELOPE_STAGE_DECAY,	//2
		ENVELOPE_STAGE_SUSTAIN,	//3
		ENVELOPE_STAGE_RELEASE,	//4
		kNumEnvelopeStages		//5
	};
	void enterStage(EnvelopeStage newStage);
	double nextSample();
	static void setSampleRate(double newSampleRate);
	inline EnvelopeStage getCurrentStage() const { return currentStage; }
	const double minimumLevel; // is needed because the envelope calculations don’t work with an amplitude of zero.
	void setStageValue(EnvelopeStage stage, double value);
	// When we re-use a Voice, we want these to be clean.
	void reset() {
		currentStage = ENVELOPE_STAGE_OFF;
		currentLevel = minimumLevel;
		multiplier = 1.0;
		currentSampleIndex = 0;
		nextStageSampleIndex = 0;
	}
	Signal0<> beganEnvelopeCycle;
	Signal0<> finishedEnvelopeCycle;

	EnvelopeGenerator() :
		currentStage(ENVELOPE_STAGE_OFF),
		minimumLevel(0.0001),
		currentLevel(minimumLevel),
		multiplier(1.0),
		nextStageSampleIndex(0)
	{
		stageValue[ENVELOPE_STAGE_OFF] = 0.0;		//0sec
		stageValue[ENVELOPE_STAGE_ATTACK] = 0.01;	//0.01sec
		stageValue[ENVELOPE_STAGE_DECAY] = 0.5;		//0.5sec
		stageValue[ENVELOPE_STAGE_SUSTAIN] = 0.1;	//quiet sustain
		stageValue[ENVELOPE_STAGE_RELEASE] = 1.0;	//1.0sec

	}

private:
	EnvelopeStage currentStage;
	double currentLevel;	// we’ll get this on every sample
	double multiplier;		// is responsible for the exponential decay
	// Different components shouldn't be run at different sample rates, so make this "static":
	static double sampleRate;
	double stageValue[kNumEnvelopeStages];
	void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);
	unsigned long long currentSampleIndex;	// Instead of comparing some double value, we’re using this
	unsigned long long nextStageSampleIndex;
};

#endif //__THETASYNTH__ENVELOPE__GEN__