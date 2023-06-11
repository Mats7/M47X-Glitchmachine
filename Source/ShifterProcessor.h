/*

*/
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class ShifterProcessor
{
public:
	ShifterProcessor();
	~ShifterProcessor();

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);
	void setupShifter(float amount, float size);
	AudioBuffer<float>& smoothEndCutoff(AudioBuffer<float>& processedBuffer, int index);
	AudioBuffer<float>& smoothStartCutoff(AudioBuffer<float>& processedBuffer, int index);
	AudioBuffer <float>& addShifter(Atomic <bool> shifterEnabled);

	SmoothedValue <float> smoother;


private:

	float cAmount;
	float cTone;
	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;
	AudioBuffer <float> tempBuffer;

	int inputBufferSize;





	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShifterProcessor)
};