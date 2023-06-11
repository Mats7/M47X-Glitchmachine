#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ReverbProcessor.h"
#include "DistortionProcessor.h"
#include "FilterProcessor.h"
#include "ExtractorProcessor.h"
#include "CustomLookAndFeel.h"
#include "CustomSlider.h"
#include "GainProcessor.h"
#include "ReverzProcessor.h"
#include "StutterProcessor.h"
#include "ShifterProcessor.h"
#include "PitchProcessor.h"
#include "FFTProcessor.h"
/*#include "ProcessingChain.h"*/

//==============================================================================

class MainComponent   : public AudioAppComponent,
	                    public ChangeListener,
	                    private Timer, public FileDragAndDropTarget
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
	void drawEffectBounds(Graphics& g);
	void drawFreqMarks(Graphics& g);
	void drawFilterResponse(Graphics &g);
	void drawFreqIndicator(Graphics& g);
	void setFreqIndicatorPosition(void);
	void setPlaybackPosition(void);
    void resized() override;

	void processAllEffects(int effectIndex);

	int position;
	int positionShiftLength = 0;

	int freqPosition;

	int fbNumSamplesTemp;

	const int spWidth = 930;
	const int spPosY = 240;
	const int spPosY2 = 410;

	float zoomFactor = 0.0f;
	float zoomPositionSeconds = 0.0f;

	float mouseClickXCord;
	float mouseClickYCord;

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

	const float freqDashLength[2] = { 5, 5 };


