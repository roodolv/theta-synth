#include "EnvelopeGenerator.h"

// static member initialization:
// The sample rate is now static, so all EnvelopeGenerators use the same one.
double EnvelopeGenerator::sampleRate = 44100.0;


double EnvelopeGenerator::nextSample() {
	// The OFF and SUSTAIN stages don’t expire after a given time,
	// so the generator doesn’t have to check if they are over.
	if(currentStage != ENVELOPE_STAGE_OFF && currentStage != ENVELOPE_STAGE_SUSTAIN) {
		// currentSampleIndex "has reached" the value of nextStageSampleIndex
		if(currentSampleIndex == nextStageSampleIndex) {
			EnvelopeStage newStage = static_cast<EnvelopeStage>(
				// ((n-1) +1) % n = 0となるので4の次が0になりループする
				(currentStage + 1) % kNumEnvelopeStages
				);
			enterStage(newStage);
		}
		currentLevel *= multiplier;	// to get an exponential transition.
		currentSampleIndex++;		// increment to keep track of time
	}
	return currentLevel;
}

void EnvelopeGenerator::calculateMultiplier(double startLevel,
											double endLevel,
											unsigned long long lengthInSamples) {
	// multiplier will be a number slightly below or slightly above 1.
	// log() is the natural logarithm.
	multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}

void EnvelopeGenerator::enterStage(EnvelopeStage newStage) {
	// It makes sure that the generator can’t go from a stage into the same stage,
	// and that a new voice won't have any effect on the last voices that already entered RELEASE stage.
	if(currentStage == newStage)			return;
	// When we go out of the OFF stage, it means we’re beginning a new cycle.
	if(currentStage == ENVELOPE_STAGE_OFF)	beganEnvelopeCycle();
	// When we go into the OFF stage, it means we have finished a cycle.
	if(newStage     == ENVELOPE_STAGE_OFF)	finishedEnvelopeCycle();

	currentStage = newStage;
	currentSampleIndex = 0;
	if(	currentStage == ENVELOPE_STAGE_OFF ||
		currentStage == ENVELOPE_STAGE_SUSTAIN) {
		nextStageSampleIndex = 0;
	} else {
		// Since stageValue[currentStage] gives us a double value (the stage duration in seconds),
		// we multiply with sampleRate to get the stage length in samples.
		nextStageSampleIndex = stageValue[currentStage] * sampleRate;
	}
	switch(newStage) {
		case ENVELOPE_STAGE_OFF:
			currentLevel = 0.0;
			multiplier = 1.0;
			break;
		case ENVELOPE_STAGE_ATTACK:
			currentLevel = minimumLevel;
			calculateMultiplier(currentLevel,
								1.0,
								nextStageSampleIndex);
			break;
		case ENVELOPE_STAGE_DECAY:
			currentLevel = 1.0;
			calculateMultiplier(currentLevel,
								fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel),
								nextStageSampleIndex);
			break;
		case ENVELOPE_STAGE_SUSTAIN:
			// stageValue[ENVELOPE_STAGE_SUSTAIN] holds a level value, not a time value.
			// So we just assign that to currentLevel.
			currentLevel = stageValue[ENVELOPE_STAGE_SUSTAIN];
			multiplier = 1.0;
			break;
		case ENVELOPE_STAGE_RELEASE:
			// We could go from ATTACK/DECAY to RELEASE,
			// so we're not changing currentLevel here.
			calculateMultiplier(currentLevel,
								minimumLevel,
								nextStageSampleIndex);
			break;
		default:
			break;
	}
}

void EnvelopeGenerator::setSampleRate(double newSampleRate) {
	sampleRate = newSampleRate;
}

// ThetaSynth::OnParamChange() calls this:
void EnvelopeGenerator::setStageValue(EnvelopeStage stage, double value) {
	// After this, we multiply stageValue[] with SampleRate on calling enterStage()
	stageValue[stage] = value;
	if(stage == currentStage) {
		// Re-calculate the multiplier and nextStageSampleIndex
		if(currentStage == ENVELOPE_STAGE_ATTACK ||
			currentStage == ENVELOPE_STAGE_DECAY ||
			currentStage == ENVELOPE_STAGE_RELEASE) {
			double nextLevelValue;
			switch(currentStage) {
				case ENVELOPE_STAGE_ATTACK:
					nextLevelValue = 1.0;
					break;
				case ENVELOPE_STAGE_DECAY:
					// fmax() returns a value bigger than minimumLevel:
					nextLevelValue = fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel);
					break;
				case ENVELOPE_STAGE_RELEASE:
					nextLevelValue = minimumLevel;
				default:
					break;
			}
			// How far the generator is into the current stage (0.0 to 1.0):
			double currentStageProcess = (currentSampleIndex + 0.0) / nextStageSampleIndex;
			// How much of the current stage is left (0.0 to 1.0) :
			double remainingStageProcess = 1.0 - currentStageProcess;
			unsigned long long samplesUntilNextStage = remainingStageProcess * value * sampleRate;
			// Update nextStageSampleIndex and multiplier:
			nextStageSampleIndex = currentSampleIndex + samplesUntilNextStage;
			calculateMultiplier(currentLevel, nextLevelValue, samplesUntilNextStage);
		} else if(currentStage == ENVELOPE_STAGE_SUSTAIN) {
			currentLevel = value;
		}
	}
	if(currentStage == ENVELOPE_STAGE_DECAY &&
		stage == ENVELOPE_STAGE_SUSTAIN) {
		// Decayステージの下り坂(残り部分)を再計算する
		// Decayのツマミを弄った訳ではないのでnextStageSampleIndexは不変
		unsigned long long samplesUntilNextStage = nextStageSampleIndex - currentSampleIndex;
		calculateMultiplier(currentLevel,
							fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel),
							samplesUntilNextStage);
	}
}