/*
  ==============================================================================

    FFTProcessor.cpp

  ==============================================================================
*/

#include "FFTProcessor.h"

FFTProcessor::FFTProcessor() : forwardFFT(fftOrder),
window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{

}

FFTProcessor::~FFTProcessor()
{

}

/** Pushes the sample from buffer in getNextAudioBlock()
* Sample is stored in fifo[] array
*/
void FFTProcessor::pushNextSampleIntoFifo(float sample) noexcept
{
	if (fifoIndex == fftSize)
	{
		if (!nextFFTBlockReady)
		{
			juce::zeromem(fftData, sizeof(fftData));
			memcpy(fftData, fifo, sizeof(fifo));
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[fifoIndex++] = sample;
}

/** Computes coordinates to draw a frame of frequency spectrum
* Uses log_e distribution for frequencies
* spectralResolutionCoef changes distribution (lower value for better low freq resolution, higher value for high freq resolution)
*/
void FFTProcessor::drawNextFrameOfSpectrum(float spectralResolutionCoef)
{
	window.multiplyWithWindowingTable(fftData, fftSize);

	forwardFFT.performFrequencyOnlyForwardTransform(fftData);

	auto mindB = -100.0f;
	auto maxdB = 3.01f;

	for (int i = 0; i < scopeSize; ++i)
	{
		//auto skewedProportionX = (float)i / (float)scopeSize;
		auto skewedProportionX = -1 * spectralResolutionCoef * std::log(1.0f - (float)i / (float)scopeSize);
		auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
		auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
			- juce::Decibels::gainToDecibels((float)fftSize)),
			mindB, maxdB, 0.0f, 1.0f);

		scopeData[i] = level;
	}
}

/** Draws a line from coordinates computed in drawNextFrameOfSpectrum()
* Uses x and y boundaries of spectral window to determine min and max xy coordinates
* @param g - graphical context to use for drawing
*/
void FFTProcessor::drawFrame(juce::Graphics& g)
{
	for (int i = 1; i < scopeSize; ++i)
	{
		g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 100, 1030),
							  juce::jmap(scopeData[i - 1], 0.0f, 1.0f, 409.0f, 282.0f),
					  (float)juce::jmap(i,     0, scopeSize - 1, 100, 1030),
							  juce::jmap(scopeData[i],     0.0f, 1.0f, 409.0f, 282.0f) });
	}
}