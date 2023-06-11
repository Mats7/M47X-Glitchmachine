#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DistortionProcessor
{
public:
    DistortionProcessor();
    ~DistortionProcessor();
    void setSoftclipThresholdValue(float ThresholdValue);
    void setHardclipThresholdValue(float ThresholdValue);
    void setSofclipAmountValue(float amountValue);
    void setInputBuffer(AudioBuffer <float>& newFileBuffer);
    AudioBuffer <float>& addSoftclipDistortion(Atomic <bool> scdEnabled);
    AudioBuffer <float>& addHardclipDistortion(Atomic <bool> hcdEnabled);
    AudioBuffer <float>& addFullrectDistortion(Atomic <bool> frdEnabled);
    AudioBuffer <float>& addHalfrectDistortion(Atomic <bool> hrdEnabled);
private:
    float hardclipThresholdValue;
    float softclipThresholdValue;
    //  Input and output buffers 
    AudioBuffer <float> inputBuffer;
    AudioBuffer <float> outputBuffer;
    int inputBufferSize;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionProcessor)
};