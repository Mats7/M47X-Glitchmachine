/*
  ==============================================================================

  ==============================================================================
*/

#include "FilterProcessor.h"

FilterProcessor::FilterProcessor()
{
	int c = 0;
}

FilterProcessor::~FilterProcessor()
{
}

/** Creates a copy of input effect buffer
*/
void FilterProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
	inputBuffer.makeCopyOf(newFileBuffer);
	inputBufferSize = inputBuffer.getNumSamples();
	
}

/** Sets LP effect parameters
*/
void FilterProcessor::setLPCutoffCurrentValue(float cutoffValue)
{
	lpcutoffCurrentValue = cutoffValue;
}

/** Sets HP effect parameters
*/
void FilterProcessor::setHPCutoffCurrentValue(float cutoffValue)
{
	hpcutoffCurrentValue = cutoffValue;
}

/** Sets BP effect parameters
*/
void FilterProcessor::setBPCutoffCurrentValue(float cutoffValue)
{
	bpcutoffCurrentValue = cutoffValue;
}

/** Sets LP effect parameters
*/
void FilterProcessor::setLPQualityCurrentValue(float qualityValue)
{
	lpqualityCurrentValue = qualityValue;
}

/** Sets HP effect parameters
*/
void FilterProcessor::setHPQualityCurrentValue(float qualityValue)
{
	hpqualityCurrentValue = qualityValue;
}

/** Sets BP effect parameters
*/
void FilterProcessor::setBPQualityCurrentValue(float qualityValue)
{
	bpqualityCurrentValue = qualityValue;
}
/** Sets LP effect parameters
*/
void FilterProcessor::setActiveFilters(Atomic <bool> lpfEn, Atomic <bool> hpfEn, Atomic <bool> bpfEn)
{
	lpfEnabled.set(lpfEn.get());
	hpfEnabled.set(hpfEn.get());
	bpfEnabled.set(bpfEn.get());
}

/** Calculates biquad coeffs for LP
*/
void FilterProcessor::calculateLPCoeffs()
{
	//qualityCurrentValue = 1.0;
	koef = tan(float_Pi * (lpcutoffCurrentValue / 44100));
	norm = 1 / (1 + koef / lpqualityCurrentValue + koef * koef);
	a0 = koef * koef * norm;
	a1 = 2 * a0;
	a2 = a0;
	b1 = 2 * (koef * koef - 1) * norm;
	b2 = (1 - koef / lpqualityCurrentValue + koef * koef) * norm;
}

/** Calculates biquad coeffs for HP
*/
void FilterProcessor::calculateHPCoeffs()
{
	//qualityCurrentValue = 1.0;
	koef = tan(float_Pi * (hpcutoffCurrentValue / 44100));
	norm = 1 / (1 + koef / hpqualityCurrentValue + koef * koef);
	a0 = 1 * norm;
	a1 = -2 * a0;
	a2 = a0;
	b1 = 2 * (koef * koef - 1) * norm;
	b2 = (1 - koef / hpqualityCurrentValue + koef * koef) * norm;
}

/** Calculates biquad coeffs for BP
*/
void FilterProcessor::calculateBPCoeffs()
{
	//qualityCurrentValue = 1.0;
	koef = tan(float_Pi * (bpcutoffCurrentValue / 44100));
	norm = 1 / (1 + koef / bpqualityCurrentValue + koef * koef);
	a0 = (koef / bpqualityCurrentValue) * norm;
	a1 = 0 * norm;
	a2 = (-koef / bpqualityCurrentValue) * norm;
	b1 = 2 * (koef * koef - 1) * norm;
	b2 = (1 - koef / bpqualityCurrentValue + koef * koef) * norm;
}

