#ifndef __THETASYNTH__
#define __THETASYNTH__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "MIDIReceiver.h"
#include "VoiceManager.h"

class ThetaSynth : public IPlug
{
public:
	ThetaSynth(IPlugInstanceInfo instanceInfo);
	~ThetaSynth();

	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
	// to receive MIDI messages:
	void ProcessMidiMsg(IMidiMsg* pMsg);

	// Should return non-zero if one or more keys are playing.
	inline int GetNumKeys() const { return mMIDIReceiver.getNumKeys(); }
	// Should return true if the specified key is playing.
	inline bool GetKeyStatus(int key) const { return mMIDIReceiver.getKeyStatus(key); }
	static const int virtualKeyboardMinimumNoteNumber = 48;
	int lastVirtualKeyboardNoteNumber;

private:
	/* In-Constructor Funcs */
	void CreateParams();
	void CreateGraphics();
	void CreatePresets();

	MIDIReceiver mMIDIReceiver;
	// We can’t declare an instance of IKeyboardControl here because it isn’t known in header files.
	// For that reason, we have to use a pointer. IKeyboardControl.h has some comments saying
	// “You should include this header file after your plug-in class has already been declared,
	// so it is propbably best to include it in your plug-in’s main .cpp file”.
	IControl* mVirtualKeyboard;
	void processVirtualKeyboard();
	VoiceManager voiceManager;
};

#endif //__THETASYNTH__