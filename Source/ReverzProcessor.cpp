/*
  ==============================================================================

    ShifterProcessor.cpp
    Created: 20 Feb 2023 12:52:17pm
    Author:  matox

  ==============================================================================
*/

#include "ReverzProcessor.h"


ReverzProcessor::ReverzProcessor()
{
    srand(time(0));
}


ReverzProcessor::~ReverzProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void ReverzProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
    inputBuffer.makeCopyOf(newFileBuffer);
    inputBufferSize = inputBuffer.getNumSamples();
}

/** Smoothing function for jumps in the signal values
*/
AudioBuffer <float>& ReverzProcessor::smoothStartCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, 99, 0.0, 1.0);
    processedBuffer.applyGainRamp(1, index, 99, 0.0, 1.0);

    return processedBuffer;
}

/** Smoothing function for jumps in the signal values
*/
AudioBuffer <float>& ReverzProcessor::smoothEndCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, 99, 1.0, 0.0);
    processedBuffer.applyGainRamp(1, index, 99, 1.0, 0.0);

    return processedBuffer;
}

/** Sets effect parameters
*/
void ReverzProcessor::setupReverz(float skew, float amount)
{
    cSkew = roundFloatToInt(skew);
    cAmount = amount*2.0f;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& ReverzProcessor::addReverz(Atomic <bool> shiftEnabled)
{ 
    if (shiftEnabled.get() == true)
    {
        outputBuffer.makeCopyOf(inputBuffer);
        //outputBuffer.setSize(inputBuffer.getNumChannels(), inputBuffer.getNumSamples() * 4, true, false, true);
        for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {   
            //divide the signal into evenly distributed parts and reverse the samples in second half in every of the parts
            for (int i = outputBuffer.getNumSamples(); i > roundFloatToInt((float)outputBuffer.getNumSamples() / cAmount) - 1; i -= roundFloatToInt((float)outputBuffer.getNumSamples() / cAmount))
            { 
                //sample array reverse function (flips samples, starting with the first and the last sample)
                int start = (i - (outputBuffer.getNumSamples() / cAmount / 2.0f)) - ((outputBuffer.getNumSamples() / cAmount / 20.0f) * cSkew);
                int end = i;

                while (start < end)
                {
                    float temp = outputBuffer.getArrayOfReadPointers()[channel][start];
                    outputBuffer.getArrayOfWritePointers()[channel][start] = outputBuffer.getArrayOfReadPointers()[channel][end];
                    outputBuffer.getArrayOfWritePointers()[channel][end] = temp;
                    start++;
                    end--;
                }

                //dont try to smooth out before the end of the buffer
                if ((i - (outputBuffer.getNumSamples() / cAmount / 2.0f)) - ((outputBuffer.getNumSamples() / cAmount / 20.0f) * cSkew) >= 99)
                    outputBuffer = smoothEndCutoff(outputBuffer, (i - (outputBuffer.getNumSamples() / cAmount / 2.0f)) - ((outputBuffer.getNumSamples() / cAmount / 20.0f) * cSkew) - 99);
                
                //can happen at the end of the buffer with low skew values and low sample counts
                if ((i - (outputBuffer.getNumSamples() / cAmount / 2.0f)) - ((outputBuffer.getNumSamples() / cAmount / 20.0f) * cSkew) <= 99)
                    outputBuffer = smoothStartCutoff(outputBuffer, (i - (outputBuffer.getNumSamples() / cAmount / 2.0f)) - ((outputBuffer.getNumSamples() / cAmount / 20.0f) * cSkew));
                
                outputBuffer = smoothEndCutoff(outputBuffer, i - 99);
                
                //dont try to smooth out after the end of the buffer
                if (i != outputBuffer.getNumSamples())
                    outputBuffer = smoothStartCutoff(outputBuffer, i);
                
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


float ReverzProcessor::randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}
