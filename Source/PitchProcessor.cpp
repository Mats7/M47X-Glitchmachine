/*
  ==============================================================================

    PitchProcessor.cpp

  ==============================================================================
*/

#include "PitchProcessor.h"
PitchProcessor::PitchProcessor()
{

}


PitchProcessor::~PitchProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void PitchProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
    inputBuffer.makeCopyOf(newFileBuffer);
    inputBufferSize = inputBuffer.getNumSamples();
}

/** Sets effect parameters
*/
void PitchProcessor::setupPitch(float pitch)
{
    cPitch = pitch;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& PitchProcessor::addPitch(Atomic <bool> pitchEnabled)
{
    if (pitchEnabled.get() == true)
    {
        outputBuffer.makeCopyOf(inputBuffer);

        if (cPitch < 1.0)
        {
            outputBuffer.setSize(outputBuffer.getNumChannels(), roundFloatToInt(outputBuffer.getNumSamples() * 1/cPitch) + 1, true);
        }
        
        for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            if (cPitch > 1.0)
            {
                for (int i = 0; i < roundFloatToInt(outputBuffer.getNumSamples() / cPitch); i++)
                {
                    outputBuffer.getArrayOfWritePointers()[channel][i] = outputBuffer.getArrayOfWritePointers()[channel][roundFloatToInt(i * cPitch)];
                }
            }
            else if (cPitch < 1.0)
            {
                for (int i = outputBuffer.getNumSamples(); i > 10; i--)
                {
                    outputBuffer.getArrayOfWritePointers()[channel][i] = outputBuffer.getArrayOfWritePointers()[channel][roundFloatToInt(i * cPitch)];
                }
            } 
        }

        if (cPitch > 1.0) 
        {
            outputBuffer.setSize(outputBuffer.getNumChannels(), outputBuffer.getNumSamples() / cPitch, true);
        }
        
        return outputBuffer;
    }
    else
    {
        outputBuffer.setSize(1, 1);
        return inputBuffer;
    }

    
}