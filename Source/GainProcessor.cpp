/*
  ==============================================================================

    GainProcessor.cpp

  ==============================================================================
*/

#include "GainProcessor.h"

GainProcessor::GainProcessor()
{
}

GainProcessor::~GainProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void GainProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
    inputBuffer.makeCopyOf(newFileBuffer);
    inputBufferSize = inputBuffer.getNumSamples();
}

/** Sets effect parameters
*/
void GainProcessor::setupGain(float gain)
{
    cGain = gain;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& GainProcessor::addGain(Atomic <bool> gainEnabled)
{
    if (gainEnabled.get() == true)
    {
        outputBuffer.makeCopyOf(inputBuffer);

        for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.applyGain(Decibels::decibelsToGain(cGain));
        }

        return outputBuffer;
    }
    else
    {
        outputBuffer.setSize(1, 1);
        return inputBuffer;
    } 
}