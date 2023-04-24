#ifndef __THETASYNTH__VOICE__MANAGER__
#define __THETASYNTH__VOICE__MANAGER__

#include "Voice.h"

// C++ Technical Report 1
#include <functional>

class VoiceManager
{
public:
	void onNoteOn(int noteNumber, int velocity);
	void onNoteOff(int noteNumber, int velocity);
	double nextSample();
	void setSampleRate(double sampleRate) {
		// EG's samplerate is static, so we only change it once:
		EnvelopeGenerator::setSampleRate(sampleRate);
		// VoiceManager has ONE LFO and so MANY Voices:
		for(int i = 0; i < NumberOfVoices; i++) {
			Voice& voice = voices[i];
			voice.mOscillatorOne.setSampleRate(sampleRate);
			voice.mOscillatorTwo.setSampleRate(sampleRate);
		}
		mLFO.setSampleRate(sampleRate);
	}
	inline void setLFOMode(Oscillator::OscillatorMode mode) { mLFO.setMode(mode); }
	inline void setLFOFrequency(double frequency) { mLFO.setFrequency(frequency); }

	// A VoiceChangerFunction is a function that takes a Voice& as first parameter and returns void
	typedef std::tr1::function<void(Voice&)> VoiceChangerFunction;
	// If at some point you decide to handle your VoiceManager‘s voices as a linked list
	// with dynamic memory allocation, you just have to change changeAllVoices:
	inline void changeAllVoices(VoiceChangerFunction changer) {
		for(int i = 0; i < NumberOfVoices; i++) {
			changer(voices[i]);
		}
	}
	// Functions below is to change a single voice:
	// They aren’t VoiceChangerFunctions because all of them take more arguments.
	// We will pre-fill all arguments except for the first one (Voice&),
	// which will turn them into VoiceChangerFunctions.
	static void setVolumeEnvelopeStageValue(Voice& voice, EnvelopeGenerator::EnvelopeStage stage, double value) {
		voice.mVolumeEnvelope.setStageValue(stage, value);
	}
	static void setFilterEnvelopeStageValue(Voice& voice, EnvelopeGenerator::EnvelopeStage stage, double value) {
		voice.mFilterEnvelope.setStageValue(stage, value);
	}
	static void setOscillatorMode(Voice& voice, int oscillatorNumber, Oscillator::OscillatorMode mode) {
		switch(oscillatorNumber) {
			case 1:
				voice.mOscillatorOne.setMode(mode);
				break;
			case 2:
				voice.mOscillatorTwo.setMode(mode);
				break;
		}
	}
	static void setOscillatorPitchMod(Voice& voice, int oscillatorNumber, double amount) {
		switch(oscillatorNumber) {
			case 1:
				voice.setOscillatorOnePitchAmount(amount);
				break;
			case 2:
				voice.setOscillatorTwoPitchAmount(amount);
				break;
		}
	}
	static void setOscillatorMix(Voice& voice, double value) {
		voice.setOscillatorMix(value);
	}
	static void setFilterCutoff(Voice& voice, double cutoff) {
		voice.mFilter.setCutoff(cutoff);
	}
	static void setFilterResonance(Voice& voice, double resonance) {
		voice.mFilter.setResonance(resonance);
	}
	static void setFilterMode(Voice& voice, Filter::FilterMode mode) {
		voice.mFilter.setFilterMode(mode);
	}
	static void setFilterEnvAmount(Voice& voice, double amount) {
		voice.setFilterEnvelopeAmount(amount);
	}
	static void setFilterLFOAmount(Voice& voice, double amount) {
		voice.setFilterLFOAmount(amount);
	}

private:
	// This uses memory for 64 voices, so you could think about using dynamic memory for this.
	// However, our plugin class is allocated dynamically (search for "new PLUG_CLASS_NAME" in IPlug_include_in_plug_src.h).
	// So all members of our plugin class go on the heap as well.
	static const int NumberOfVoices = 64;
	// The VoiceManager has one LFO (which is an Oscillator) and many Voices.
	Voice voices[NumberOfVoices];
	// You could argue that mLFO should be inside the plugin class
	// (a VoiceManager doesn’t have to know about an LFO).
	// But this introduces another layer of separation between the Voices and the LFO,
	// which means that we would need more "Glue" Code.
	Oscillator mLFO;
	// to get a voice that's not currently playing
	Voice* findFreeVoice();
};

#endif //__THETASYNTH__VOICE__MANAGER__