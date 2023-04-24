#include "VoiceManager.h"

Voice* VoiceManager::findFreeVoice() {
	// Returning NULL means that all voices are currently playing.
	// NULL wouldn't be allowed when we return a reference:
	Voice* freeVoice = NULL;
	// This just iterates over all voices and finds the first one that isn’t currently playing
	for(int i = 0; i < NumberOfVoices; i++) {
		if(!voices[i].isActive) {
			freeVoice = &(voices[i]);
			break;
		}
	}
	return freeVoice;
}

void VoiceManager::onNoteOn(int noteNumber, int velocity) {
	Voice* voice = findFreeVoice();
	if(!voice) {
		// If there's no free voice(all voices are playing), we just return.
		// This means that when all voices are in use, pressing a note will do nothing.
		return;
	}
	voice->reset();
	voice->setNoteNumber(noteNumber);
	voice->mVelocity = velocity;
	voice->isActive = true;
	voice->mVolumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
	voice->mFilterEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
}

void VoiceManager::onNoteOff(int noteNumber, int velocity) {
	// Find the voice(s) with the given noteNumber:
	for(int i = 0; i < NumberOfVoices; i++) {
		// Unlike onNoteOn(), get a reference to array:
		Voice& voice = voices[i];
		if(voice.isActive && voice.mNoteNumber == noteNumber) {
			voice.mVolumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
			voice.mVolumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
		}
	}
}

double VoiceManager::nextSample() {
	double output = 0.0;
	double lfoValue = mLFO.nextSample();
	for(int i = 0; i < NumberOfVoices; i++) {
		Voice& voice = voices[i];
		voice.setLFOValue(lfoValue);
		output += voice.nextSample();
	}
	return output;
}