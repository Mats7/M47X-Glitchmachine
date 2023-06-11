/*
  ==============================================================================

    CrusherProcessor.cpp
    Created: 27 Apr 2023 1:50:20pm
    Author:  matox

  ==============================================================================
*/

#include "ShifterProcessor.h"

ShifterProcessor::ShifterProcessor()
{
}

ShifterProcessor::~ShifterProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void ShifterProcessor::setInputBuffer(AudioBuffer<float>& newFileBuffer)
{
    inputBuffer.makeCopyOf(newFileBuffer);
    inputBufferSize = inputBuffer.getNumSamples();
}

/** Smooths out the end of the sample block
*/
AudioBuffer <float>& ShifterProcessor::smoothEndCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, 75, 1.0, 0.0);
    processedBuffer.applyGainRamp(1, index, 75, 1.0, 0.0);

    return processedBuffer;
}

/** Smooths out the start of the sample block
*/
AudioBuffer <float>& ShifterProcessor::smoothStartCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, 75, 0.0, 1.0);
    processedBuffer.applyGainRamp(1, index, 75, 0.0, 1.0);

    return processedBuffer;
}

/** Sets effect parameters
*/
void ShifterProcessor::setupShifter(float amount, float tone)
{
    cAmount = amount;
    cTone = tone;
}

/** Applies the effect to the signal
*/
AudioBuffer<float>& ShifterProcessor::addShifter(Atomic<bool> crusherEnabled)
{   
    if (crusherEnabled.get() == true)
    {
        outputBuffer.makeCopyOf(inputBuffer);
        for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            //divide buffer into even blocks
            for (int j = 0; j < roundFloatToInt(cAmount); j += 2)
            {
                int k = 0;

                //replace second half of the block by shifted version
                for (int i = roundFloatToInt((outputBuffer.getNumSamples() / cAmount) * (j + 1)); i < roundFloatToInt(  ((outputBuffer.getNumSamples() / cAmount) * (j + 1)) + (outputBuffer.getNumSamples() / cAmount / cTone) ); i++)
                {
                    outputBuffer.getArrayOfWritePointers()[channel][i] = outputBuffer.getArrayOfReadPointers()[channel][roundFloatToInt( ((outputBuffer.getNumSamples() / cAmount) * (j + 1)) + k * cTone)];
                    k++;
                }

                //smooth the starting and ending part of each of the halves
                outputBuffer = smoothEndCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cAmount) * (j + 1)) - 75);
                outputBuffer = smoothEndCutoff(outputBuffer, roundFloatToInt(((outputBuffer.getNumSamples() / cAmount) * (j + 1)) + (outputBuffer.getNumSamples() / cAmount / cTone) ) - 75);
                outputBuffer = smoothStartCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cAmount) * (j)));
                outputBuffer = smoothStartCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cAmount) * (j + 1)));

                if(roundFloatToInt(((outputBuffer.getNumSamples() / cAmount) * (j + 1)) + (outputBuffer.getNumSamples() / cAmount / cTone)) <= 75)
                    outputBuffer = smoothStartCutoff(outputBuffer, roundFloatToInt(((outputBuffer.getNumSamples() / cAmount) * (j + 1)) + (outputBuffer.getNumSamples() / cAmount / cTone)));
            }
        }
        return outputBuffer;
    }
    else
    {
        outputBuffer.setSize(1, 1);
        return inputBuffer;
    }
    
}