private:
    //==============================================================================
	CustomLookAndFeel customLookAndFeel;
	//CustomSlider customSlider;

	//file and playback controls
	Label fileOpenSaveLabel;
	TextButton openButton;
	TextButton saveToFileButton;

	TextButton playButton;
	TextButton stopButton;
	TextButton forwardPlaybackButton;
	TextButton backwardPlaybackButton;
	TextEditor playbackShiftLength;
	TextButton loopButton;
	
	Label fftFreqDisplayLabel;
	Label fftFreqDisplayText;
	Label freqMark1;
	Label freqMark2;
	Label freqMark3;
	Label freqMark4;
	Label freqMark5;
	Label freqMark6;
	Label freqMark7;
	Label freqMark8;
	Label freqMark9;
	Label freqBaseValueLabel;

	//effect controls
	TextButton processReverbButton;
	TextButton processSoftclipDistortionButton;
	TextButton processHardclipDistortionButton;
	TextButton processFullrectDistortionButton;
	TextButton processHalfrectDistortionButton;
	TextButton processLowpassFilterButton;
	TextButton processHighpassFilterButton;
	TextButton processBandpassFilterButton;
	TextButton processExtractorButton;
	TextButton processReverzButton;
	TextButton processStutterButton;
	TextButton processShifterButton;

	CustomSlider reverbBalanceSlider;
	Label reverbBalanceLabel;
	CustomSlider reverbSizeSlider;
	Label reverbSizeLabel;
	CustomSlider reverbWidthSlider;
	Label reverbWidthLabel;
	CustomSlider reverbDampeningSlider;
	Label reverbDampeningLabel;
	CustomSlider reverbInitSlider;
	Label reverbInitLabel;

	CustomSlider hcdThresholdSlider;
	Label hcdThresholdLabel;

	CustomSlider scdThresholdSlider;
	Label scdThresholdLabel;

	Label LowpassLabel;
	ToggleButton LowpassButton;
	CustomSlider LowpassFreqSlider;
	Label LowpassFreqLabel;
	CustomSlider LowpassQualitySlider;
	Label LowpassQualityLabel;

	Label BandpassLabel;
	ToggleButton BandpassButton;
	CustomSlider BandpassFreqSlider;
	Label BandpassFreqLabel;
	CustomSlider BandpassQualitySlider;
	Label BandpassQualityLabel;

	Label HighpassLabel;
	ToggleButton HighpassButton;
	CustomSlider HighpassFreqSlider;
	Label HighpassFreqLabel;
	CustomSlider HighpassQualitySlider;
	Label HighpassQualityLabel;

	CustomSlider extractorIntensitySlider;
	Label extractorIntensityLabel;
	CustomSlider extractorWidthSlider;
	Label extractorWidthLabel;

	CustomSlider reverzSkewSlider;
	Label reverzSkewLabel;
	CustomSlider reverzAmountSlider;
	Label reverzAmountLabel;

	CustomSlider stutterAmountSlider;
	Label stutterAmountLabel;
	CustomSlider stutterChorusSlider;
	Label stutterChorusLabel;
	CustomSlider stutterDelaySlider;
	Label stutterDelayLabel;

	CustomSlider shifterAmountSlider;
	Label shifterAmountLabel;
	CustomSlider shifterToneSlider;
	Label shifterToneLabel;

	TextButton processPitchButton;
	CustomSlider pitchSlider;

	TextButton processGainButton;
	CustomSlider gainSlider;

	ToggleButton bitDepth16Button;
	ToggleButton bitDepth24Button;
	ToggleButton bitDepth32Button;

	CustomSlider bitDepthSlider;

	Label clippingLabel;
	Label bitDepthLabel;
	Label wetLabel;
	Label dryLabel;

	TextButton resetEffectButton;

	//TextButton processingChainButton;

	Label spectralResolutionLabel;
	CustomSlider spectralResolutionSlider;

	Label presetOpenSaveLabel;
	TextButton openPresetButton;
	TextButton savePresetButton;

	enum RadioButtonIDs
	{
		inputGainButtons = 1,
		outputGainButtons = 2,
		bitDepthButtons = 3
	};


	//playback states
	enum class TransportState
	{
		Playing,
		Stopped,
		Paused
	};


	//buffers and audio device structures
	AudioFormatManager formatManager;
	AudioBuffer <float> fileBuffer;
	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> scdistortionBuffer;
	AudioBuffer <float> hcdistortionBuffer;
	AudioBuffer <float> frdistortionBuffer;
	AudioBuffer <float> hrdistortionBuffer;
	AudioBuffer <float> reverbBuffer;
	AudioBuffer <float> extractorBuffer;
	AudioBuffer <float> filterBuffer;
	AudioBuffer <float> gainBuffer;
	AudioBuffer <float> reverzBuffer;
	AudioBuffer <float> stutterBuffer;
	AudioBuffer <float> shifterBuffer;
	AudioBuffer <float> pitchBuffer;

	TransportState state;
	AudioTransportSource transportSource;
	AudioDeviceManager deviceManager;
	AudioDeviceManager::AudioDeviceSetup adsetup;
	std::unique_ptr<juce::FileChooser> chooser;


	//effect control flags
	int bitDepth;
	Atomic <bool> stopFlag;
	Atomic <bool> pauseFlag;
	Atomic <bool> loopEnabled;
	Atomic <bool> gainEnabled;
	Atomic <bool> lpfEnabled;
	Atomic <bool> hpfEnabled;
	Atomic <bool> bpfEnabled;
	Atomic <bool> reverbEnabled;
	Atomic <bool> extractorEnabled;
	Atomic <bool> scdEnabled;
	Atomic <bool> hcdEnabled;
	Atomic <bool> frdEnabled;
	Atomic <bool> hrdEnabled;
	Atomic <bool> reverzEnabled;
	Atomic <bool> stutterEnabled;
	Atomic <bool> shifterEnabled;
	Atomic <bool> pitchEnabled;
	Atomic <bool> isClipping;

	Atomic <bool> filterResponseEnabled;

	Atomic <bool> pitchStop;


	//waveform variables
	AudioThumbnailCache thumbnailCache;
	AudioThumbnail thumbnail;

	
	//effect instances
	ReverbProcessor reverb;
	DistortionProcessor distortion;
	FilterProcessor filter;
	ExtractorProcessor extractor;
	GainProcessor gain;
	ReverzProcessor reverz;
	StutterProcessor stutter;
	ShifterProcessor shifter;
	PitchProcessor pitch;
	FFTProcessor fftprocessor;

	//gui colours
	Colour buttonColour;
	Colour sliderThumbColour;
	Colour sliderLineColour;

	//mouse listener
	MouseListener *playButtonMouseListener;
	SafePointer<MouseListener> sliderMouseListener;
 
	//frequency display variables
	juce::dsp::FFT forwardFFT;                   
	juce::dsp::WindowingFunction<float> window;
	float spectralResolutionCoef;

	TextButton toggleTooltipButton;

	//functions
	void openButtonClicked(void);
	void playButtonClicked(void);
	void stopButtonClicked(void);

	void processOpenedFile(File file, std::unique_ptr<juce::FileChooser> chooser, std::unique_ptr<AudioFormatReader> reader);
	void filesDropped(const StringArray& files, int x, int y) override;
	bool isInterestedInFileDrag(const StringArray& files) override;

	void forwardPlaybackButtonClicked(void);
	void backwardPlaybackButtonClicked(void);
	void playbackShiftLengthChanged(void);
	void loopButtonClicked(void);

	void spectralResolutionSliderChanged(void);

	//void processAllEffects(void);

	void pushNextSampleIntoFifo(float sample) noexcept;
	void drawNextFrameOfSpectrum(void);
	void drawFrame(juce::Graphics& g);
	float cordValueToFreq(float cordValue);
	float freqValueToCord(float freqValue);

	Atomic<bool> processEffectButtonClicked(TextButton& effectButton, Atomic<bool> effectEnabled);
	void processEffectClipping(AudioBuffer <float> effectBuffer);

	void processGainButtonClicked(void);
	void processGainSliderChange(void);

	void processPitchButtonClicked(void);
	void processPitchSliderChange(void);

	void processReverbButtonClicked(void);
	void processReverbSliderChange(void);

	void processSoftclipDistortionButtonClicked(void);
	void processSoftclipDistortionSliderChange(void);
	void processHardclipDistortionButtonClicked(void);
	void processHardclipDistortionSliderChange(void);
	void processFullrectDistortionButtonClicked(void);
	void processFullrectDistortionSliderChange(void);
	void processHalfrectDistortionButtonClicked(void);
	void processHalfrectDistortionSliderChange(void);

	void processFiltersSliderChange(void);
	void processLowpassFilterButtonClicked(void);
	void processHighpassFilterButtonClicked(void);
	void processBandpassFilterButtonClicked(void);

	void processExtractorButtonClicked(void);
	void processExtractorSliderChange(void);

	void processReverzButtonClicked(void);
	void processReverzSliderChange(void);

	void processStutterButtonClicked(void);
	void processStutterSliderChange(void);

	void processShifterButtonClicked(void);
	void processShifterSliderChange(void);

	void saveToFileButtonClicked(void);

	void resetEffectButtonClicked(void);

	void fftFreqDisplayChanged(void);

	void changeListenerCallback(ChangeBroadcaster* source) override;
	void timerCallback(void) override;

	void changeState(TransportState newState);

	void thumbnailChanged(void);
	void paintIfNoFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds);
	void paintIfFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds);

	bool keyPressed(const KeyPress& press);
	void mouseDown(const MouseEvent& event);
	void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel);

	void processingChainButtonClicked(void);

	void createXmlFile(void);

	void openXmlFile(void);
	void saveXmlFile(void);

	void parseXmlElements(File file);
	void saveXmlElements(File file);

	void savePresetButtonClicked(void);
	void openPresetButtonClicked(void);

	double atomicToDouble(Atomic<bool> atomicValue);
	bool doubleToBool(double doubleValue);

	TooltipWindow toolTipW;
	void toggleTooltipButtonClicked(void);
	Atomic <bool> tooltipEnabled;

	//juce::AudioProcessorValueTreeState parameters;
	
	//juce::AudioAppComponent::SafePointer<ProcessingChain> processchainwindow;
	/*juce::TopLevelWindow::SafePointer<ProcessingChain> processchainwindow;*/
	//AudioAppComponent::SafePointer<TopLevelWindow> toplevelwindow;
	//void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

//thread for multi-thread processing
/*class BackgroundThread : public Thread
{
public:
	BackgroundThread() : Thread("background operation thread")
	{

	}

	void run() override
	{
		mcomp.processAllEffects();
		signalThreadShouldExit();
	}

	MainComponent mcomp;

};*/