/** Processes input samples with biquad filter
*/
float FilterProcessor::processBiquad(float in)
{
	float out = in * a0 + z1;
	z1 = in * a1 + z2 - b1 * out;
	z2 = in * a2 - b2 * out;
	return out;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& FilterProcessor::addLowpassFilter()
{
	lpBuffer.makeCopyOf(inputBuffer);
	for (auto channel = 0; channel < lpBuffer.getNumChannels(); ++channel)
	{
		auto* outputData = lpBuffer.getWritePointer(channel);
		auto* inputData = lpBuffer.getReadPointer(channel);

		for (int sample = 0; sample < lpBuffer.getNumSamples(); sample++)
		{
			float inputSample = *inputData;
			calculateLPCoeffs();
			*outputData = processBiquad(inputSample);
			inputData++;
			outputData++;
		}
	}

	return lpBuffer;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& FilterProcessor::addHighpassFilter()
{
	if(lpfEnabled.get() == false)
		hpBuffer.makeCopyOf(inputBuffer);
	else
		hpBuffer.makeCopyOf(lpBuffer);

	for (auto channel = 0; channel < hpBuffer.getNumChannels(); ++channel)
	{
		auto* outputData = hpBuffer.getWritePointer(channel);
		auto* inputData = hpBuffer.getReadPointer(channel);

		for (int sample = 0; sample < hpBuffer.getNumSamples(); ++sample)
		{
			float inputSample = *inputData;
			calculateHPCoeffs();
			*outputData = processBiquad(inputSample);
			inputData++;
			outputData++;
		}
	}

	return hpBuffer;
}

/** Applies the effect to the signal
*/
AudioBuffer <float>& FilterProcessor::addBandpassFilter()
{
	if(lpfEnabled.get() == false && hpfEnabled.get() == false)
		bpBuffer.makeCopyOf(inputBuffer);
	else if(lpfEnabled.get() == true && hpfEnabled.get() == false)
		bpBuffer.makeCopyOf(lpBuffer);
	else
		bpBuffer.makeCopyOf(hpBuffer);

	for (auto channel = 0; channel < bpBuffer.getNumChannels(); ++channel)
	{
		auto* outputData = bpBuffer.getWritePointer(channel);
		auto* inputData = bpBuffer.getReadPointer(channel);

		for (int sample = 0; sample < bpBuffer.getNumSamples(); sample++)
		{
			float inputSample = *inputData;
			calculateBPCoeffs();
			*outputData = processBiquad(inputSample);
			inputData++;
			outputData++;
		}
	}

	return bpBuffer;
}

/** Calls LP/HP/BP filters by their enabled/disabled state
*/
AudioBuffer <float>& FilterProcessor::addFilters()
{
	if (lpfEnabled.get() == false && hpfEnabled.get() == true && bpfEnabled.get() == true)
	{
		outputBuffer.setSize(1, 1);
		return inputBuffer;
	}
	else
	{
		outputBuffer.makeCopyOf(inputBuffer);

		if (lpfEnabled.get() == true)
		{
			addLowpassFilter();
			outputBuffer.makeCopyOf(lpBuffer);
		}
		if (hpfEnabled.get() == true)
		{
			addHighpassFilter();
			outputBuffer.makeCopyOf(hpBuffer);
		}
		if (bpfEnabled.get() == true)
		{
			addBandpassFilter();
			outputBuffer.makeCopyOf(bpBuffer);
		}

		return outputBuffer;
	}
}

/** Calculates magnitude response for biquad LP filter
*/
float FilterProcessor::magnitudeResponseLP(float fc)
{
	float k = tan(float_Pi * (lpcutoffCurrentValue / 44100));
	float nnnn = 1 / (1 + k / lpqualityCurrentValue + k * k);
	float a00 = k * k * nnnn;
	float a11 = 2 * a00;
	float a22 = a00;
	float b11 = 2 * (k * k - 1) * nnnn;
	float b22 = (1 - k / lpqualityCurrentValue + k * k) * nnnn;


	auto w0 = 2 * float_Pi*(fc / 44100.0);
	auto const cosw = std::cos(w0);
	auto const cos2w = std::cos(2 * w0);
	auto const sinw = std::sin(w0);

	auto square = [](auto z) { return z * z; };

	auto const numerator = sqrt(square(a00) + square(a11) + square(a22) + 2 * (a00 * a11 + a11 * a22) * cosw + 2 * a00 * a22 * cos2w);
	auto const denominator = sqrt(1 + square(b11) + square(b22) + 2 * (b11 + b11 * b22) * cosw + 2 * b22 * cos2w);

	return numerator / denominator;
}

/** Calculates magnitude response for biquad LP filter
*/
float FilterProcessor::magnitudeResponseHP(float fc)
{
	float k = tan(float_Pi * (hpcutoffCurrentValue / 44100));
	float nnnn = 1 / (1 + k / hpqualityCurrentValue + k * k);
	float a00 = 1 * nnnn;
	float a11 = -2 * a00;
	float a22 = a00;
	float b11 = 2 * (k * k - 1) * nnnn;
	float b22 = (1 - k / hpqualityCurrentValue + k * k) * nnnn;


	auto w0 = 2 * float_Pi * (fc / 44100.0);
	auto const piw0 = w0 * juce::MathConstants<float>::pi;
	auto const cosw = std::cos(w0);
	auto const cos2w = std::cos(2 * w0);
	auto const sinw = std::sin(w0);

	auto square = [](auto z) { return z * z; };

	auto const numerator = sqrt(square(a00) + square(a11) + square(a22) + 2 * (a00 * a11 + a11 * a22) * cosw + 2 * a00 * a22 * cos2w);
	auto const denominator = sqrt(1 + square(b11) + square(b22) + 2 * (b11 + b11 * b22) * cosw + 2 * b22 * cos2w);

	return numerator / denominator;
}

/** Calculates magnitude response for biquad BP filter
*/
float FilterProcessor::magnitudeResponseBP(float fc)
{
	float k = tan(float_Pi * (bpcutoffCurrentValue / 44100));
	float nnnn = 1 / (1 + k / bpqualityCurrentValue + k * k);
	float a00 = (k / bpqualityCurrentValue) * nnnn;
	float a11 = 0 * nnnn;
	float a22 = (-k / bpqualityCurrentValue) * nnnn;
	float b11 = 2 * (k * k - 1) * nnnn;
	float b22 = (1 - k / bpqualityCurrentValue + k * k) * nnnn;


	auto w0 = 2 * float_Pi * (fc / 44100.0);
	auto const piw0 = w0 * juce::MathConstants<float>::pi;
	auto const cosw = std::cos(w0);
	auto const cos2w = std::cos(2 * w0);
	auto const sinw = std::sin(w0);

	auto square = [](auto z) { return z * z; };

	auto const numerator = sqrt(square(a00) + square(a11) + square(a22) + 2 * (a00 * a11 + a11 * a22) * cosw + 2 * a00 * a22 * cos2w);
	auto const denominator = sqrt(1 + square(b11) + square(b22) + 2 * (b11 + b11 * b22) * cosw + 2 * b22 * cos2w);

	return numerator / denominator;
}

