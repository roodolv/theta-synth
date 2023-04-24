#include "ThetaSynth.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmain"
#include "IPlug_include_in_plug_src.h"
#pragma clang diagnostic pop
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"
// C++ Technical Report 1
#include <functional>

const int kNumPrograms = 5;
const double parameterStep = 0.001;

enum EParams
{
	// Oscillator Section:
	mOsc1Waveform = 0,
	mOsc1PitchMod,
	mOsc2Waveform,
	mOsc2PitchMod,
	mOscMix,
	// Filter Section:
	mFilterMode,
	mFilterCutoff,
	mFilterResonance,
	mFilterLfoAmount,
	mFilterEnvAmount,
	// LFO:
	mLFOWaveform,
	mLFOFrequency,
	// Volume Envelope:
	mVolumeEnvAttack,
	mVolumeEnvDecay,
	mVolumeEnvSustain,
	mVolumeEnvRelease,
	// Filter Envelope:
	mFilterEnvAttack,
	mFilterEnvDecay,
	mFilterEnvSustain,
	mFilterEnvRelease,
	kNumParams
};

typedef struct {
	const char* name;
	const int x;
	const int y;
	const double defaultVal;
	const double minVal;
	const double maxVal;
} parameterProperties_struct;

const parameterProperties_struct parameterProperties[kNumParams] =
{
	{ /*name*/ "Osc 1 Waveform",		/*  x  */ 27,	/*  y  */ 84 },
	{ /*name*/ "Osc 1 Pitch Mod",		/*  x  */ 73,	/*  y  */ 73,	/*defaultVal*/ 0.0,	/*minVal*/ 0.0,  /*maxVal*/ 1.0 },
	{ /*name*/ "Osc 2 Waveform",		/*  x  */ 246,	/*  y  */ 84 },
	{ /*name*/ "Osc 2 Pitch Mod",		/*  x  */ 187,	/*  y  */ 73,	/*defaultVal*/ 0.0,	/*minVal*/ 0.0,  /*maxVal*/ 1.0 },
	{ /*name*/ "Osc Mix",				/*  x  */ 130,	/*  y  */ 73,	/*defaultVal*/ 0.5,	/*minVal*/ 0.0,  /*maxVal*/ 1.0 },
	{ /*name*/ "Filter Mode",			/*  x  */ 28,	/*  y  */ 190 },
	{ /*name*/ "Filter Cutoff",			/*  x  */ 73,	/*  y  */ 179,	/*defaultVal*/ 0.99,/*minVal*/ 0.0,  /*maxVal*/ 0.99 },
	{ /*name*/ "Filter Resonance",		/*  x  */ 130,	/*  y  */ 179,	/*defaultVal*/ 0.0,	/*minVal*/ 0.0,  /*maxVal*/ 1.0 },
	{ /*name*/ "Filter LFO Amount",		/*  x  */ 187,	/*  y  */ 179,	/*defaultVal*/ 0.0,	/*minVal*/ 0.0,  /*maxVal*/ 1.0 },
	{ /*name*/ "Filter Envelope Amount",/*  x  */ 239,	/*  y  */ 179,	/*defaultVal*/ 0.0,	/*minVal*/ -1.0, /*maxVal*/ 1.0 },
	{ /*name*/ "LFO Waveform",			/*  x  */ 27,	/*  y  */ 296 },
	{ /*name*/ "LFO Frequency",			/*  x  */ 73,	/*  y  */ 285,	/*defaultVal*/ 6.0,	/*minVal*/ 0.01, /*maxVal*/ 30.0 },
	{ /*name*/ "Volume Env Attack",		/*  x  */ 325,	/*  y  */ 73,	/*defaultVal*/ 0.01,/*minVal*/ 0.01, /*maxVal*/ 10.0 },
	{ /*name*/ "Volume Env Decay",		/*  x  */ 380,	/*  y  */ 73,	/*defaultVal*/ 0.5,	/*minVal*/ 0.01, /*maxVal*/ 15.0 },
	{ /*name*/ "Volume Env Sustain",	/*  x  */ 435,	/*  y  */ 73,	/*defaultVal*/ 0.1,	/*minVal*/ 0.001,/*maxVal*/ 1.0 },
	{ /*name*/ "Volume Env Release",	/*  x  */ 490,	/*  y  */ 73,	/*defaultVal*/ 1.0,	/*minVal*/ 0.01, /*maxVal*/ 15.0 },
	{ /*name*/ "Filter Env Attack",		/*  x  */ 325,	/*  y  */ 179,	/*defaultVal*/ 0.01,/*minVal*/ 0.01, /*maxVal*/ 10.0 },
	{ /*name*/ "Filter Env Decay",		/*  x  */ 380,	/*  y  */ 179,	/*defaultVal*/ 0.5,	/*minVal*/ 0.01, /*maxVal*/ 15.0 },
	{ /*name*/ "Filter Env Sustain",	/*  x  */ 435,	/*  y  */ 179,	/*defaultVal*/ 0.1,	/*minVal*/ 0.001,/*maxVal*/ 1.0 },
	{ /*name*/ "Filter Env Release",	/*  x  */ 490,	/*  y  */ 179,	/*defaultVal*/ 1.0,	/*minVal*/ 0.01, /*maxVal*/ 15.0 }
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,
	kKeybX = 50,
	kKeybY = 425
};

