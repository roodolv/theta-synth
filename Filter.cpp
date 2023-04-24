#include "Filter.h"

// DSP By Paul Kellett
// http://www.musicdsp.org/showone.php?id=29

double Filter::process(double inputValue) {
	/* 直列接続の一次ローパスフィルタ4基(1基あたり-6dB/octave、4基で-24dB/octave) */

	// This if ensures the filter won’t be busy when the input is silent.
	if(inputValue == 0.0) return inputValue;
	double calculatedCutoff = getCalculatedCutoff();
	//  (buf0 - buf1) is a bandpass output
	// feedbackAmount is proportional to resonance. So the peak will be louder if resonance is high.
	buf0 += calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));	// 1基目
	buf1 += calculatedCutoff * (buf0 - buf1);											// 2基目
	buf2 += calculatedCutoff * (buf1 - buf2);											// 3基目
	buf3 += calculatedCutoff * (buf2 - buf3);											// 4基目
	switch(mode) {
		case FILTER_MODE_LOWPASS:
			// Try returning buf0 instead:
			// You’ll get an attenuation of -6dB per octave (instead of -12dB)
			return buf3;
		case FILTER_MODE_HIGHPASS:
			return inputValue - buf3;
		case FILTER_MODE_BANDPASS:
			// Subtracting a lowpass output from a lower-order lowpass gives a bandpass output.
			return buf0 - buf3;
		default:
			return 0.0;
	}
}