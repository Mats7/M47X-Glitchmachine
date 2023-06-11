/*
  ==============================================================================

    FFTProcessor.h
    Created: 28 Apr 2023 1:05:42pm
    Author:  matox

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class FFTProcessor {
public:
	FFTProcessor();
	~FFTProcessor();

	enum fftVariables
	{
		fftOrder = 12,
		fftSize = 1 << fftOrder,
		scopeSize = 512,
	};


	float fifo[fftSize];
	float fftData[2 * fftSize];
	int fifoIndex = 0;
	bool nextFFTBlockReady = false;
	float scopeData[scopeSize];

	void pushNextSampleIntoFifo(float sample) noexcept;

	void drawNextFrameOfSpectrum(float spectralResolutionCoef);

	void drawFrame(juce::Graphics& g);

private:

	juce::dsp::FFT forwardFFT;
	juce::dsp::WindowingFunction<float> window;

	

};