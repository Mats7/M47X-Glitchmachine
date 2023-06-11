/*
  ==============================================================================

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class FilterProcessor
{
public:
    FilterProcessor();
    ~FilterProcessor();
    void setInputBuffer(AudioBuffer <float>& newFileBuffer);
    void setLPCutoffCurrentValue(float cutoffValue);
    void setHPCutoffCurrentValue(float cutoffValue);
    void setBPCutoffCurrentValue(float cutoffValue);
    void setLPQualityCurrentValue(float qualityValue);
    void setHPQualityCurrentValue(float qualityValue);
    void setBPQualityCurrentValue(float qualityValue);
    void setActiveFilters(Atomic <bool> lpfEn, Atomic <bool> hpfEn, Atomic <bool> bpfEn);
    void calculateLPCoeffs();
    void calculateHPCoeffs();
    void calculateBPCoeffs();
    AudioBuffer <float>& addLowpassFilter();
    AudioBuffer <float>& addHighpassFilter();
    AudioBuffer <float>& addBandpassFilter();
    AudioBuffer <float>& addFilters();
    float processBiquad(float in);
    float magnitudeResponseLP(float w0);
    float magnitudeResponseHP(float w0);
    float magnitudeResponseBP(float w0);

private:
    AudioBuffer <float> inputBuffer;
    AudioBuffer <float> outputBuffer;
    AudioBuffer <float> lpBuffer;
    AudioBuffer <float> hpBuffer;
    AudioBuffer <float> bpBuffer;
    int inputBufferSize;

    float a0;
    float a1;
    float a2;
    float b0;
    float b1;
    float b2;
    float b3;
    float vvv;
    float koef;
    float lpcutoffCurrentValue;
    float hpcutoffCurrentValue;
    float bpcutoffCurrentValue;
    float lpqualityCurrentValue;
    float hpqualityCurrentValue;
    float bpqualityCurrentValue;
    float gain;
    float norm;
    float z1, z2;

    Atomic <bool> lpfEnabled;
    Atomic <bool> hpfEnabled;
    Atomic <bool> bpfEnabled;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterProcessor)
};