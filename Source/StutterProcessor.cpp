/*
  ==============================================================================

    DegraderProcessor.cpp
    Created: 3 Mar 2023 6:26:05pm
    Author:  matox

  ==============================================================================
*/

#include "StutterProcessor.h"

StutterProcessor::StutterProcessor()
{

}


StutterProcessor::~StutterProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void StutterProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
    inputBuffer.makeCopyOf(newFileBuffer);
    inputBufferSize = inputBuffer.getNumSamples();
}

/** Set up stutter variables and chorus processor
* cMix - stutter frequency
* chorusAmount - is used to set all of chorus variables
*/
void StutterProcessor::setupStutter(float mix, float chorusAmount, float delay)
{
    cMix = mix;
    chorus.setDepth(chorusAmount / 20.0f);
    chorus.setFeedback(chorusAmount / -20.0f);
    chorus.setRate(0.0f);
    chorus.setMix(chorusAmount / 20.0f);
    chorus.setCentreDelay(delay);
    dsp::ProcessSpec chorusSpec;
    chorusSpec.numChannels = 2;
    chorusSpec.maximumBlockSize = inputBufferSize;
    chorusSpec.sampleRate = 44100.0;
    chorus.prepare(chorusSpec);

    tempBuffer.setSize(inputBuffer.getNumChannels(), inputBufferSize, false, true, false);
}

/** Smooths out the end of the sample block
*/
AudioBuffer <float>& StutterProcessor::smoothEndCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, 50, 1.0, 0.0);
    processedBuffer.applyGainRamp(1, index, 50, 1.0, 0.0);

    return processedBuffer;
}

/** Smooths out the start of the sample block
*/
AudioBuffer <float>& StutterProcessor::smoothStartCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, 50, 0.0, 1.0);
    processedBuffer.applyGainRamp(1, index, 50, 0.0, 1.0);

    return processedBuffer;
}

/** Applies stutter effect
* stutterEnabled - bypasses the processing when false (effect just sends inputBuffer to its output)
*/
AudioBuffer <float>& StutterProcessor::addStutter(Atomic <bool> stutterEnabled)
{
    if (stutterEnabled.get() == true)
    {
        outputBuffer.makeCopyOf(inputBuffer);
        for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            //divide buffer into even blocks
            for (int j = 0; j < roundFloatToInt(cMix); j+=2)
            {
                //save backup of the first half of the block
                for (int i = roundFloatToInt((outputBuffer.getNumSamples() / cMix) * j); i < roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j + 1)); i++)
                {
                    tempBuffer.getArrayOfWritePointers()[channel][i] = outputBuffer.getArrayOfReadPointers()[channel][i]; 
                }
                
                //replace second half of the block by backup
                for (int i = roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j + 1)); i < roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j + 2)); i++)
                {
                    outputBuffer.getArrayOfWritePointers()[channel][i] = tempBuffer.getArrayOfReadPointers()[channel][i - roundFloatToInt(outputBuffer.getNumSamples() / cMix)];

                }

                //smooth the starting and ending part of each of the halves
                outputBuffer = smoothEndCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j + 1)) - 50);
                outputBuffer = smoothEndCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j + 2)) - 50);
                outputBuffer = smoothStartCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j)));
                outputBuffer = smoothStartCutoff(outputBuffer, roundFloatToInt((outputBuffer.getNumSamples() / cMix) * (j+1)));
            }     
        }

        //add simple chorus effect
        chorus.process(dsp::ProcessContextReplacing<float>(dsp::AudioBlock<float>(outputBuffer)));
        
        return outputBuffer;
    }
    else
    {
        outputBuffer.setSize(1, 1);
        return inputBuffer;
    }

    
}