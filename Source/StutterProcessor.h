/*
  ==============================================================================

    DegraderProcessor.h
    Created: 3 Mar 2023 6:26:05pm
    Author:  matox

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class StutterProcessor
{
public:
	StutterProcessor();
	~StutterProcessor();

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);
	void setupStutter(float mix, float chorusAmount, float delay);
	AudioBuffer<float>& smoothEndCutoff(AudioBuffer<float>& processedBuffer, int index);
	AudioBuffer<float>& smoothStartCutoff(AudioBuffer<float>& processedBuffer, int index);
	AudioBuffer <float>& addStutter(Atomic <bool> stutterEnabled);

	SmoothedValue <float> smoother;

private:
	float cMix;
	int cColor;
	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;
	AudioBuffer <float> tempBuffer;

	int inputBufferSize;

	dsp::Chorus<float> chorus;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StutterProcessor)
};