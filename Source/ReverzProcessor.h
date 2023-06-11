/*
  ==============================================================================

    ShifterProcessor.h

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class ReverzProcessor
{
public:
	ReverzProcessor();
	~ReverzProcessor();

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);
	AudioBuffer<float>& smoothEndCutoff(AudioBuffer<float>& processedBuffer, int index);
	AudioBuffer<float>& smoothStartCutoff(AudioBuffer<float>& processedBuffer, int index);
	void setupReverz(float amount, float color);
	AudioBuffer <float>& addReverz(Atomic <bool> reverzEnabled);


private:
	float cAmount;
	int cSkew;
	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;

	float randomFloat();

	int inputBufferSize;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverzProcessor)
};