ThetaSynth::ThetaSynth(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	lastVirtualKeyboardNoteNumber(virtualKeyboardMinimumNoteNumber - 1)
{
	TRACE;

	CreateParams();
	CreateGraphics();
	CreatePresets();

	// Here is how to use phogan's signal library(GallantSignal.h):
	mMIDIReceiver.noteOn.Connect(&voiceManager, &VoiceManager::onNoteOn);
	mMIDIReceiver.noteOff.Connect(&voiceManager, &VoiceManager::onNoteOff);
}

ThetaSynth::~ThetaSynth() {}

void ThetaSynth::CreateParams() {
	for(int i = 0; i < kNumParams; i++) {
		// Get paramIdx from EParams
		IParam* param = GetParam(i);
		// Set a reference to struct_array
		const parameterProperties_struct& properties = parameterProperties[i];
		switch(i) {
			// Enum Parameters:
			case mOsc1Waveform:
			case mOsc2Waveform:
				param->InitEnum(properties.name,
					Oscillator::OSCILLATOR_MODE_SAW,
					Oscillator::kNumOscillatorModes);
				// For VST3:
				param->SetDisplayText(0, properties.name);
				break;
			case mLFOWaveform:
				param->InitEnum(properties.name,
					Oscillator::OSCILLATOR_MODE_TRIANGLE,
					Oscillator::kNumOscillatorModes);
				// For VST3:
				param->SetDisplayText(0, properties.name);
				break;
			case mFilterMode:
				param->InitEnum(properties.name,
					Filter::FILTER_MODE_LOWPASS,
					Filter::kNumFilterModes);
				break;
			// Double Parameters:
			default:
				param->InitDouble(properties.name,
					properties.defaultVal,
					properties.minVal,
					properties.maxVal,
					parameterStep);
				break;
		}
	}
	GetParam(mFilterCutoff)->SetShape(2);
	GetParam(mVolumeEnvAttack)->SetShape(3);
	GetParam(mFilterEnvAttack)->SetShape(3);
	GetParam(mVolumeEnvDecay)->SetShape(3);
	GetParam(mFilterEnvDecay)->SetShape(3);
	GetParam(mVolumeEnvSustain)->SetShape(2);
	GetParam(mFilterEnvSustain)->SetShape(2);
	GetParam(mVolumeEnvRelease)->SetShape(3);
	GetParam(mFilterEnvRelease)->SetShape(3);

	for(int i = 0; i < kNumParams; i++) {
		// call OnParamChange() for every parameter once
		// so the plugin have the right internal values when it’s first loaded:
		OnParamChange(i);
	}
}

void ThetaSynth::CreateGraphics() {
	/* [GUI] Total */
	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BG_ID, BG_FN);

	/* [GUI] Virtual keyboard */
	IBitmap whiteKeyImage = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, /* Number of States(for MultiBitmap) */ 6);
	IBitmap blackKeyImage = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);
	// By default pRegularKeys should contain 6 bitmaps (C/F, D, E/B, G, A, high C),
	// while pSharpKey should only contain 1 bitmap (for all flat/sharp keys).
	// The keyCoordinates array tells the system how far each key is offset from the left.
	//                           C#    D#       F#    G#    A#
	int keyCoordinates[12] = { 0,10,17,30,35,52,61,68,79,85,97,102 };
	// 親クラスIControl型ポインタmVirtualKeyboardへ子クラスIKeyboardControl型アドレスを渡す
	// このポインタから親子両方のメソッド(メンバ関数)を呼び出せる
	mVirtualKeyboard = new IKeyboardControl(this, kKeybX, kKeybY, virtualKeyboardMinimumNoteNumber, /* octaves: */ 4, &whiteKeyImage, &blackKeyImage, keyCoordinates);
	pGraphics->AttachControl(mVirtualKeyboard);

	/* [GUI] Knobs & Switches */
	IBitmap waveformBitmap = pGraphics->LoadIBitmap(WAVEFORM_ID, WAVEFORM_FN, 4);
	IBitmap filterBitmap = pGraphics->LoadIBitmap(FILTERMODE_ID, FILTERMODE_FN, 3);
	IBitmap knobBitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, 64);

	for(int i = 0; i < kNumParams; i++) {
		const parameterProperties_struct& properties = parameterProperties[i];
		IControl* control;
		IBitmap* graphic;
		switch(i) {
			// Switches:
			case mOsc1Waveform:
			case mOsc2Waveform:
			case mLFOWaveform:
				graphic = &waveformBitmap;
				control = new ISwitchControl(this, properties.x, properties.y, i, graphic);
				break;
			case mFilterMode:
				graphic = &filterBitmap;
				control = new ISwitchControl(this, properties.x, properties.y, i, graphic);
				break;
			// Knobs:
			default:
				graphic = &knobBitmap;
				control = new IKnobMultiControl(this, properties.x, properties.y, i, graphic);
				break;
		}
		pGraphics->AttachControl(control);
	}
	// All GUI components are attached here
	AttachGraphics(pGraphics);
}

