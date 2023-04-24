#ifndef __THETASYNTH__MIDIRECEIVER__
#define __THETASYNTH__MIDIRECEIVER__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
// -Wno-extra-tokens. コンパイラーは、プリプロセッサー・ディレクティブの最後にある余分なトークンについて警告しません。
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "IMidiQueue.h"

#include "GallantSignal.h"
// Signal2 is a signal that passes two parameters.
// There’s Signal0 through Signal8, so you can choose depending on how many parameters you need.
using Gallant::Signal2;

class MIDIReceiver {
public:
	// Constructor
	MIDIReceiver() :
		mNumKeys(0),
		mOffset(0) {
			for(int i = 0; i < keyCount; i++) {
				mKeyStatus[i] = false;
			}
	}

	// Returns true if the key with a given index is currently pressed
	inline bool getKeyStatus(int keyIndex) const { return mKeyStatus[keyIndex]; }
	// Returns the number of keys currently pressed
	inline int getNumKeys() const { return mNumKeys; }
	void advance();
	void onMessageReceived(IMidiMsg* midiMessage);
	// In Flush, we’re also setting mOffset to zero:
	// Calling mMidiQueue.Flush(nFrames) means that we discard nFrames from the queue’s front.
	// We have already processed that length in the last call to the advance function.
	// Resetting mOffset ensures that inside the next advance, we’re starting from the queue’s front again.
	inline void Flush(int nFrames) { mMidiQueue.Flush(nFrames); mOffset = 0; }
	inline void Resize(int blockSize) { mMidiQueue.Resize(blockSize); }

	Signal2<int, int> noteOn;
	Signal2<int, int> noteOff;

private:
	IMidiQueue mMidiQueue;
	static const int keyCount = 128;
	int mNumKeys; // how many keys are being played at the moment(via midi)
	bool mKeyStatus[keyCount]; // array of on/off for each key (index is note number)
	int mOffset;
};

#endif //__THETASYNTH__MIDIRECEIVER__