#include "MIDIReceiver.h"


// This function will be CALLED WHENEVER THE PLUGIN RECEIVES A MIDI MESSAGE
// by the ProcessMidiMsg() on PLUGIN.cpp
void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage) {
	IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
	// We're only interested in Note On/Off messages(not CC, pitch, etc.)
	if(status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff){
		mMidiQueue.Add(midiMessage);
	}
}


// This is CALLED ON EVERY SAMPLE while we're generating an audio buffer.
// As long as there are messages in the queue,
// we’re processing and removing them from the front(using Peek and Remove).
void MIDIReceiver::advance() {
	while(!mMidiQueue.Empty()) {
		IMidiMsg* midiMessage = mMidiQueue.Peek();
		// We only do this for MIDI messages whose mOffset isn’t greater than the current offset into the buffer.
		// This means that we process every message at the right sample, keeping the relative timing intact.
		if(midiMessage->mOffset > mOffset) break;

		// StatusMsg() returns only EStatusMsg messages:
		IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
		int noteNumber = midiMessage->NoteNumber();
		int velocity = midiMessage->Velocity();

		// There are only note on/off messages in the queue, see ::OnMessageReceived()
		if(status == IMidiMsg::kNoteOn && velocity) {
			if(mKeyStatus[noteNumber] == false) {
				mKeyStatus[noteNumber] = true;
				mNumKeys += 1;
				// noteOn is emitted when any note is pressed:
				noteOn(noteNumber, velocity);
			}
		// Zero velocity or kNoteOff is on:
		} else {
			if(mKeyStatus[noteNumber] == true) {
				mKeyStatus[noteNumber] = false;
				mNumKeys -= 1;
				// noteOff is emitted when any note is released:
				noteOff(noteNumber, velocity);
			}
		}
		mMidiQueue.Remove();
	}
	// mOffset is incremented so that the receiver knows how far into the buffer it currently is:
	mOffset++;
}