void ThetaSynth::CreatePresets() {}

void ThetaSynth::ProcessDoubleReplacing(double** inputs,	double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	double *leftOutput = outputs[0];
	double *rightOutput = outputs[1];
	processVirtualKeyboard();
	///////// Main Audio Processing /////////
	for(int i = 0; i < nFrames; i++) {
		mMIDIReceiver.advance();
		leftOutput[i] = rightOutput[i] = voiceManager.nextSample();
	}
	// Finally we call Flush to move the MIDI queue’s front.
	mMIDIReceiver.Flush(nFrames);
}

void ThetaSynth::Reset()
{
	TRACE;
	IMutexLock lock(this);
	// サンプリングレート更新はReset()関数で行う
	double sampleRate = GetSampleRate();
	voiceManager.setSampleRate(sampleRate);
}

void ThetaSynth::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);
	IParam* param = GetParam(paramIdx);
	if(paramIdx == mLFOWaveform) {
		voiceManager.setLFOMode(static_cast<Oscillator::OscillatorMode>(param->Int()));
	} else if(paramIdx==mLFOFrequency) {
		voiceManager.setLFOFrequency(param->Value());
	} else {
		// Now we can just type _1 and bind
		using std::tr1::placeholders::_1; // a mark that represents "arguments start from me"
		using std::tr1::bind;
		VoiceManager::VoiceChangerFunction changer;
		switch(paramIdx) {
			// Oscillator Section:
			case mOsc1Waveform:
				changer = bind(&VoiceManager::setOscillatorMode, _1, 1, static_cast<Oscillator::OscillatorMode>(param->Int()));
				break;
			case mOsc1PitchMod:
				changer = bind(&VoiceManager::setOscillatorPitchMod, _1, 1, param->Value());
				break;
			case mOsc2Waveform:
				changer = bind(&VoiceManager::setOscillatorMode, _1, 2, static_cast<Oscillator::OscillatorMode>(param->Int()));
				break;
			case mOsc2PitchMod:
				changer = bind(&VoiceManager::setOscillatorPitchMod, _1, 2, param->Value());
				break;
			case mOscMix:
				changer = bind(&VoiceManager::setOscillatorMix, _1, param->Value());
				break;
			// Filter Section:
			case mFilterMode:
				changer = bind(&VoiceManager::setFilterMode, _1, static_cast<Filter::FilterMode>(param->Int()));
				break;
			case mFilterCutoff:
				changer = bind(&VoiceManager::setFilterCutoff, _1, param->Value());
				break;
			case mFilterResonance:
				changer = bind(&VoiceManager::setFilterResonance, _1, param->Value());
				break;
			case mFilterLfoAmount:
				changer = bind(&VoiceManager::setFilterLFOAmount, _1, param->Value());
				break;
			case mFilterEnvAmount:
				changer = bind(&VoiceManager::setFilterEnvAmount, _1, param->Value());
				break;
			// Volume Envelope:
			case mVolumeEnvAttack:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
				break;
			case mVolumeEnvDecay:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
				break;
			case mVolumeEnvSustain:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
				break;
			case mVolumeEnvRelease:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
				break;
			// Filter Envelope:
			case mFilterEnvAttack:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
				break;
			case mFilterEnvDecay:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
				break;
			case mFilterEnvSustain:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
				break;
			case mFilterEnvRelease:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
				break;
		}
		voiceManager.changeAllVoices(changer);
	}
}

