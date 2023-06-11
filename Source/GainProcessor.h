/*
  ==============================================================================

    GainProcessor.h

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class GainProcessor
{
public:
	GainProcessor();
	~GainProcessor();

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);
	void setupGain(float gain);
	AudioBuffer <float>& addGain(Atomic <bool> gainEnabled);


private:

	float cGain;
	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;

	//dsp::Gain<float> myGain;

	int inputBufferSize;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainProcessor)
};