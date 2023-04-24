#ifndef __THETASYNTH__POLYBLEP__
#define __THETASYNTH__POLYBLEP__

#include "Oscillator.h"

class PolyBLEPOscillator : public Oscillator
{
public:
	// Constructor
	PolyBLEPOscillator() : lastOutput(0.0) { updateIncrement(); }
	// To override how waveforms are calculated:
	double nextSample();

private:
	// This will generate the ripples at the edges:
	double poly_blep(double t);
	// This will only be needed for the triangle wave:
	double lastOutput;
};

#endif //__THETASYNTH__POLYBLEP__