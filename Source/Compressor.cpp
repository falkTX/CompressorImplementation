/*
  ==============================================================================

    Compressor.cpp
    Created: 14 Aug 2021 3:16:53pm
    Author:  marks

  ==============================================================================
*/

#include "Compressor.h"
#include <math.h>

Compressor::Compressor()
{
    allocateDelayBuffer();
    sf_advancecomp(
        0.000f, // pregain
        -12.000f, // threshold
        30.000f, // knee
        12.000f, // ratio
        0.003f, // attack
        0.250f, // release
        0.006f, // predelay
        0.090f, // releasezone1
        0.160f, // releasezone2
        0.420f, // releasezone3
        0.980f, // releasezone4
        0.000f, // postgain
        1.000f  // wet
    );
}

Compressor::~Compressor()
{
    float lMax = 0.0f, rMax = 0.0f;
    for (int i = 0; i < delaybufsize; i++)
    {
        if (delaybufL[i] > lMax)
        {
            lMax = delaybufL[i];
        }
        if (delaybufR[i] > rMax)
        {
            rMax = delaybufR[i];
        }
    }
    free(delaybufL);
    free(delaybufR);
}

void Compressor::sf_advancecomp(float pregain, float threshold,
    float knee, float ratio, float attack, float release, float predelay, float releasezone1,
    float releasezone2, float releasezone3, float releasezone4, float postgain, float wet)
{
    this->predelay = predelay;
    set_delaybufsize(sampleRate, predelay);
    set_linearpregain(pregain);
    set_linearthreshold(threshold);
    set_slope(1.0/ratio);
    this->attack = attack;
    set_attack(sampleRate, attack);
    this->release = release;
    set_release(sampleRate, release);
    set_wetlevel(wet);
    set_meterrelease(sampleRate);
    set_postgain(postgain);
    this->releasezone1 = releasezone1;
    this->releasezone2 = releasezone2;
    this->releasezone3 = releasezone3;
    this->releasezone4 = releasezone4;
    calculate_releasecurve();
    calculate_knee(knee);
}

void Compressor::allocateDelayBuffer()
{
    delaybufL = (float*)malloc(SF_COMPRESSOR_MAXDELAY * sizeof(float));
    delaybufR = (float*)malloc(SF_COMPRESSOR_MAXDELAY * sizeof(float));
    memset(delaybufL, 0.0, sizeof(float) * SF_COMPRESSOR_MAXDELAY);
    memset(delaybufR, 0.0, sizeof(float) * SF_COMPRESSOR_MAXDELAY);
}

void Compressor::setSampleRate(int sr_in) 
{ 
    sampleRate = sr_in;
    set_delaybufsize(sr_in, predelay);
    set_attack(sr_in, attack);
    set_meterrelease(sr_in);
    calculate_releasecurve();
}

void Compressor::set_delaybufsize(int sr_in, float predelay)
{
    sampleRate = sr_in;
    this->predelay = predelay;
    delaybufsize = sampleRate * predelay;
    if (delaybufsize < 1)
    {
        delaybufsize = 1;
    }
    else if (delaybufsize > SF_COMPRESSOR_MAXDELAY)
    {
        delaybufsize = SF_COMPRESSOR_MAXDELAY;
    }
    delaywritepos = 0;
    delayreadpos = delaybufsize;
}

void Compressor::set_linearpregain(float val_in)
{
    linearpregain = db2lin(val_in);
}

void Compressor::set_linearthreshold(float val_in)
{
    threshold = val_in;
    linearthreshold = db2lin(val_in);
    calculate_knee(knee);
}

void Compressor::set_slope(float val_in)
{
    slope = val_in;
    calculate_knee(knee);
}

void Compressor::set_attack(int sr_in, float attack_in)
{
    attacksamplesinv = 1.0f / ((float)sr_in * attack_in);
}

void Compressor::set_release(int sr_in, float release_in)
{
    releasesamples = sr_in * release_in;
    satreleasesamplesinv = 1.0f / ((float)sr_in * 0.0025f);
    calculate_releasecurve();
}

void Compressor::set_wetlevel(float wet_in)
{
    wet = wet_in;
    dry = 1.0f - wet_in;
}

void Compressor::set_meterrelease(int sr_in)
{
    meterrelease = 1.0f - exp(-1.0f / ((float)sr_in * 0.325f));
}

void Compressor::calculate_knee(float k_in)
{
    knee = k_in;
    k = 5.0f;
    kneedboffset = 0.0f;
    linearthresholdknee = 0.0f;
    if (knee > 0.0f) { // if a knee exists, search for a good k value
        float xknee = db2lin(threshold + knee);
        float mink = 0.1f;
        float maxk = 10000.0f;
        // search by comparing the knee slope at the current k guess, to the ideal slope
        for (int i = 0; i < 15; i++) {
            if (kneeslope(xknee, k, linearthreshold) < slope)
                maxk = k;
            else
                mink = k;
            k = sqrt(mink * maxk);
        }
        kneedboffset = lin2db(kneecurve(xknee, k, linearthreshold));
        linearthresholdknee = db2lin(threshold + knee);
    }
    // calculate a master gain based on what sounds good
    float fulllevel = compcurve(1.0f, k, slope, linearthreshold, linearthresholdknee,
        threshold, knee, kneedboffset);
    mastergain = db2lin(postgain) * pow(1.0f / fulllevel, 0.6f);
}

