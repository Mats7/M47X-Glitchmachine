#include "ExtractorProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

ExtractorProcessor::ExtractorProcessor()
{
    smoothCutoffWidth = 100;
}

ExtractorProcessor::~ExtractorProcessor()
{

}

/** Creates a copy of input effect buffer
*/
void ExtractorProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
    inputBuffer.makeCopyOf(newFileBuffer);
    inputBufferSize = inputBuffer.getNumSamples();

}

/** Sets effect parameters
*/
void ExtractorProcessor::setupExtractor(int extIntensity, int extWidth)
{
    extractorIntensity = extIntensity;
    extractorWidth = extWidth*500;
    signChanged.set(false);
}

/** Smooths out the start of the sample block
*/
AudioBuffer <float>& ExtractorProcessor::smoothStartCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, smoothCutoffWidth, 1.0, 0.0);
    processedBuffer.applyGainRamp(1, index, smoothCutoffWidth, 1.0, 0.0);

    return processedBuffer;
}

/** Smooths out the end of the sample block
*/
AudioBuffer <float>& ExtractorProcessor::smoothEndCutoff(AudioBuffer <float>& processedBuffer, int index)
{
    processedBuffer.applyGainRamp(0, index, smoothCutoffWidth, 0.0, 1.0);
    processedBuffer.applyGainRamp(1, index, smoothCutoffWidth, 0.0, 1.0);

    return processedBuffer;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& ExtractorProcessor::addExtractor(Atomic <bool> extractorEnabled)
{
    if (extractorEnabled.get() == true)
    {
        outputBuffer.makeCopyOf(inputBuffer);

        signalSize = outputBuffer.getNumSamples();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(0, signalSize);

        int cntr = 0;
        float signBackup = 0.0;

        for (int i = 0; i < extractorIntensity; i++)
        {
            //generate random index of sample
            unsigned long int randIndex = distribution(gen);

            //random index + smoothing width should be lower than num of total samples
            if (randIndex + smoothCutoffWidth < signalSize)
            {
                outputBuffer = smoothStartCutoff(outputBuffer, randIndex);
            }

            //set samples value to 0 from generated index to index+width
            for (int j = smoothCutoffWidth; j < extractorWidth; j++)
            {
                //random index + number of samples for deletion should be lower than num of total samples
                if (randIndex + j + extractorWidth < signalSize - 1)
                {
                    //deletion of samples (else is for the last smoothCutoffWidth num of samples to smooth out)
                    if (randIndex + j < randIndex + extractorWidth - 100)
                    {
                        outputBuffer.getArrayOfWritePointers()[0][randIndex + j] = 0;
                        outputBuffer.getArrayOfWritePointers()[1][randIndex + j] = 0;
                    }
                    else
                    {
                        outputBuffer = smoothEndCutoff(outputBuffer, randIndex + j);
                    }    
                }
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