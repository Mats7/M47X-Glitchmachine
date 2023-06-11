#include "DistortionProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

DistortionProcessor::DistortionProcessor()										
{

}

DistortionProcessor::~DistortionProcessor()
{

}

/** Sets softclip effect parameters
*/
void DistortionProcessor::setSoftclipThresholdValue(float ThresholdValue)
{
	softclipThresholdValue = ThresholdValue;
}

/** Sets hardclip effect parameters
*/
void DistortionProcessor::setHardclipThresholdValue(float ThresholdValue)
{
	hardclipThresholdValue = ThresholdValue;
}

/** Creates a copy of input effect buffer
*/
void DistortionProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
	inputBuffer.makeCopyOf(newFileBuffer);
	inputBufferSize = inputBuffer.getNumSamples();
}

/** Applies softclip effect to the signal
*/
AudioBuffer <float>& DistortionProcessor::addSoftclipDistortion(Atomic <bool> scdEnabled)
{
	if (scdEnabled.get() == true)
	{
		outputBuffer.makeCopyOf(inputBuffer);

		for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
		{
			auto* channelData = outputBuffer.getWritePointer(channel);

			for (int sample = 0; sample < outputBuffer.getNumSamples(); sample++)
			{
				if (*channelData < 0.000f)
				{
					if (*channelData < -0.666f)
						*channelData = -1.000f;
					else
						*channelData = -1 * ((1.88079f) * softclipThresholdValue * (std::powf(-1 * *channelData, 1.0f) - std::powf(-1 * *channelData, 4.93917f)));
				}
				else if(*channelData > 0.000f)
				{
					if (*channelData > 0.666f)
						*channelData = 1.000f;
					else
						*channelData = (1.88079f) * softclipThresholdValue * (std::powf(*channelData, 1.0f) - std::powf(*channelData, 4.93917f));
				}
				channelData++;
			}
		}
		return outputBuffer;;
	}
	else
	{
		outputBuffer.setSize(1,1);
		return inputBuffer;
	}
}

/** Applies hardclip effect to the signal
*/
AudioBuffer <float>& DistortionProcessor::addHardclipDistortion(Atomic <bool> hcdEnabled)
{
	if (hcdEnabled.get() == true)
	{
		outputBuffer.makeCopyOf(inputBuffer);

		float thresholdP = hardclipThresholdValue;
		float thresholdN = -(hardclipThresholdValue);

		for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
		{
			auto* channelData = outputBuffer.getWritePointer(channel);

			for (int sample = 0; sample < outputBuffer.getNumSamples(); sample++)
			{
				if (*channelData > thresholdP)
				{
					*channelData = thresholdP;
				}
				else if (*channelData < thresholdN)
				{
					*channelData = thresholdN;
				}
				channelData++;
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

/** Applies fullrect effect to the signal
*/
AudioBuffer <float>& DistortionProcessor::addFullrectDistortion(Atomic <bool> frdEnabled)
{
	if (frdEnabled.get() == true)
	{
		outputBuffer.makeCopyOf(inputBuffer);

		for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
		{
			auto* channelData = outputBuffer.getWritePointer(channel);

			for (int sample = 0; sample < outputBuffer.getNumSamples(); sample++)
			{
				if (*channelData < 0)
				{
					*channelData *= -1;
				}
				channelData++;
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

/** Applies halfrect effect to the signal
*/
AudioBuffer <float>& DistortionProcessor::addHalfrectDistortion(Atomic <bool> hrdEnabled)
{
	if (hrdEnabled.get() == true)
	{
		outputBuffer.makeCopyOf(inputBuffer);

		for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
		{
			auto* channelData = outputBuffer.getWritePointer(channel);

			for (int sample = 0; sample < outputBuffer.getNumSamples(); sample++)
			{
				if (*channelData < 0)
				{
					*channelData = 0;
				}
				channelData++;
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