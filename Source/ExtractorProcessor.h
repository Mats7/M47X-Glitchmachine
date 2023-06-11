#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include<random>

class ExtractorProcessor
{
public:
    ExtractorProcessor();
    ~ExtractorProcessor();
    void setInputBuffer(AudioBuffer <float>& newFileBuffer);
    void setupExtractor(int extIntensity, int extWidth);
    AudioBuffer <float>& smoothStartCutoff(AudioBuffer <float>& processedBuffer, int index);
    AudioBuffer<float>& smoothEndCutoff(AudioBuffer<float>& processedBuffer, int index);
    AudioBuffer <float>& addExtractor(Atomic <bool> extractorEnabled);
private:
    AudioBuffer <float> inputBuffer;
    AudioBuffer <float> outputBuffer;
    int inputBufferSize;
    int signalSize;
    int extractorIntensity;  //number of sections to delete
    int extractorWidth; //size of sections to delete (in samples)
    int smoothCutoffWidth;
    Atomic <bool> signChanged;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtractorProcessor)
};