// This function will be called WHENEVER the application receives a MIDI message.
// We’re passing the messages through to MIDI receiver and our virtual keyboard.
void ThetaSynth::ProcessMidiMsg(IMidiMsg* pMsg) {
	mMIDIReceiver.onMessageReceived(pMsg);
	mVirtualKeyboard->SetDirty();
}

// This function is called every mBlockSize samples
// We’re remembering the lastVirtualKeyboardNoteNumber to avoid a kind of “re-triggering” on every call.
// サンプル単位(より細かい単位)で関数自体を呼び出し、1つめのif文では厳密にノートオフにするかどうかを判定する
// 2つめのif文ではサンプル単位ではなくノートオンが入力された時だけメッセージ単位(より粗い単位)でif以下を実行する
void ThetaSynth::processVirtualKeyboard() {
	// mVirtualKeyboardはIControl*型なのでキャストしてIKeyboardControl*型のまま扱えるようにする
	IKeyboardControl* virtualKeyboard = (IKeyboardControl*) mVirtualKeyboard;
	// GetKey()はノートオフなら-1を返す
	int virtualKeyboardNoteNumber = virtualKeyboard->GetKey() + virtualKeyboardMinimumNoteNumber;

	// The note number has changed from a last valid (greater than minimum) key to something else (valid key or nothing).
	// キーを離した時のノートオフ信号も「前回と異なるメッセージ」と見なされる
	if(lastVirtualKeyboardNoteNumber >= virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
		IMidiMsg midiMessage;
		// Release the valid key:
		midiMessage.MakeNoteOffMsg(lastVirtualKeyboardNoteNumber, 0);
		mMIDIReceiver.onMessageReceived(&midiMessage);
	}

	// A valid (greater than minimum) key is pressed that wasn't pressed the previous call.
	if(virtualKeyboardNoteNumber >= virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
		IMidiMsg midiMessage;
		// Send a "note on" message to the MIDI receiver:
		midiMessage.MakeNoteOnMsg(virtualKeyboardNoteNumber, virtualKeyboard->GetVelocity(), 0);
		mMIDIReceiver.onMessageReceived(&midiMessage);
	}

	// The last MIDI note is forced to correspond with new one.
	lastVirtualKeyboardNoteNumber = virtualKeyboardNoteNumber;
}
