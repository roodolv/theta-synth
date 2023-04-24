#ifndef __THETASYNTH__FILTER__
#define __THETASYNTH__FILTER__

#include <cmath>

class Filter {
public:
	enum FilterMode {
		FILTER_MODE_LOWPASS = 0,
		FILTER_MODE_HIGHPASS,
		FILTER_MODE_BANDPASS,
		kNumFilterModes
	};
	Filter() :
		cutoff(0.99),
		resonance(0.01),
		cutoffMod(0.0),
		mode(FILTER_MODE_LOWPASS),
		buf0(0.0),
		buf1(0.0),
		buf2(0.0),
		buf3(0.0) {
		calculateFeedbackAmount();
	}
	double process(double inputValue);  // will be called every sample
	inline void setCutoff(double newCutoff) {
		cutoff = newCutoff;
		calculateFeedbackAmount();
	}
	inline void setResonance(double newResonance) {
		resonance = newResonance;
		calculateFeedbackAmount();
	}
	inline void setCutoffMod(double newCutoffMod) {
		cutoffMod = newCutoffMod;
		calculateFeedbackAmount();
	}
	inline void setFilterMode(FilterMode newMode) {
		mode = newMode;
	}
	// These values are the filter’s previous output samples.
	// When we re-use a Voice, we want these to be clean.
	void reset() {
		buf0 = buf1 = buf2 = buf3 = 0.0;
	}

private:
	double cutoff;
	double resonance;
	double cutoffMod;
	FilterMode mode;  // what mode the filter is currently in
	// values below are used by the filter algorithm
	double feedbackAmount;
	inline void calculateFeedbackAmount() {
		feedbackAmount = resonance + resonance / (1.0 - getCalculatedCutoff());
	}
	inline double getCalculatedCutoff() const {
		return fmax(fmin(cutoff + cutoffMod, 0.99), 0.01);
	}
	double buf0;
	double buf1;
	double buf2;
	double buf3;
};

#endif //__THETASYNTH__FILTER__