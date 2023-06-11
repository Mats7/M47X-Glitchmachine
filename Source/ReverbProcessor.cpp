#include "../JuceLibraryCode/JuceHeader.h"
#include "ReverbProcessor.h"
#include <deque>

//==============================================================================
ReverbProcessor::ReverbProcessor()
{
	
}


ReverbProcessor::~ReverbProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void ReverbProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
	inputBuffer.makeCopyOf(newFileBuffer);
	inputBufferSize = inputBuffer.getNumSamples();

}

/** Sets effect parameters and initializes Reverb instance
*/
void ReverbProcessor::setupReverb(float damp, float balance, float size, float widthr)
{
	reverbParams.damping = damp;
	reverbParams.dryLevel = 1.0 - balance;
	reverbParams.roomSize = size;
	reverbParams.wetLevel = balance;
	reverbParams.width = widthr;
	myReverb.setParameters(reverbParams);
	myReverb.setSampleRate(44100.0);
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& ReverbProcessor::addReverb(Atomic <bool> reverbEnabled)
{
	if (reverbEnabled.get() == true)
	{
		outputBuffer.makeCopyOf(inputBuffer);
		myReverb.processStereo(outputBuffer.getArrayOfWritePointers()[0], outputBuffer.getArrayOfWritePointers()[1], outputBuffer.getNumSamples());
		return outputBuffer;
	}
	else 
	{
		outputBuffer.setSize(1, 1);
		return inputBuffer;
	}
}