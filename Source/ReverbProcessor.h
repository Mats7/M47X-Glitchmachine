#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <deque>

//==============================================================================
class ReverbProcessor   
{
public:
    ReverbProcessor();
    ~ReverbProcessor();

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);
	void setupReverb(float damp, float balance, float size, float widthr);
	AudioBuffer <float>& addReverb(Atomic <bool> reverbEnabled);


private:
	Reverb myReverb;
	Reverb::Parameters reverbParams;

	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;

	int inputBufferSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbProcessor)
};
