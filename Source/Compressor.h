/*
  ==============================================================================

    Compressor.h
    Created: 14 Aug 2021 3:16:53pm
    Author:  marks

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

// maximum number of samples in the delay buffer
#define SF_COMPRESSOR_MAXDELAY   1024

// samples per update; the compressor works by dividing the input chunks into even smaller sizes,
// and performs heavier calculations after each mini-chunk to adjust the final envelope
#define SF_COMPRESSOR_SPU        32

// not sure what this does exactly, but it is part of the release curve
#define SF_COMPRESSOR_SPACINGDB  5.0f

// already defined in math.h
// #define M_PI 3.1415926535

class Compressor
{

public:

    Compressor();
    ~Compressor();
	void setSampleRate(int sr_in);
	void processBuffer(juce::AudioBuffer<float>& buffer);
	int inline getSampleRate() { return sampleRate; }
	float inline getKnee() { return knee; }
	void set_slope(float val_in);
	void set_attack(int sr_in, float attack_in);
	void set_release(int sr_in, float release_in);
	void set_wetlevel(float wet_in);
	void set_delaybufsize(int sr_in, float predelay);
	void set_linearpregain(float val_in);
	void set_linearthreshold(float val_in);
	void set_postgain(float val_in) { this->postgain = val_in; calculate_knee(getKnee()); }
	void calculate_knee(float k_in);

private:

	void allocateDelayBuffer();
	void set_meterrelease(int sr_in);
	void calculate_releasecurve();
	void perSampleProcessing(juce::AudioBuffer<float>& buffer, float* lptr, float* rptr);

	// only compressor setup since this will only once be called in the constructor
	void sf_advancecomp(float pregain, float threshold,
		float knee, float ratio, float attack, float release, float predelay, float releasezone1,
		float releasezone2, float releasezone3, float releasezone4, float postgain, float wet);

	// inline functions from the original compressor implementation
	static inline float db2lin(float db) { // dB to linear
		return pow(10.0f, 0.05f * db);
	}
	static inline float lin2db(float lin) { // linear to dB
		if (lin <= 0)
		{
			return -100;
		}
		else
		{
			return 20.0f * log10(lin);
		}
	}
	static inline float kneecurve(float x, float k, float linearthreshold) {
		// remove once bug is solved
		//DBG("x: " << x << ", k: " << k << ", linthresh: " << linearthreshold);
		float val = linearthreshold + (1.0f - exp(-k * (x - linearthreshold))) / k; DBG("val: " << val);
		return val;
	}
	static inline float kneeslope(float x, float k, float linearthreshold) {
		return k * x / ((k * linearthreshold + 1.0f) * exp(k * (x - linearthreshold)) - 1);
	}
	static inline float compcurve(float x, float k, float slope, float linearthreshold,
		float linearthresholdknee, float threshold, float knee, float kneedboffset) {
		if (x < linearthreshold)
			return x; //DBG("x < linthresh");
		if (knee <= 0.0f) // no knee in curve
			return db2lin(threshold + slope * (lin2db(x) - threshold)); //DBG("x <= 0"); 
		if (x < linearthresholdknee)
			return kneecurve(x, k, linearthreshold); //DBG("x < linthreshknee"); 
		return db2lin(kneedboffset + slope * (lin2db(x) - threshold - knee)); //DBG("else"); 
	}
	// for more information on the adaptive release curve, check out adaptive-release-curve.html demo +
	// source code included in this repo
	static inline float adaptivereleasecurve(float x, float a, float b, float c, float d) {
		// a*x^3 + b*x^2 + c*x + d
		float x2 = x * x;
		return a * x2 * x + b * x2 + c * x + d;
	}
	static inline float clampf(float v, float min, float max) {
		return v < min ? min : (v > max ? max : v);
	}
	static inline float absf(float v) {
		return v < 0.0f ? -v : v;
	}
	inline float fixf(float v, float def) {
		// fix NaN and infinity values that sneak in... not sure why this is needed, but it is
		if (std::isnan(v) || std::isinf(v))
		{
			DBG("fixf check out of bounds, v set to def at value: " << def);
			DBG("v was value: " << v);
			DBG("linenr: " << getlinenr());
			return def;
		}
		return v;
	}

	inline int getlinenr()
	{
		return debuglinenr;
	}

    int sampleRate = 48000;

	// all struct parameters from the original code
	float metergain = 1.0;
	float meterrelease;
	float threshold;
	float knee;
	float linearpregain;
	float linearthreshold;
	float slope;
	float attacksamplesinv;
	float satreleasesamplesinv;
	float wet;
	float dry;
	float k = 5.0f;
	float kneedboffset = 0.0f;
	float linearthresholdknee = 0.0f;
	float mastergain;
	float a; // adaptive release polynomial coefficients
	float b;
	float c;
	float d;
	float detectoravg = 0.0001f;
	float compgain = 1.0f;
	float maxcompdiffdb = -1.0f;
	int delaybufsize = SF_COMPRESSOR_MAXDELAY;
	int delaywritepos = 0;
	int delayreadpos = delaybufsize > 1 ? 1 : 0;
	float* delaybufL; float* delaybufR; // predelay buffer

	// additional parameters due to changeable samplerate
	float predelay;
	float attack;
	float release;
	float releasesamples;
	float postgain;
	float releasezone1, releasezone2, releasezone3, releasezone4;
	int size;
	float ang90 = (float)M_PI * 0.5f;
	float ang90inv = 2.0f / (float)M_PI;
	float enveloperate;
	int samplepos;
	float scaleddesiredgain;
	int debuglinenr;
};
