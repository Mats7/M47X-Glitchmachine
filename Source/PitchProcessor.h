/*
  ==============================================================================

    PitchProcessor.h

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class PitchProcessor
{
public:
	PitchProcessor();
	~PitchProcessor();

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);
	void setupPitch(float pitch);
	AudioBuffer <float>& addPitch(Atomic <bool> pitchEnabled);


private:

	float cPitch;
	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;

	int inputBufferSize;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchProcessor)
};