void Compressor::calculate_releasecurve()
{
    float y1 = releasesamples * releasezone1;
    float y2 = releasesamples * releasezone2;
    float y3 = releasesamples * releasezone3;
    float y4 = releasesamples * releasezone4;
    a = (-y1 + 3.0f * y2 - 3.0f * y3 + y4) / 6.0f;
    b = y1 - 2.5f * y2 + 2.0f * y3 - 0.5f * y4;
    c = (-11.0f * y1 + 18.0f * y2 - 9.0f * y3 + 2.0f * y4) / 6.0f;
    d = y1;
}

void Compressor::processBuffer(juce::AudioBuffer<float>& buffer)
{
    auto* lReadWritePointer = buffer.getWritePointer(0);
    auto* rReadWritePointer = buffer.getWritePointer(1);
    size = buffer.getNumSamples();
    int samplesperchunk = SF_COMPRESSOR_SPU;
    if (samplesperchunk > size)
    {
        samplesperchunk = size;
    }
    int chunks = size / samplesperchunk;
    int remainder = size - (chunks * samplesperchunk);
    samplepos = 0;

    for (int ch = 0; ch < chunks; ch++) {
        detectoravg = fixf(detectoravg, 1.0f);
        float desiredgain = detectoravg;
        scaleddesiredgain = asin(desiredgain) * ang90inv;
        float compdiffdb = lin2db(compgain / scaleddesiredgain);

        // calculate envelope rate based on whether we're attacking or releasing
        if (compdiffdb < 0.0f) { // compgain < scaleddesiredgain, so we're releasing
            compdiffdb = fixf(compdiffdb, -1.0f);
            maxcompdiffdb = -1; // reset for a future attack mode
            // apply the adaptive release curve
            // scale compdiffdb between 0-3
            float x = (clampf(compdiffdb, -12.0f, 0.0f) + 12.0f) * 0.25f;
            float releasesamples = adaptivereleasecurve(x, a, b, c, d);
            enveloperate = db2lin(SF_COMPRESSOR_SPACINGDB / releasesamples);
        }
        else { // compresorgain > scaleddesiredgain, so we're attacking
            compdiffdb = fixf(compdiffdb, 1.0f);
            if (maxcompdiffdb == -1 || maxcompdiffdb < compdiffdb) {
                maxcompdiffdb = compdiffdb;
            }
            float attenuate = maxcompdiffdb;
            if (attenuate < 0.5f) {
                attenuate = 0.5f;
            }
            enveloperate = 1.0f - pow(0.25f / attenuate, attacksamplesinv);
        }
        // process the chunk
        for (int chi = 0; chi < samplesperchunk; chi++) {
            perSampleProcessing(buffer, lReadWritePointer, rReadWritePointer);
            samplepos++;
            delayreadpos++;
            delaywritepos++;
        }
    }
    // process any remaining samples that dont fit in a chunk
    for (int chi = 0; chi < remainder; chi++) {
        perSampleProcessing(buffer, lReadWritePointer, rReadWritePointer);
        samplepos++;
        delayreadpos++;
        delaywritepos++;
    }
}

void Compressor::perSampleProcessing(juce::AudioBuffer<float>& buffer, float* lptr, float* rptr)
{
    while (delaywritepos >= delaybufsize)
    {
        delaywritepos -= delaybufsize;
    }
    while (delayreadpos >= delaybufsize)
    {
        delayreadpos -= delaybufsize;
    }

    float inputL = buffer.getSample(0, samplepos) * linearpregain; // these change from the original code due to reading from the JUCE buffer
    float inputR = buffer.getSample(1, samplepos) * linearpregain;

    delaybufL[delaywritepos] = inputL;
    delaybufR[delaywritepos] = inputR;

    inputL = absf(inputL);
    inputR = absf(inputR);
    float inputmax = inputL > inputR ? inputL : inputR;

    float attenuation;
    if (inputmax < 0.0001f) {
        attenuation = 1.0f;
    }
    else {
        float inputcomp = compcurve(inputmax, k, slope, linearthreshold,
            linearthresholdknee, threshold, knee, kneedboffset);
        attenuation = inputcomp / inputmax;
    }

    float rate;
    if (attenuation > detectoravg) { // if releasing
        float attenuationdb = -lin2db(attenuation);
        if (attenuationdb < 2.0f) {
            attenuationdb = 2.0f;
        }
        float dbpersample = attenuationdb * satreleasesamplesinv;
        rate = db2lin(dbpersample) - 1.0f;
    }
    else {
        rate = 1.0f;
    }

    detectoravg += (attenuation - detectoravg) * rate;
    if (detectoravg > 1.0f) {
        detectoravg = 1.0f;
    }
    detectoravg = fixf(detectoravg, 1.0f);

    if (enveloperate < 1) { // attack, reduce gain
        compgain += (scaleddesiredgain - compgain) * enveloperate;
    }
    else { // release, increase gain
        compgain *= enveloperate;
        if (compgain > 1.0f) {
            compgain = 1.0f;
        }
    }

    // the final gain value!
    float premixgain = sin(ang90 * compgain);
    float gain = dry + wet * mastergain * premixgain;

    // calculate metering (not used in core algo, but used to output a meter if desired)
    float premixgaindb = lin2db(premixgain);
    if (premixgaindb < metergain) {
        metergain = premixgaindb; // spike immediately
    }
    else {
        metergain += (premixgaindb - metergain) * meterrelease; // fall slowly
    }

    // apply the gain
    lptr[samplepos] = delaybufL[delayreadpos] * gain;
    rptr[samplepos] = delaybufR[delayreadpos] * gain;
}