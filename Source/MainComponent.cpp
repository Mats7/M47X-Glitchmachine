/**
* Main Class of the Application
* Contains - all GUI controls and their behaviour.
* -open/save file
* -open/save preset (with xml)
* -playback controls
* -reset effects
* -tooltips
* -helper functions
* 
* Code for opening and saving the file is inspired by JUCE Tutorial - https://docs.juce.com/master/tutorial_playing_sound_files.html
* Code for playing the file (and timer controls) is inspired by JUCE Tutorial - https://docs.juce.com/master/tutorial_playing_sound_files.html
* Code for drawing a thumbnail is inspired by JUCE Tutorial - https://docs.juce.com/master/tutorial_audio_thumbnail.html
* Code for drawing a frequency spectrum is inspired by JUCE Tutorial - https://docs.juce.com/master/tutorial_spectrum_analyser.html
* Code for a Biquad filter (in FilterProcessor.cpp) is inspired by Earlevel Biquad C++ Source Code - https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
*/


#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : juce::AudioAppComponent(deviceManager),
                                 thumbnailCache(5),
	                             thumbnail(2, formatManager, thumbnailCache), 
								 forwardFFT(fftOrder),
								 window(fftSize, juce::dsp::WindowingFunction<float>::hann)

{
	//custom LookAndFeel class is used (for custom sliders)
	setLookAndFeel(&customLookAndFeel);

	//Custom colours
	buttonColour = Colour((uint8)48, (uint8)24, (uint8)96, (uint8)255);
	setColour(Slider::textBoxOutlineColourId, Colours::black);
	
	//Play Button
	playButton.setButtonText("Play");
	playButton.setColour(TextButton::buttonColourId, Colours::darkgreen);
	playButton.setColour(TextButton::textColourOffId, Colours::white);
	playButton.setEnabled(false);
	playButton.onClick = [this] { playButtonClicked(); };
	addAndMakeVisible(&playButton);

	//Stop Button
	stopButton.setButtonText("Stop");
	stopButton.setColour(TextButton::buttonColourId, Colours::darkred);
	stopButton.setColour(TextButton::textColourOffId, Colours::white);
	stopButton.setEnabled(false);
	stopButton.onClick = [this] { stopButtonClicked(); };
	addAndMakeVisible(&stopButton);

	fileOpenSaveLabel.setText("FILE", dontSendNotification);
	fileOpenSaveLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	fileOpenSaveLabel.setJustificationType(Justification::centred);
	fileOpenSaveLabel.setColour(Label::outlineColourId, buttonColour);
	addAndMakeVisible(&fileOpenSaveLabel);

	//Open Button
	openButton.setButtonText("Open");
	openButton.setColour(TextButton::buttonColourId, Colours::midnightblue);
	openButton.setColour(TextButton::textColourOffId, Colours::white);
	openButton.onClick = [this] { openButtonClicked(); };
	addAndMakeVisible(&openButton);

	//Save Button
	saveToFileButton.setButtonText("Save");
	saveToFileButton.setColour(TextButton::buttonColourId, Colour(32, 96, 96));
	saveToFileButton.setColour(TextButton::textColourOffId, Colours::white);
	saveToFileButton.setEnabled(false);
	saveToFileButton.onClick = [this] { saveToFileButtonClicked();  };
	addAndMakeVisible(&saveToFileButton);

	presetOpenSaveLabel.setText("PRESET", dontSendNotification);
	presetOpenSaveLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	presetOpenSaveLabel.setJustificationType(Justification::centred);
	presetOpenSaveLabel.setColour(Label::outlineColourId, buttonColour);
	addAndMakeVisible(&presetOpenSaveLabel);

	//Open preset button
	openPresetButton.setButtonText("Open");
	openPresetButton.setColour(TextButton::buttonColourId, Colours::midnightblue);
	openPresetButton.setColour(TextButton::textColourOffId, Colours::white);
	openPresetButton.setEnabled(false);
	openPresetButton.onClick = [this] { openPresetButtonClicked(); };
	addAndMakeVisible(&openPresetButton);

	//Save preset button
	savePresetButton.setButtonText("Save");
	savePresetButton.setColour(TextButton::buttonColourId, Colour(32, 96, 96));
	savePresetButton.setColour(TextButton::textColourOffId, Colours::white);
	savePresetButton.setEnabled(false);
	savePresetButton.onClick = [this] { savePresetButtonClicked(); };
	addAndMakeVisible(&savePresetButton);


	//Playback Controls
	forwardPlaybackButton.setButtonText(">");
	forwardPlaybackButton.setColour(TextButton::buttonColourId, buttonColour);
	forwardPlaybackButton.setColour(TextButton::textColourOffId, Colours::white);
	forwardPlaybackButton.setRepeatSpeed(800, 100);
	forwardPlaybackButton.setEnabled(false);
	forwardPlaybackButton.onClick = [this] { forwardPlaybackButtonClicked(); };
	addAndMakeVisible(&forwardPlaybackButton);

	backwardPlaybackButton.setButtonText("<");
	backwardPlaybackButton.setColour(TextButton::buttonColourId, buttonColour);
	backwardPlaybackButton.setColour(TextButton::textColourOffId, Colours::white);
	backwardPlaybackButton.setRepeatSpeed(800, 100);
	backwardPlaybackButton.setEnabled(false);
	backwardPlaybackButton.onClick = [this] { backwardPlaybackButtonClicked(); };
	addAndMakeVisible(&backwardPlaybackButton);

	playbackShiftLength.setTextToShowWhenEmpty("samples", Colours::white);
	playbackShiftLength.setFont({ "Montserrat", 14.0f, Font::plain });
	playbackShiftLength.setColour(TextEditor::backgroundColourId, buttonColour);
	playbackShiftLength.setColour(TextEditor::textColourId, Colours::white);
	playbackShiftLength.setColour(TextEditor::outlineColourId, Colours::black);
	playbackShiftLength.setColour(TextEditor::focusedOutlineColourId, Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
	playbackShiftLength.setCaretVisible(false);
	playbackShiftLength.setJustification(juce::Justification::centred);
	playbackShiftLength.setTooltip(juce::String::String("Enter shift length in samples..."));
	playbackShiftLength.setEnabled(false);
	playbackShiftLength.onTextChange = [this] { playbackShiftLengthChanged(); };
	addAndMakeVisible(&playbackShiftLength);


	//Loop Button
	loopButton.setButtonText("Loop");
	loopButton.setColour(TextButton::buttonColourId, buttonColour);
	loopButton.setColour(TextButton::textColourOffId, Colours::white);
	loopButton.setEnabled(false);
	loopButton.onClick = [this] { loopButtonClicked(); };
	addAndMakeVisible(&loopButton);


	//Spectrum window controls and labels
	spectralResolutionLabel.setText("Res", dontSendNotification);
	spectralResolutionLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	spectralResolutionLabel.setJustificationType(Justification::centred);
	spectralResolutionLabel.attachToComponent(&spectralResolutionSlider, false);
	addAndMakeVisible(&spectralResolutionLabel);

	spectralResolutionSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	spectralResolutionSlider.setRange(1, 5, 1);
	spectralResolutionSlider.setValue(4);
	spectralResolutionSlider.setDoubleClickReturnValue(true, 4);
	spectralResolutionSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	spectralResolutionSlider.setEnabled(false);
	addAndMakeVisible(&spectralResolutionSlider);

	fftFreqDisplayLabel.setText("Freq", dontSendNotification);
	fftFreqDisplayLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	fftFreqDisplayLabel.setJustificationType(Justification::centred);
	fftFreqDisplayLabel.attachToComponent(&fftFreqDisplayText, false);
	addAndMakeVisible(&fftFreqDisplayLabel);

	fftFreqDisplayText.setJustificationType(Justification::centred);
	fftFreqDisplayText.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&fftFreqDisplayText);


	//frequency marks
	freqMark1.setJustificationType(Justification::centred);
	freqMark1.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark1);
	freqMark2.setJustificationType(Justification::centred);
	freqMark2.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark2);
	freqMark3.setJustificationType(Justification::centred);
	freqMark3.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark3);
	freqMark4.setJustificationType(Justification::centred);
	freqMark4.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark4);
	freqMark5.setJustificationType(Justification::centred);
	freqMark5.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark5);
	freqMark6.setJustificationType(Justification::centred);
	freqMark6.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark6);
	freqMark7.setJustificationType(Justification::centred);
	freqMark7.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark7);
	freqMark8.setJustificationType(Justification::centred);
	freqMark8.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark8);
	freqMark9.setJustificationType(Justification::centred);
	freqMark9.setFont({ "Montserrat", 15.0f, Font::plain });
	addAndMakeVisible(&freqMark9);

	freqBaseValueLabel.setJustificationType(Justification::left);
	freqBaseValueLabel.setFont({ "Montserrat", 14.0f, Font::plain });
	freqBaseValueLabel.setText("0dB", dontSendNotification);
	freqBaseValueLabel.setColour(Label::textColourId, Colour(200, 200, 200));
	addAndMakeVisible(&freqBaseValueLabel);
	

	//SC Distortion Button
	processSoftclipDistortionButton.setButtonText("SoftClip");
	processSoftclipDistortionButton.setColour(TextButton::buttonColourId, buttonColour);
	processSoftclipDistortionButton.setColour(TextButton::textColourOffId, Colours::white);
	processSoftclipDistortionButton.setEnabled(false);
	processSoftclipDistortionButton.onClick = [this] {  processSoftclipDistortionButtonClicked(); };
	addAndMakeVisible(&processSoftclipDistortionButton);

	//Softclip Threshold
	scdThresholdSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	scdThresholdSlider.setRange(0, 1.0, 0.05);
	scdThresholdSlider.setValue(0.0);
	scdThresholdSlider.setDoubleClickReturnValue(true, 0);
	scdThresholdSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	scdThresholdSlider.setEnabled(false);
	addAndMakeVisible(&scdThresholdSlider);
	scdThresholdLabel.setText("Threshold", dontSendNotification);
	scdThresholdLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	scdThresholdLabel.setJustificationType(Justification::centred);
	scdThresholdLabel.attachToComponent(&scdThresholdSlider, false);
	addAndMakeVisible(&hcdThresholdLabel);


	//HC Distortion Button
	processHardclipDistortionButton.setButtonText("Hardclip");
	processHardclipDistortionButton.setColour(TextButton::buttonColourId, buttonColour);
	processHardclipDistortionButton.setColour(TextButton::textColourOffId, Colours::white);
	processHardclipDistortionButton.setEnabled(false);
	processHardclipDistortionButton.onClick = [this] {  processHardclipDistortionButtonClicked(); };
	addAndMakeVisible(&processHardclipDistortionButton);

	//Hardclip Threshold
	hcdThresholdSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	hcdThresholdSlider.setRange(0, 1.0, 0.05);
	hcdThresholdSlider.setValue(0.0);
	hcdThresholdSlider.setDoubleClickReturnValue(true, 0);
	hcdThresholdSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	hcdThresholdSlider.setEnabled(false);
	addAndMakeVisible(&hcdThresholdSlider);
	hcdThresholdLabel.setText("Threshold", dontSendNotification);
	hcdThresholdLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	hcdThresholdLabel.setJustificationType(Justification::centred);
	hcdThresholdLabel.attachToComponent(&hcdThresholdSlider, false);
	addAndMakeVisible(&hcdThresholdLabel);


	//Fullrect Distortion Button
	processFullrectDistortionButton.setButtonText("Fullrect");
	processFullrectDistortionButton.setColour(TextButton::buttonColourId, buttonColour);
	processFullrectDistortionButton.setColour(TextButton::textColourOffId, Colours::white);
	processFullrectDistortionButton.setEnabled(false);
	processFullrectDistortionButton.onClick = [this] {  processFullrectDistortionButtonClicked(); };
	addAndMakeVisible(&processFullrectDistortionButton);


	//Halfrect Distortion Button
	processHalfrectDistortionButton.setButtonText("Halfrect");
	processHalfrectDistortionButton.setColour(TextButton::buttonColourId, buttonColour);
	processHalfrectDistortionButton.setColour(TextButton::textColourOffId, Colours::white);
	processHalfrectDistortionButton.setEnabled(false);
	processHalfrectDistortionButton.onClick = [this] {  processHalfrectDistortionButtonClicked(); };
	addAndMakeVisible(&processHalfrectDistortionButton);


	//LP Filter Button
	processLowpassFilterButton.setButtonText("LP");
	processLowpassFilterButton.setColour(TextButton::buttonColourId, buttonColour);
	processLowpassFilterButton.setColour(TextButton::textColourOffId, Colours::white);
	processLowpassFilterButton.setEnabled(false);
	processLowpassFilterButton.onClick = [this] {  processLowpassFilterButtonClicked(); };
	addAndMakeVisible(&processLowpassFilterButton);

	//LP Frequency
	LowpassFreqSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	LowpassFreqSlider.setRange(1, 20000, 1.0);
	LowpassFreqSlider.setValue(2000.0);
	LowpassFreqSlider.setSkewFactor(0.5);
	LowpassFreqSlider.setDoubleClickReturnValue(true, 2000.0);
	LowpassFreqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	LowpassFreqSlider.setEnabled(false);
	addAndMakeVisible(&LowpassFreqSlider);
	LowpassFreqLabel.setText("Cutoff", dontSendNotification);
	LowpassFreqLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	LowpassFreqLabel.setJustificationType(Justification::centred);
	LowpassFreqLabel.attachToComponent(&LowpassFreqSlider, false);
	addAndMakeVisible(&LowpassFreqLabel);

	//LP Quality
	LowpassQualitySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	LowpassQualitySlider.setRange(0.1, 10.0, 0.1);
	LowpassQualitySlider.setValue(1.0);
	LowpassQualitySlider.setDoubleClickReturnValue(true, 1.0);
	LowpassQualitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	LowpassQualitySlider.setEnabled(false);
	addAndMakeVisible(&LowpassQualitySlider);
	LowpassQualityLabel.setText("Q", dontSendNotification);
	LowpassQualityLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	LowpassQualityLabel.setJustificationType(Justification::centred);
	LowpassQualityLabel.attachToComponent(&LowpassQualitySlider, false);
	addAndMakeVisible(&LowpassQualityLabel);


	//HP Filter Button
	processHighpassFilterButton.setButtonText("HP");
	processHighpassFilterButton.setColour(TextButton::buttonColourId, buttonColour);
	processHighpassFilterButton.setColour(TextButton::textColourOffId, Colours::white);
	processHighpassFilterButton.setEnabled(false);
	processHighpassFilterButton.onClick = [this] {  processHighpassFilterButtonClicked(); };
	addAndMakeVisible(&processHighpassFilterButton);

	//HP Frequency
	HighpassFreqSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	HighpassFreqSlider.setRange(1, 20000, 1.0);
	HighpassFreqSlider.setValue(200.0);
	HighpassFreqSlider.setSkewFactor(0.5);
	HighpassFreqSlider.setDoubleClickReturnValue(true, 200.0);
	HighpassFreqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	HighpassFreqSlider.setEnabled(false);
	addAndMakeVisible(&HighpassFreqSlider);
	HighpassFreqLabel.setText("Cutoff", dontSendNotification);
	HighpassFreqLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	HighpassFreqLabel.setJustificationType(Justification::centred);
	HighpassFreqLabel.attachToComponent(&HighpassFreqSlider, false);
	addAndMakeVisible(&HighpassFreqLabel);

	//HP Quality
	HighpassQualitySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	HighpassQualitySlider.setRange(0.1, 10.0, 0.1);
	HighpassQualitySlider.setValue(1.0);
	HighpassQualitySlider.setDoubleClickReturnValue(true, 1.0);
	HighpassQualitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	HighpassQualitySlider.setEnabled(false);
	addAndMakeVisible(&HighpassQualitySlider);
	HighpassQualityLabel.setText("Q", dontSendNotification);
	HighpassQualityLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	HighpassQualityLabel.setJustificationType(Justification::centred);
	HighpassQualityLabel.attachToComponent(&HighpassQualitySlider, false);
	addAndMakeVisible(&HighpassQualityLabel);


	//BP Filter Button
	processBandpassFilterButton.setButtonText("BP");
	processBandpassFilterButton.setColour(TextButton::buttonColourId, buttonColour);
	processBandpassFilterButton.setColour(TextButton::textColourOffId, Colours::white);
	processBandpassFilterButton.setEnabled(false);
	processBandpassFilterButton.onClick = [this] {  processBandpassFilterButtonClicked(); };
	addAndMakeVisible(&processBandpassFilterButton);

	//BP Frequency 
	BandpassFreqSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	BandpassFreqSlider.setRange(1, 20000, 1.0);
	BandpassFreqSlider.setValue(666.0);
	BandpassFreqSlider.setSkewFactor(0.5);
	BandpassFreqSlider.setDoubleClickReturnValue(true, 666.0);
	BandpassFreqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	BandpassFreqSlider.setEnabled(false);
	addAndMakeVisible(&BandpassFreqSlider);
	BandpassFreqLabel.setText("Cutoff", dontSendNotification);
	BandpassFreqLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	BandpassFreqLabel.setJustificationType(Justification::centred);
	BandpassFreqLabel.attachToComponent(&BandpassFreqSlider, false);
	addAndMakeVisible(&BandpassFreqLabel);

	//BP Quality
	BandpassQualitySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	BandpassQualitySlider.setRange(0.1, 10.0, 0.1);
	BandpassQualitySlider.setValue(1.0);
	BandpassQualitySlider.setDoubleClickReturnValue(true, 1.0);
	BandpassQualitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	BandpassQualitySlider.setEnabled(false);
	addAndMakeVisible(&BandpassQualitySlider);
	BandpassQualityLabel.setText("Q", dontSendNotification);
	BandpassQualityLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	BandpassQualityLabel.setJustificationType(Justification::centred);
	BandpassQualityLabel.attachToComponent(&BandpassQualitySlider, false);
	addAndMakeVisible(&BandpassQualityLabel);


	//Reverb Button
	processReverbButton.setButtonText("Reverb");
	processReverbButton.setColour(TextButton::buttonColourId, buttonColour);
	processReverbButton.setColour(TextButton::textColourOffId, Colours::white);
	processReverbButton.setEnabled(false);
	processReverbButton.onClick = [this] {  processReverbButtonClicked(); };
	addAndMakeVisible(&processReverbButton);


	//Reverb Balance
	reverbBalanceSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	reverbBalanceSlider.setRange(0.0, 1.0, 0.01);
	reverbBalanceSlider.setValue(0.5);
	reverbBalanceSlider.setDoubleClickReturnValue(true, 0.5);
	reverbBalanceSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	reverbBalanceSlider.setEnabled(false);
	addAndMakeVisible(&reverbBalanceSlider);
	reverbBalanceLabel.setText("Balance", dontSendNotification);
	reverbBalanceLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	reverbBalanceLabel.setJustificationType(Justification::centred);
	reverbBalanceLabel.attachToComponent(&reverbBalanceSlider, false);
	addAndMakeVisible(&reverbBalanceLabel);


	//Reverb Size
	reverbSizeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	reverbSizeSlider.setRange(0.0, 1.0, 0.01);
	reverbSizeSlider.setValue(0.5);
	reverbSizeSlider.setDoubleClickReturnValue(true, 0.5);
	reverbSizeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	reverbSizeSlider.setEnabled(false);
	addAndMakeVisible(&reverbSizeSlider);
	reverbSizeLabel.setText("Size", dontSendNotification);
	reverbSizeLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	reverbSizeLabel.setJustificationType(Justification::centred);
	reverbSizeLabel.attachToComponent(&reverbSizeSlider, false);
	addAndMakeVisible(&reverbSizeLabel);

	//Reverb Damp
	reverbDampeningSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	reverbDampeningSlider.setRange(0.0, 1.0, 0.01);
	reverbDampeningSlider.setValue(0.5);
	reverbDampeningSlider.setDoubleClickReturnValue(true, 0.5);
	reverbDampeningSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	reverbDampeningSlider.setEnabled(false);
	addAndMakeVisible(&reverbDampeningSlider);
	reverbDampeningLabel.setText("Damp", dontSendNotification);
	reverbDampeningLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	reverbDampeningLabel.setJustificationType(Justification::centred);
	reverbDampeningLabel.attachToComponent(&reverbDampeningSlider, false);
	addAndMakeVisible(&reverbDampeningLabel);

	//Reverb Width
	reverbWidthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	reverbWidthSlider.setRange(0.0, 1.0, 0.01);
	reverbWidthSlider.setValue(0.5);
	reverbWidthSlider.setDoubleClickReturnValue(true, 0.5);
	reverbWidthSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	reverbWidthSlider.setEnabled(false);
	addAndMakeVisible(&reverbWidthSlider);
	reverbWidthLabel.setText("Width", dontSendNotification);
	reverbWidthLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	reverbWidthLabel.setJustificationType(Justification::centred);
	reverbWidthLabel.attachToComponent(&reverbWidthSlider, false);
	addAndMakeVisible(&reverbWidthLabel);


	//Extractor Button
	processExtractorButton.setButtonText("Extractor");
	processExtractorButton.setColour(TextButton::buttonColourId, buttonColour);
	processExtractorButton.setColour(TextButton::textColourOffId, Colours::white);
	processExtractorButton.setEnabled(false);
	processExtractorButton.onClick = [this] {  processExtractorButtonClicked(); };
	addAndMakeVisible(&processExtractorButton);

	//Extractor Intensity
	extractorIntensitySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	extractorIntensitySlider.setRange(0, 50, 1);
	extractorIntensitySlider.setValue(0);
	extractorIntensitySlider.setDoubleClickReturnValue(true, 0);
	extractorIntensitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	extractorIntensitySlider.setEnabled(false);
	addAndMakeVisible(&extractorIntensitySlider);
	extractorIntensityLabel.setText("Intensity", dontSendNotification);
	extractorIntensityLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	extractorIntensityLabel.setJustificationType(Justification::centred);
	extractorIntensityLabel.attachToComponent(&extractorIntensitySlider, false);
	addAndMakeVisible(&extractorIntensityLabel);

	//Extractor Width
	extractorWidthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	extractorWidthSlider.setRange(0, 100, 1);
	extractorWidthSlider.setValue(0);
	extractorWidthSlider.setDoubleClickReturnValue(true, 0);
	extractorWidthSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	extractorWidthSlider.setEnabled(false);
	addAndMakeVisible(&extractorWidthSlider);
	extractorWidthLabel.setText("Width", dontSendNotification);
	extractorWidthLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	extractorWidthLabel.setJustificationType(Justification::centred);
	extractorWidthLabel.attachToComponent(&extractorWidthSlider, false);
	addAndMakeVisible(&extractorWidthLabel);


	//Reverz Button
	processReverzButton.setButtonText("Reverz");
	processReverzButton.setColour(TextButton::buttonColourId, buttonColour);
	processReverzButton.setColour(TextButton::textColourOffId, Colours::white);
	processReverzButton.setEnabled(false);
	processReverzButton.onClick = [this] {  processReverzButtonClicked(); };
	addAndMakeVisible(&processReverzButton);

	//Reverz Skew
	reverzSkewSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	reverzSkewSlider.setRange(-10, 10, 1);
	reverzSkewSlider.setValue(0);
	reverzSkewSlider.setDoubleClickReturnValue(true, 0);
	reverzSkewSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	reverzSkewSlider.setEnabled(false);
	addAndMakeVisible(&reverzSkewSlider);
	reverzSkewLabel.setText("Skew", dontSendNotification);
	reverzSkewLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	reverzSkewLabel.setJustificationType(Justification::centred);
	reverzSkewLabel.attachToComponent(&reverzSkewSlider, false);
	addAndMakeVisible(&reverzSkewLabel);

	//Reverz Amount
	reverzAmountSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	reverzAmountSlider.setRange(1, 64, 1);
	reverzAmountSlider.setValue(16);
	reverzAmountSlider.setDoubleClickReturnValue(true, 16);
	reverzAmountSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	reverzAmountSlider.setEnabled(false);
	addAndMakeVisible(&reverzAmountSlider);
	reverzAmountLabel.setText("Amount", dontSendNotification);
	reverzAmountLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	reverzAmountLabel.setJustificationType(Justification::centred);
	reverzAmountLabel.attachToComponent(&reverzAmountSlider, false);
	addAndMakeVisible(&reverzAmountLabel);


	//Stutter Button
	processStutterButton.setButtonText("Stutter");
	processStutterButton.setColour(TextButton::buttonColourId, buttonColour);
	processStutterButton.setColour(TextButton::textColourOffId, Colours::white);
	processStutterButton.setEnabled(false);
	processStutterButton.onClick = [this] {  processStutterButtonClicked(); };
	addAndMakeVisible(&processStutterButton);

	//Stutter Amount
	stutterAmountSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	stutterAmountSlider.setRange(4, 64, 2);
	stutterAmountSlider.setValue(16);
	stutterAmountSlider.setDoubleClickReturnValue(true, 16);
	stutterAmountSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	stutterAmountSlider.setEnabled(false);
	addAndMakeVisible(&stutterAmountSlider);
	stutterAmountLabel.setText("Amount", dontSendNotification);
	stutterAmountLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	stutterAmountLabel.setJustificationType(Justification::centred);
	stutterAmountLabel.attachToComponent(&stutterAmountSlider, false);
	addAndMakeVisible(&stutterAmountLabel);

	//Stutter Chorus
	stutterChorusSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	stutterChorusSlider.setRange(0, 20, 1);
	stutterChorusSlider.setValue(0);
	stutterChorusSlider.setDoubleClickReturnValue(true, 0);
	stutterChorusSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	stutterChorusSlider.setEnabled(false);
	addAndMakeVisible(&stutterChorusSlider);
	stutterChorusLabel.setText("Chorus", dontSendNotification);
	stutterChorusLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	stutterChorusLabel.setJustificationType(Justification::centred);
	stutterChorusLabel.attachToComponent(&stutterChorusSlider, false);
	addAndMakeVisible(&stutterChorusLabel);

	//Stutter Delay
	stutterDelaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	stutterDelaySlider.setRange(1, 99, 1);
	stutterDelaySlider.setValue(50);
	stutterDelaySlider.setDoubleClickReturnValue(true, 50);
	stutterDelaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	stutterDelaySlider.setEnabled(false);
	addAndMakeVisible(&stutterDelaySlider);
	stutterDelayLabel.setText("Delay", dontSendNotification);
	stutterDelayLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	stutterDelayLabel.setJustificationType(Justification::centred);
	stutterDelayLabel.attachToComponent(&stutterDelaySlider, false);
	addAndMakeVisible(&stutterDelayLabel);


	//Shifter Button
	processShifterButton.setButtonText("Shifter");
	processShifterButton.setColour(TextButton::buttonColourId, buttonColour);
	processShifterButton.setColour(TextButton::textColourOffId, Colours::white);
	processShifterButton.setEnabled(false);
	processShifterButton.onClick = [this] {  processShifterButtonClicked(); };
	addAndMakeVisible(&processShifterButton);

	//Shifter Amount
	shifterAmountSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	shifterAmountSlider.setRange(4, 128, 2);
	shifterAmountSlider.setValue(16);
	shifterAmountSlider.setDoubleClickReturnValue(true, 16);
	shifterAmountSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	shifterAmountSlider.setEnabled(false);
	addAndMakeVisible(&shifterAmountSlider);
	shifterAmountLabel.setText("Amount", dontSendNotification);
	shifterAmountLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	shifterAmountLabel.setJustificationType(Justification::centred);
	shifterAmountLabel.attachToComponent(&shifterAmountSlider, false);
	addAndMakeVisible(&shifterAmountLabel);

	//Shifter Tone
	shifterToneSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	shifterToneSlider.setRange(1.0, 8.0, 0.1);
	shifterToneSlider.setValue(2.0);
	shifterToneSlider.setDoubleClickReturnValue(true, 2.0);
	shifterToneSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	shifterToneSlider.setEnabled(false);
	addAndMakeVisible(&shifterToneSlider);
	shifterToneLabel.setText("Tone", dontSendNotification);
	shifterToneLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	shifterToneLabel.setJustificationType(Justification::centred);
	shifterToneLabel.attachToComponent(&shifterToneSlider, false);
	addAndMakeVisible(&shifterToneLabel);


	//Pitch
	processPitchButton.setButtonText("Pitch");
	processPitchButton.setColour(TextButton::buttonColourId, buttonColour);
	processPitchButton.setColour(TextButton::textColourOffId, Colours::white);
	processPitchButton.setEnabled(false);
	processPitchButton.onClick = [this] {  processPitchButtonClicked(); };
	addAndMakeVisible(&processPitchButton);

	pitchSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	pitchSlider.setRange(0.1, 4.0, 0.1);
	pitchSlider.setValue(1.0);
	pitchSlider.setDoubleClickReturnValue(true, 1.0);
	pitchSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	pitchSlider.setEnabled(false);
	addAndMakeVisible(&pitchSlider);


	//Gain
	processGainButton.setButtonText("Gain");
	processGainButton.setColour(TextButton::buttonColourId, buttonColour);
	processGainButton.setColour(TextButton::textColourOffId, Colours::white);
	processGainButton.setEnabled(false);
	processGainButton.onClick = [this] {  processGainButtonClicked(); };
	addAndMakeVisible(&processGainButton);

	gainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	gainSlider.setRange(-40.0, 40.0, 0.1);
	gainSlider.setValue(0.0);
	gainSlider.setDoubleClickReturnValue(true, 0);
	gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	//gainSlider.addMouseListener(sliderMouseListener, true);
	gainSlider.setEnabled(false);
	addAndMakeVisible(&gainSlider);


	//Bit Depth
	bitDepth16Button.setButtonText("16b");
	bitDepth16Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	bitDepth16Button.setRadioGroupId(bitDepthButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&bitDepth16Button);

	bitDepth24Button.setButtonText("24b");
	bitDepth24Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	bitDepth24Button.setRadioGroupId(bitDepthButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&bitDepth24Button);

	bitDepth32Button.setButtonText("32b");
	bitDepth32Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	bitDepth32Button.setRadioGroupId(bitDepthButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&bitDepth32Button);


	bitDepthSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
	bitDepthSlider.setRange(16, 32, 8);
	bitDepthSlider.setDoubleClickReturnValue(false, 32);
	bitDepthSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	bitDepthSlider.setEnabled(false);
	addAndMakeVisible(&bitDepthSlider);



	//Labels
	clippingLabel.setText("No Clipping", NotificationType::dontSendNotification);
	clippingLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	clippingLabel.setColour(Label::textColourId, Colours::limegreen);
	clippingLabel.setColour(Label::outlineColourId, buttonColour);
	clippingLabel.setEditable(false);
	addAndMakeVisible(&clippingLabel);

	bitDepthLabel.setText("Bit depth", NotificationType::dontSendNotification);
	bitDepthLabel.setFont({ "Montserrat", 15.0f, Font::plain });
	bitDepthLabel.setColour(Label::outlineColourId, buttonColour);
	bitDepthLabel.setEditable(false);
	addAndMakeVisible(&bitDepthLabel);

	wetLabel.setText("Wet", NotificationType::dontSendNotification);
	wetLabel.setEditable(false);
	addAndMakeVisible(&wetLabel);

	dryLabel.setText("Dry", NotificationType::dontSendNotification);
	dryLabel.setEditable(false);
	addAndMakeVisible(&dryLabel);


	resetEffectButton.setButtonText("Reset");
	resetEffectButton.setColour(TextButton::buttonColourId, Colours::darkred);
	resetEffectButton.setColour(TextButton::textColourOffId, Colours::white);
	resetEffectButton.setEnabled(false);
	resetEffectButton.onClick = [this] {  resetEffectButtonClicked(); };
	addAndMakeVisible(&resetEffectButton);


	toggleTooltipButton.setButtonText("Tooltips");
	toggleTooltipButton.setColour(TextButton::buttonColourId, Colours::darkgrey);
	toggleTooltipButton.setColour(TextButton::textColourOffId, Colours::white);
	toggleTooltipButton.setEnabled(false);
	toggleTooltipButton.onClick = [this] {  toggleTooltipButtonClicked(); };
	addAndMakeVisible(&toggleTooltipButton);

	position = 0;
	state = TransportState::Stopped;

	thumbnail.addChangeListener(this);

    deviceManager.initialise(0, 2, nullptr, true);
	adsetup = deviceManager.getAudioDeviceSetup();
  	adsetup.sampleRate = 44100.0;
    deviceManager.setAudioDeviceSetup(adsetup, true);

	formatManager.registerBasicFormats();

	setSize(1200, 680);
}

/** Destructor
*
*/
MainComponent::~MainComponent()
{
	/*if (processchainwindow) delete processchainwindow;*/
	setLookAndFeel(nullptr);
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/** Playback of the audio file from the fileBuffer
* Is controlled by state flags used in playback control
*/
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
	bufferToFill.clearActiveBufferRegion();

	if (stopFlag.get() == false)
	{
		bool playbackFinished = false;
		auto numInputChannels = fileBuffer.getNumChannels();
		auto numOutputChannels = bufferToFill.buffer->getNumChannels();
		auto outputSamplesRemaining = bufferToFill.numSamples;
		auto outputSamplesOffset = bufferToFill.startSample;

		//load the samples until the end of the fileBuffer
		while (outputSamplesRemaining > 0)
		{
			if (pauseFlag.get() == false) 
			{
				auto bufferSamplesRemaining = fileBuffer.getNumSamples() - position;
				auto samplesThisTime = jmin(outputSamplesRemaining, bufferSamplesRemaining);

				if (samplesThisTime == 0)
				{
					playbackFinished = true;
					break;
				}

				//copy the current block into buffer sent to output device
				for (auto channel = 0; channel < numOutputChannels; ++channel)
				{
					bufferToFill.buffer->copyFrom(channel,
						outputSamplesOffset,
						fileBuffer,
						channel % numInputChannels,
						position,
						samplesThisTime);
				}

				//push the current sample block into fft fifo array
				for (auto i = outputSamplesOffset; i < outputSamplesOffset + samplesThisTime; i++)
				{
					fftprocessor.pushNextSampleIntoFifo(bufferToFill.buffer->getArrayOfReadPointers()[0][i]);
				}

				outputSamplesRemaining -= samplesThisTime;
				outputSamplesOffset += samplesThisTime;
				position += samplesThisTime;
			}
		}

		if (playbackFinished && loopEnabled.get() == true)
		{
			position = 0;
			
		}
		else if (playbackFinished && loopEnabled.get() == false)
		{
			stopFlag.set(true);
			pauseFlag.set(true);
		}
			
	}
}

/** Called when app is stopped. Releases playback transport source and fileBuffer
*/
void MainComponent::releaseResources()
{
	fileBuffer.setSize (0, 0);
	transportSource.releaseResources();
}


/** Drawing of some of the graphical components (that need to be drawn periodically, or on command)
* Can be called via repaint() function, which is used to force the refresh of graphical components defined in paint()
* @param g - graphical context to use for drawing
*/
void MainComponent::paint(Graphics& g)
{
	//g.setFont(12.0f);
	g.fillAll(Colour((uint8)16, (uint8)16, (uint8)32, (uint8)255));

	Rectangle <int> thumbnailBounds(100, 10, getWidth() - 110, 150);
	if (thumbnail.getNumChannels() == 0)
		paintIfNoFileLoaded(g, thumbnailBounds);
	else
		paintIfFileLoaded(g, thumbnailBounds);

	g.setColour(Colours::white);
	g.setOpacity(1.0f);
	
	drawFreqMarks(g);
	drawFreqIndicator(g);

	g.setColour(juce::Colour(220, 220, 220));
	fftprocessor.drawFrame(g);

	drawFilterResponse(g);

	drawEffectBounds(g);

	Rectangle <int> spectrometerBounds(100, spPosY, spWidth, 170);
	g.setColour(Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
	g.drawRect(spectrometerBounds);

	//draw mark for 0dB value in spectrometer
	g.drawDashedLine({ 100, 282, 1030, 282 }, freqDashLength, 2);
}


/** Drawing of lines for effect boundaries
* @param g - graphical context to use for drawing
*/
void MainComponent::drawEffectBounds(Graphics& g)
{
	g.setColour(buttonColour);
	Rectangle <int> rect;
	rect.setBounds(getWidth() - 255, getHeight() - 255, 245, 245);
	g.drawRect(rect);
	rect.setBounds(getWidth() - 435, getHeight() - 255, 180, 245);
	g.drawRect(rect);
	rect.setBounds(getWidth() - 535, getHeight() - 255, 100, 245);
	g.drawRect(rect);
	g.drawLine(getWidth() - 535, getHeight() - 120, getWidth() - 435, getHeight() - 120);
	rect.setBounds(getWidth() - 635, getHeight() - 255, 100, 245);
	g.drawRect(rect);
	g.drawLine(getWidth() - 635, getHeight() - 120, getWidth() - 535, getHeight() - 120);
	rect.setBounds(getWidth() - 735, getHeight() - 255, 100, 245);
	g.drawRect(rect);
	rect.setBounds(getWidth() - 835, getHeight() - 255, 100, 245);
	g.drawRect(rect);
	rect.setBounds(getWidth() - 1015, getHeight() - 255, 180, 245);
	g.drawRect(rect);
	rect.setBounds(getWidth() - 1110, getHeight() - 255, 95, 245);
	g.drawRect(rect);
}


/** Drawing of marks for freq sepctrum window
* @param g - graphical context to use for drawing
*/
void MainComponent::drawFreqMarks(Graphics& g)
{
	g.setColour(buttonColour);
	g.drawLine(100 + (spWidth / 10) * 1, spPosY, 100 + (spWidth / 10) * 1, spPosY2);
	freqMark1.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 1)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 2, spPosY, 100 + (spWidth / 10) * 2, spPosY2);
	freqMark2.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 2)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 3, spPosY, 100 + (spWidth / 10) * 3, spPosY2);
	freqMark3.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 3)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 4, spPosY, 100 + (spWidth / 10) * 4, spPosY2);
	freqMark4.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 4)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 5, spPosY, 100 + (spWidth / 10) * 5, spPosY2);
	freqMark5.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 5)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 6, spPosY, 100 + (spWidth / 10) * 6, spPosY2);
	freqMark6.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 6)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 7, spPosY, 100 + (spWidth / 10) * 7, spPosY2);
	freqMark7.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 7)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 8, spPosY, 100 + (spWidth / 10) * 8, spPosY2);
	freqMark8.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 8)), juce::NotificationType::dontSendNotification);
	g.drawLine(100 + (spWidth / 10) * 9, spPosY, 100 + (spWidth / 10) * 9, spPosY2);
	freqMark9.setText(juce::String::String((int)cordValueToFreq((spWidth / 10) * 9)), juce::NotificationType::dontSendNotification);
}


/** Draws frequency response for LP, HP and BP filter.
* Response is drawn only for enabled filters.
* @param g - graphical context to use for drawing
*/
void MainComponent::drawFilterResponse(Graphics &g)
{
	if (lpfEnabled.get() == true)
	{
		//line is drawn every 8 points - saves resources
		g.setColour(Colours::indianred);
		for (int i = 0; i < 928; i+=8)
		{	
			//line is drawn between 2 neighbouring points of response
			float mr = filter.magnitudeResponseLP(cordValueToFreq(i));
			float mr2 = filter.magnitudeResponseLP(cordValueToFreq(i + 8));
			if (spPosY2 - (mr * 85) > spPosY)
			{
				//amplitude of the line is 0.75x spectral window height
				if (409 - (mr * 127) >= spPosY && 409 - (mr2 * 127) >= spPosY)
					g.drawLine(i + 100, 409 - (mr * 127), i + 108, 409 - (mr2 * 127));
			}
		}
	}

	if (hpfEnabled.get() == true)
	{
		g.setColour(Colours::aquamarine);
		for (int i = 0; i < 928; i+=8)
		{
			float mr = filter.magnitudeResponseHP(cordValueToFreq(i));
			float mr2 = filter.magnitudeResponseHP(cordValueToFreq(i + 8));
			if (spPosY2 - (mr * 85) > spPosY)
			{
				if (409 - (mr * 127) >= spPosY && 409 - (mr2 * 127) >= spPosY)
					g.drawLine(i + 100, 409 - (mr * 127), i + 108, 409 - (mr2 * 127));
			}
		}
	}

	if (bpfEnabled.get() == true)
	{
		g.setColour(Colours::yellowgreen);
		for (int i = 0; i < 928; i+=8)
		{
			float mr = filter.magnitudeResponseBP(cordValueToFreq(i));
			float mr2 = filter.magnitudeResponseBP(cordValueToFreq(i + 8));
			if (spPosY2 - (mr * 85) > spPosY)
			{
				if (409 - (mr * 127) >= spPosY && 409 - (mr2 * 127) >= spPosY)
					g.drawLine(i + 100, 409 - (mr * 127), i + 108, 409 - (mr2 * 127));
			}
		}
	}
}


/** Drawing of freq indicator for chosen coordinate in freq spectrum window
* @param g - graphical context to use for drawing
*/
void MainComponent::drawFreqIndicator(Graphics& g)
{
	g.setColour(Colour((uint8)240, (uint8)236, (uint8)60, (uint8)255));
	g.drawLine(100 + freqPosition, spPosY, 100 + freqPosition, spPosY2);
}


/** Sets the position to use with freq indicator. Called in mouse click event (not in the paint()), 
* because indicator should stay in the freq spectrum window, when user clicks out of its bounds
*/
void MainComponent::setFreqIndicatorPosition()
{
	if (mouseClickXCord < 930 && mouseClickXCord > 0 && mouseClickYCord < spPosY2 && mouseClickYCord > spPosY)
	{
		freqPosition = mouseClickXCord;
	}	
}


/** Sets position of the playback used in playback functions and in drawing the playback indicator. 
* Called in mouse click event.
*/
void MainComponent::setPlaybackPosition()
{
	if (mouseClickXCord < 1090 && mouseClickXCord > 0 && mouseClickYCord < 160 && mouseClickYCord > 10)
	{
		position = fileBuffer.getNumSamples() * (mouseClickXCord / 1090.0f);
	}	
}


/** Converts coordinate value in freq spectrum window to corresponding frequency value
*/
float MainComponent::cordValueToFreq(float cordValue)
{
	return (std::log(1.0f - (float)cordValue / (float)spWidth) * spectralResolutionCoef) * -22050;		
}


/** Converts frequency value to corresponding value in freq spectrum window 
*/
float MainComponent::freqValueToCord(float freqValue)
{
	freqValue /= adsetup.sampleRate;
	return 790 * (std::pow(freqValue, 5) - 5 * std::pow(freqValue, 4) + 10 * std::pow(freqValue, 3) - 10 * std::pow(freqValue, 2) + 5 * freqValue);
}


/** Positioning of graphical components for effect controls
* Called when the window is refreshed (also at the start of the program)
*/
void MainComponent::resized()
{
	playButton.setBounds(8, 10, 42, 30);
	stopButton.setBounds(52, 10, 42, 30);

	fileOpenSaveLabel.setBounds(8, 50, 84, 25);
	openButton.setBounds(8, 80, 42, 30);
	saveToFileButton.setBounds(52, 80, 42, 30);

	presetOpenSaveLabel.setBounds(8, 120, 84, 25);
	openPresetButton.setBounds(8, 150, 42, 30);
	savePresetButton.setBounds(52, 150, 42, 30);

	forwardPlaybackButton.setBounds(685, 170, 30, 30);
	backwardPlaybackButton.setBounds(575, 170, 30, 30);
	playbackShiftLength.setBounds(615, 169, 60, 29);
	loopButton.setBounds(725, 170, 50, 30);

	spectralResolutionLabel.setBounds(getWidth() - 160, getHeight() - 440, 70, 25);
	spectralResolutionSlider.setBounds(getWidth() - 160, getHeight() - 410, 70, 130);

	fftFreqDisplayLabel.setBounds(getWidth() - 80, getHeight() - 440, 70, 25);
	fftFreqDisplayText.setBounds(getWidth() - 80, getHeight() - 410, 70, 30);

	freqMark1.setBounds(65 + (spWidth / 10) * 1, 213, 70, 25);
	freqMark2.setBounds(65 + (spWidth / 10) * 2, 213, 70, 25);
	freqMark3.setBounds(65 + (spWidth / 10) * 3, 213, 70, 25);
	freqMark4.setBounds(65 + (spWidth / 10) * 4, 213, 70, 25);
	freqMark5.setBounds(65 + (spWidth / 10) * 5, 213, 70, 25);
	freqMark6.setBounds(65 + (spWidth / 10) * 6, 213, 70, 25);
	freqMark7.setBounds(65 + (spWidth / 10) * 7, 213, 70, 25);
	freqMark8.setBounds(65 + (spWidth / 10) * 8, 213, 70, 25);
	freqMark9.setBounds(65 + (spWidth / 10) * 9, 213, 70, 25);

	freqBaseValueLabel.setBounds(101, 257, 40, 25);

	processBandpassFilterButton.setBounds(getWidth() - 80, getHeight() - 250, 70, 25);
	BandpassFreqLabel.setBounds(getWidth() - 80, getHeight() - 230, 70, 25);
	BandpassFreqSlider.setBounds(getWidth() - 80, getHeight() - 200, 70, 70);
	BandpassQualityLabel.setBounds(getWidth() - 80, getHeight() - 120, 70, 25);
	BandpassQualitySlider.setBounds(getWidth() - 80, getHeight() - 90, 70, 70);

	processHighpassFilterButton.setBounds(getWidth() - 160, getHeight() - 250, 70, 25);
	HighpassFreqLabel.setBounds(getWidth() - 160, getHeight() - 230, 70, 25);
	HighpassFreqSlider.setBounds(getWidth() - 160, getHeight() - 200, 70, 70);
	HighpassQualityLabel.setBounds(getWidth() - 160, getHeight() - 120, 70, 25);
	HighpassQualitySlider.setBounds(getWidth() - 160, getHeight() - 90, 70, 70);

	processLowpassFilterButton.setBounds(getWidth() - 240, getHeight() - 250, 70, 25);
	LowpassFreqLabel.setBounds(getWidth() - 240, getHeight() - 230, 70, 25);
	LowpassFreqSlider.setBounds(getWidth() - 240, getHeight() - 200, 70, 70);
	LowpassQualityLabel.setBounds(getWidth() - 240, getHeight() - 120, 70, 25);
	LowpassQualitySlider.setBounds(getWidth() - 240, getHeight() - 90, 70, 70);

	processReverbButton.setBounds(getWidth() - 420, getHeight() - 250, 150, 25);
	reverbBalanceLabel.setBounds(getWidth() - 420, getHeight() - 230, 70, 25);
	reverbBalanceSlider.setBounds(getWidth() - 420, getHeight() - 200, 70, 70);
	reverbSizeLabel.setBounds(getWidth() - 420, getHeight() - 120, 70, 25);
	reverbSizeSlider.setBounds(getWidth() - 420, getHeight() - 90, 70, 70);
	reverbDampeningLabel.setBounds(getWidth() - 340, getHeight() - 230, 70, 25);
	reverbDampeningSlider.setBounds(getWidth() - 340, getHeight() - 200, 70, 70);
	reverbWidthLabel.setBounds(getWidth() - 340, getHeight() - 120, 70, 25);
	reverbWidthSlider.setBounds(getWidth() - 340, getHeight() - 90, 70, 70);

	processHardclipDistortionButton.setBounds(getWidth() - 520, getHeight() - 250, 70, 25);
	hcdThresholdLabel.setBounds(getWidth() - 520, getHeight() - 230, 70, 25);
	hcdThresholdSlider.setBounds(getWidth() - 520, getHeight() - 200, 70, 70);

	processFullrectDistortionButton.setBounds(getWidth() - 520, getHeight() - 90, 70, 50);

	processSoftclipDistortionButton.setBounds(getWidth() - 620, getHeight() - 250, 70, 25);
	scdThresholdLabel.setBounds(getWidth() - 620, getHeight() - 230, 70, 25);
	scdThresholdSlider.setBounds(getWidth() - 620, getHeight() - 200, 70, 70);

	processHalfrectDistortionButton.setBounds(getWidth() - 620, getHeight() - 90, 70, 50);

	processExtractorButton.setBounds(getWidth() - 720, getHeight() - 250, 70, 25);
	extractorIntensityLabel.setBounds(getWidth() - 720, getHeight() - 230, 70, 25);
	extractorIntensitySlider.setBounds(getWidth() - 720, getHeight() - 200, 70, 70);
	extractorWidthLabel.setBounds(getWidth() - 720, getHeight() - 120, 70, 25);
	extractorWidthSlider.setBounds(getWidth() - 720, getHeight() - 90, 70, 70);

	processReverzButton.setBounds(getWidth() - 820, getHeight() - 250, 70, 25);
	reverzAmountLabel.setBounds(getWidth() - 820, getHeight() - 230, 70, 25);
	reverzAmountSlider.setBounds(getWidth() - 820, getHeight() - 200, 70, 70);
	reverzSkewLabel.setBounds(getWidth() - 820, getHeight() - 120, 70, 25);
	reverzSkewSlider.setBounds(getWidth() - 820, getHeight() - 90, 70, 70);

	processStutterButton.setBounds(getWidth() - 1000, getHeight() - 250, 150, 25);
	stutterAmountLabel.setBounds(getWidth() - 920, getHeight() - 230, 70, 25);
	stutterAmountSlider.setBounds(getWidth() - 920, getHeight() - 200, 70, 70);
	stutterChorusLabel.setBounds(getWidth() - 920, getHeight() - 120, 70, 25);
	stutterChorusSlider.setBounds(getWidth() - 920, getHeight() - 90, 70, 70);
	stutterDelayLabel.setBounds(getWidth() - 1000, getHeight() - 230, 70, 25);
	stutterDelaySlider.setBounds(getWidth() - 1000, getHeight() - 200, 70, 70);

	processShifterButton.setBounds(getWidth() - 1100, getHeight() - 250, 70, 25);
	shifterAmountLabel.setBounds(getWidth() - 1100, getHeight() - 230, 70, 25);
	shifterAmountSlider.setBounds(getWidth() - 1100, getHeight() - 200, 70, 70);
	shifterToneLabel.setBounds(getWidth() - 1100, getHeight() - 120, 70, 25);
	shifterToneSlider.setBounds(getWidth() - 1100, getHeight() - 90, 70, 70);

	clippingLabel.setBounds(8, 190, 84, 30);
	bitDepthLabel.setBounds(8, 230, 84, 30);

	processPitchButton.setBounds(10, getHeight() - 230, 70, 25);
	pitchSlider.setBounds(10, getHeight() - 200, 70, 70);

	processGainButton.setBounds(10, getHeight() - 120, 70, 25);
	gainSlider.setBounds(10, getHeight() - 90, 70, 70);

	bitDepthSlider.setBounds(8, 265, 84, 45);

	resetEffectButton.setBounds(8, 325, 84, 30);

	toggleTooltipButton.setBounds(8, 370, 84, 30);

	spectralResolutionSlider.onValueChange = [this] { spectralResolutionSliderChanged(); };

	//call processing of all effects on any effects slider value change
	scdThresholdSlider.onDragEnd = [this] { processAllEffects(0); };
	hcdThresholdSlider.onDragEnd = [this] { processAllEffects(1); };
	extractorIntensitySlider.onDragEnd = [this] { processAllEffects(4); };
	extractorWidthSlider.onDragEnd = [this] { processAllEffects(4); };
	reverzSkewSlider.onDragEnd = [this] { processAllEffects(5); };
	reverzAmountSlider.onDragEnd = [this] { processAllEffects(5); };
	stutterAmountSlider.onDragEnd = [this] { processAllEffects(6); };
	stutterChorusSlider.onDragEnd = [this] { processAllEffects(6); };
	stutterDelaySlider.onDragEnd = [this] { processAllEffects(6); };
	shifterAmountSlider.onDragEnd = [this] { processAllEffects(7); };
	shifterToneSlider.onDragEnd = [this] { processAllEffects(7); };
	reverbBalanceSlider.onDragEnd = [this] { processAllEffects(8); };
	reverbSizeSlider.onDragEnd = [this] { processAllEffects(8); };
	reverbDampeningSlider.onDragEnd = [this] { processAllEffects(8); };
	reverbWidthSlider.onDragEnd = [this] { processAllEffects(8); };
	LowpassFreqSlider.onDragEnd = [this] { processAllEffects(9); };
	HighpassFreqSlider.onDragEnd = [this] { processAllEffects(9); };
	BandpassFreqSlider.onDragEnd = [this] { processAllEffects(9); };
	LowpassQualitySlider.onDragEnd = [this] { processAllEffects(9); };
	HighpassQualitySlider.onDragEnd = [this] { processAllEffects(9); };
	BandpassQualitySlider.onDragEnd = [this] { processAllEffects(9); };
	pitchSlider.onDragEnd = [this] { pitchStop.set(true);  processAllEffects(10); };
	gainSlider.onDragEnd = [this] { processAllEffects(11); };
}


/** Opens the file asynchronously via default Windows explorer window
*/
void MainComponent::openButtonClicked(void)
{
	chooser = std::make_unique<juce::FileChooser>("Select a file to open...", juce::File{}, "*.wav;*.mp3;*.flac");
	
	auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

	chooser->launchAsync(chooserFlags, [&](const FileChooser& fc)
	{
		auto file = chooser->getResult();

		std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
		
		if (reader.get() != nullptr)
		{
			processOpenedFile(file, move(chooser), move(reader));
		}
	});
}


/** Saves the final output buffer to a .wav file.
* Bit depth is adjustable with bit depth buttons
*/
void MainComponent::saveToFileButtonClicked(void)
{
	chooser = std::make_unique<juce::FileChooser>("Select a file to save to...", juce::File{}, "*.wav; *.flac");
	auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

	chooser->launchAsync(chooserFlags, [&](const FileChooser& fc)
	{
		//set out bit depth from toggle buttons
		int outBitDepth = 16;

		outBitDepth = (int)bitDepthSlider.getValue();

		auto file = chooser->getResult();

		if (file.getFileName().isNotEmpty())
		{
			if (file.getFileExtension().contains(".wav"))
			{
				WavAudioFormat wavFileFormat;
				OptionalScopedPointer <AudioFormatWriter> fileWriter;
				fileWriter.reset();
				fileWriter.set(wavFileFormat.createWriterFor(new FileOutputStream(file), 44100.0, fileBuffer.getNumChannels(), outBitDepth, {}, 0), true);
				if (fileWriter != nullptr)
					fileWriter->writeFromAudioSampleBuffer(fileBuffer, 0, fileBuffer.getNumSamples());
			}
			else if (file.getFileExtension().contains(".flac"))
			{
				FlacAudioFormat flacFileFormat;
				OptionalScopedPointer <AudioFormatWriter> fileWriter;
				fileWriter.reset();
				fileWriter.set(flacFileFormat.createWriterFor(new FileOutputStream(file), 44100.0, fileBuffer.getNumChannels(), outBitDepth, {}, 0), true);
				if (fileWriter != nullptr)
					fileWriter->writeFromAudioSampleBuffer(fileBuffer, 0, fileBuffer.getNumSamples());
			}
			
		}
	});
}


/** Handles file drag and drop functionality
* @param x - mouse position relative to the component
* @param y - mouse position relative to the component
* @param &files - files that are being drag'n'dropped
*/
void MainComponent::filesDropped(const StringArray& files, int x, int y)
{
	//accept only the first dropped file (if multiple are selected)
	auto filename = files.strings.getFirst();
	auto file = File::File(filename);
	std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
	processOpenedFile(file, nullptr, move(reader));
}


/** Determines if the component is even interested in drag'n'dropped file
* @param &files - files that are being drag'n'dropped
*/
bool MainComponent::isInterestedInFileDrag(const StringArray& files)
{
	return true;
}


/** Gets a file's parameters and saves the content into each of the effect's buffers. 
* All controls are enabled and the filebuffer content is set to be drawn
* @param file - file to process
* @param chooser - file chooser (from opening the file via openButton)
* @param reader - file reader (holds file's parameters)
*/
void MainComponent::processOpenedFile(File file, std::unique_ptr<juce::FileChooser> chooser, std::unique_ptr<AudioFormatReader> reader)
{
	//resetting flags
	stopFlag.set(true);
	pauseFlag.set(true);
	lpfEnabled.set(false);
	hpfEnabled.set(false);
	bpfEnabled.set(false);
	scdEnabled.set(false);
	hcdEnabled.set(false);
	frdEnabled.set(false);
	hrdEnabled.set(false);
	reverbEnabled.set(false);
	extractorEnabled.set(false);
	reverzEnabled.set(false);
	stutterEnabled.set(false);
	shifterEnabled.set(false);
	pitchEnabled.set(false);
	gainEnabled.set(false);
	isClipping.set(false);
	loopEnabled.set(false);
	tooltipEnabled.set(false);

	//resetting audio source
	shutdownAudio();
	auto duration = reader->lengthInSamples / reader->sampleRate;

	//Maximum length of input audio track - 60
	if (duration < 61)
	{
		fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
		reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
		position = 0;
		bitDepth = reader->bitsPerSample;

		//copying file to the input of effects
		inputBuffer.makeCopyOf(fileBuffer);
		scdistortionBuffer.makeCopyOf(fileBuffer);
		hcdistortionBuffer.makeCopyOf(fileBuffer);
		frdistortionBuffer.makeCopyOf(fileBuffer);
		hrdistortionBuffer.makeCopyOf(fileBuffer);
		reverbBuffer.makeCopyOf(fileBuffer);
		extractorBuffer.makeCopyOf(fileBuffer);
		reverzBuffer.makeCopyOf(fileBuffer);
		stutterBuffer.makeCopyOf(fileBuffer);
		shifterBuffer.makeCopyOf(fileBuffer);
		filterBuffer.makeCopyOf(fileBuffer);
		pitchBuffer.makeCopyOf(fileBuffer);
		gainBuffer.makeCopyOf(fileBuffer);

		//showing the file bit depth
		bitDepthSlider.setValue(bitDepth);

		//enabling gui components
		playButton.setEnabled(true);
		saveToFileButton.setEnabled(true);
		openPresetButton.setEnabled(true);
		savePresetButton.setEnabled(true);
		forwardPlaybackButton.setEnabled(true);
		backwardPlaybackButton.setEnabled(true);
		playbackShiftLength.setEnabled(true);
		loopButton.setEnabled(true);
		spectralResolutionSlider.setEnabled(true);
		resetEffectButton.setEnabled(true);
		bitDepthSlider.setEnabled(true);
		processGainButton.setEnabled(true);
		gainSlider.setEnabled(true);
		processReverbButton.setEnabled(true);
		reverbBalanceSlider.setEnabled(true);
		reverbSizeSlider.setEnabled(true);
		reverbWidthSlider.setEnabled(true);
		reverbDampeningSlider.setEnabled(true);
		processSoftclipDistortionButton.setEnabled(true);
		processHardclipDistortionButton.setEnabled(true);
		processFullrectDistortionButton.setEnabled(true);
		processHalfrectDistortionButton.setEnabled(true);
		scdThresholdSlider.setEnabled(true);
		hcdThresholdSlider.setEnabled(true);
		processLowpassFilterButton.setEnabled(true);
		processHighpassFilterButton.setEnabled(true);
		processBandpassFilterButton.setEnabled(true);
		HighpassFreqSlider.setEnabled(true);
		LowpassFreqSlider.setEnabled(true);
		BandpassFreqSlider.setEnabled(true);
		HighpassQualitySlider.setEnabled(true);
		LowpassQualitySlider.setEnabled(true);
		BandpassQualitySlider.setEnabled(true);
		processExtractorButton.setEnabled(true);
		extractorIntensitySlider.setEnabled(true);
		extractorWidthSlider.setEnabled(true);
		processReverzButton.setEnabled(true);
		reverzSkewSlider.setEnabled(true);
		reverzAmountSlider.setEnabled(true);
		processStutterButton.setEnabled(true);
		stutterAmountSlider.setEnabled(true);
		stutterChorusSlider.setEnabled(true);
		stutterDelaySlider.setEnabled(true);
		processShifterButton.setEnabled(true);
		shifterAmountSlider.setEnabled(true);
		shifterToneSlider.setEnabled(true);
		processPitchButton.setEnabled(true);
		pitchSlider.setEnabled(true);
		toggleTooltipButton.setEnabled(true);

		pitchStop.set(false);

		//resetting waveform zoom
		zoomFactor = 0.00f;

		//setting up the source for waveform display

		thumbnail.setSource(new FileInputSource(file));
		setAudioChannels(0, (int)reader->numChannels);
		stopFlag.set(true);

		//process effects when new file is loaded, because they couldve been set while editing previous file
		processAllEffects(0);
	}
}


/** Processes interaction with play button
* Sets the flags to determine the behavior of the play button
*/
void MainComponent::playButtonClicked(void)
{
	if (pauseFlag.get() == true)
	{
		changeState(TransportState::Playing);
	} 
	else if (pauseFlag.get() == false)
	{
		pauseFlag.set(true);
		changeState(TransportState::Paused);
	}
}

/** Processes interaction with stop button
* stopFlag is used to stop the playback of the audio
*/
void MainComponent::stopButtonClicked(void)
{
	stopFlag.set(true);
	pauseFlag.set(true);
	changeState(TransportState::Stopped);
}


/** Processes interaction with forwardPlayback button. 
* sets position used in getNextAudioBlock()
*/
void MainComponent::forwardPlaybackButtonClicked(void) 
{
	if (position + positionShiftLength > fileBuffer.getNumSamples())
	{
		position = fileBuffer.getNumSamples();
	}
	else
	{
		position += positionShiftLength;
	}

	repaint();
}


/** Processes interaction with backwardPlayback button. 
* sets position used in getNextAudioBlock()
*/
void MainComponent::backwardPlaybackButtonClicked(void)
{
	if (position - positionShiftLength < 0)
	{
		position = 0;
	}
	else
	{
		position -= positionShiftLength;
	}
	
	repaint();
}


/** Sets shift length for forwardPlaybackButtonClicked() and backwardPlaybackButtonClicked()
*/
void MainComponent::playbackShiftLengthChanged(void)
{
	positionShiftLength = (playbackShiftLength.getText()).getIntValue();

	if (positionShiftLength < 0)
	{
		positionShiftLength = 0;
		playbackShiftLength.setText(juce::String::String("0"));
	}
}


/** Processes interaction with loop button.
* Enables / disables looping playback mode
*/
void MainComponent::loopButtonClicked(void)
{
	if (loopEnabled.get() == false)
	{
		loopEnabled.set(true);
		loopButton.setColour(TextButton::buttonColourId, Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
	}
	else if (loopEnabled.get() == true)
	{
		loopEnabled.set(false);
		loopButton.setColour(TextButton::buttonColourId, buttonColour);
	}
}


/** Applies coefficient for spectrum visualiser distribution
*/
void MainComponent::spectralResolutionSliderChanged(void)
{
	spectralResolutionCoef = spectralResolutionSlider.getValue() * 0.05f;

	//explicit repaint() so the filter frequency response is redrawn
	repaint();
}


/** Main method for processing mouse click on effect buttons
* @param effectButton - effect Button to process
* @param effectEnabled - state of the button
*/
Atomic<bool> MainComponent::processEffectButtonClicked(TextButton& effectButton, Atomic<bool> effectEnabled)
{
	if (effectEnabled.get() == false)
	{
		effectEnabled.set(true);
		effectButton.setColour(TextButton::buttonColourId, Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
	}
	else if (effectEnabled.get() == true)
	{
		effectEnabled.set(false);
		effectButton.setColour(TextButton::buttonColourId, buttonColour);
	}

	return effectEnabled;
}


/** Method for processing clipping in the effect output buffer
* @param effectBuffer - effect buffer to process
*/
void MainComponent::processEffectClipping(AudioBuffer <float> effectBuffer)
{
	if (effectBuffer.getMagnitude(0, effectBuffer.getNumSamples()) > 1.00f)
	{
		isClipping.set(true);
		clippingLabel.setColour(Label::textColourId, Colours::indianred);
		clippingLabel.setText("Clipping!!!", NotificationType::dontSendNotification);
	}
	else
	{
		clippingLabel.setColour(Label::textColourId, Colours::limegreen);
		clippingLabel.setText("No Clipping", NotificationType::dontSendNotification);
	}
}


/** Enables or disables the reverb effect
*/
void MainComponent::processReverbButtonClicked()
{
	reverbEnabled = processEffectButtonClicked(processReverbButton, reverbEnabled);
	processAllEffects(8);
}

/** Applies reverb effect on value change
*	Called when the value of any slider is changed 
*/
void MainComponent::processReverbSliderChange(void)
{
	reverb.setInputBuffer(shifterBuffer);
	reverb.setupReverb(reverbDampeningSlider.getValue(), reverbBalanceSlider.getValue(), reverbSizeSlider.getValue(), reverbWidthSlider.getValue());
	reverbBuffer = reverb.addReverb(reverbEnabled);

	processEffectClipping(reverbBuffer);
}


/** Toggles softclip effect
*/
void MainComponent::processSoftclipDistortionButtonClicked(void)
{
	scdEnabled = processEffectButtonClicked(processSoftclipDistortionButton, scdEnabled);
	processAllEffects(0);
}

/** Applies softclip effect on value change
*	Called when the value of any slider is changed
*/
void MainComponent::processSoftclipDistortionSliderChange(void)
{
	distortion.setInputBuffer(inputBuffer);
	distortion.setSoftclipThresholdValue(scdThresholdSlider.getValue());
	scdistortionBuffer = distortion.addSoftclipDistortion(scdEnabled);

	processEffectClipping(scdistortionBuffer);
}


/** Toggles hardclip effect
*/
void MainComponent::processHardclipDistortionButtonClicked(void)
{
	hcdEnabled = processEffectButtonClicked(processHardclipDistortionButton, hcdEnabled);
	processAllEffects(1);
}

/** Applies hardclip effect on value change
*	Called when the value of any slider is changed
*/
void MainComponent::processHardclipDistortionSliderChange(void)
{
	distortion.setInputBuffer(scdistortionBuffer);
	distortion.setHardclipThresholdValue(hcdThresholdSlider.getValue());
	hcdistortionBuffer = distortion.addHardclipDistortion(hcdEnabled);

	processEffectClipping(hcdistortionBuffer);
}


/** Toggles Fullrect effect
*/
void MainComponent::processFullrectDistortionButtonClicked(void)
{
	frdEnabled = processEffectButtonClicked(processFullrectDistortionButton, frdEnabled);
	processAllEffects(2);
}

void MainComponent::processFullrectDistortionSliderChange(void)
{
	distortion.setInputBuffer(hcdistortionBuffer);
	frdistortionBuffer = distortion.addFullrectDistortion(frdEnabled);

	processEffectClipping(frdistortionBuffer);
}


/** Toggles Halfrect effect
*/
void MainComponent::processHalfrectDistortionButtonClicked(void)
{
	hrdEnabled = processEffectButtonClicked(processHalfrectDistortionButton, hrdEnabled);
	processAllEffects(3);
}

void MainComponent::processHalfrectDistortionSliderChange(void)
{
	distortion.setInputBuffer(frdistortionBuffer);
	hrdistortionBuffer = distortion.addHalfrectDistortion(hrdEnabled);

	processEffectClipping(hrdistortionBuffer);
}


/** Toggles Extractor effect state
*/
void MainComponent::processExtractorButtonClicked(void)
{
	extractorEnabled = processEffectButtonClicked(processExtractorButton, extractorEnabled);
	processAllEffects(4);
}

/** Applies Extractor on the value change (with parameters from sliders)
* Is called on every one of the effect's sliders
*/
void MainComponent::processExtractorSliderChange(void)
{
	extractor.setInputBuffer(hrdistortionBuffer);
	extractor.setupExtractor(extractorIntensitySlider.getValue(), extractorWidthSlider.getValue());
	extractorBuffer = extractor.addExtractor(extractorEnabled);

	processEffectClipping(extractorBuffer);
}


/** Toggles Shifter effect state
*/
void MainComponent::processReverzButtonClicked(void)
{
	reverzEnabled = processEffectButtonClicked(processReverzButton, reverzEnabled);
	processAllEffects(5);
}

/** Applies shifter effect on its pitch or color slider change
* Is called on every one of the effect's sliders
*/
void MainComponent::processReverzSliderChange(void)
{
	reverz.setInputBuffer(extractorBuffer);
	reverz.setupReverz(reverzSkewSlider.getValue(), roundDoubleToInt(reverzAmountSlider.getValue()));
	reverzBuffer = reverz.addReverz(reverzEnabled);

	processEffectClipping(reverzBuffer);
}


/** Toggles Stutter effect state
*/
void MainComponent::processStutterButtonClicked(void)
{
	stutterEnabled = processEffectButtonClicked(processStutterButton, stutterEnabled);
	processAllEffects(6);
}

/** Applies Stutter effect on its sliders change
* Is called on every one of the effect's sliders
*/
void MainComponent::processStutterSliderChange(void)
{
	stutter.setInputBuffer(reverzBuffer);
	stutter.setupStutter(stutterAmountSlider.getValue(), stutterChorusSlider.getValue(), stutterDelaySlider.getValue());
	stutterBuffer = stutter.addStutter(stutterEnabled);

	processEffectClipping(stutterBuffer);
}


/** Toggles Crusher effect state
*/
void MainComponent::processShifterButtonClicked(void)
{
	shifterEnabled = processEffectButtonClicked(processShifterButton, shifterEnabled);
	processAllEffects(7);
}

/** Applies Crusher effect on its sliders change
* Is called on every one of the effect's sliders
*/
void MainComponent::processShifterSliderChange(void)
{
	shifter.setInputBuffer(stutterBuffer);
	shifter.setupShifter(shifterAmountSlider.getValue(), shifterToneSlider.getValue());
	shifterBuffer = shifter.addShifter(shifterEnabled);

	processEffectClipping(shifterBuffer);
}


/** Applies filters on the value change (with parameters from sliders)
* Is called on every one of the filter's sliders
*/
void MainComponent::processFiltersSliderChange(void)
{
	filter.setInputBuffer(reverbBuffer);
	filter.setLPCutoffCurrentValue(LowpassFreqSlider.getValue());
	filter.setHPCutoffCurrentValue(HighpassFreqSlider.getValue());
	filter.setBPCutoffCurrentValue(BandpassFreqSlider.getValue());
	filter.setLPQualityCurrentValue(LowpassQualitySlider.getValue());
	filter.setHPQualityCurrentValue(HighpassQualitySlider.getValue());
	filter.setBPQualityCurrentValue(BandpassQualitySlider.getValue());
	filter.setActiveFilters(lpfEnabled, hpfEnabled, bpfEnabled);
	filterBuffer = filter.addFilters();

	processEffectClipping(filterBuffer);
}

/** Toggles LP filter state
*/
void MainComponent::processLowpassFilterButtonClicked(void)
{
	lpfEnabled = processEffectButtonClicked(processLowpassFilterButton, lpfEnabled);
	processAllEffects(9);
}

/** Toggles HP filter state
*/
void MainComponent::processHighpassFilterButtonClicked(void)
{
	hpfEnabled = processEffectButtonClicked(processHighpassFilterButton, hpfEnabled);
	processAllEffects(9);
}

/** Toggles BP filter state
*/
void MainComponent::processBandpassFilterButtonClicked(void)
{
	bpfEnabled = processEffectButtonClicked(processBandpassFilterButton, bpfEnabled);
	processAllEffects(9);
}


/** Toggles the pitch effect state
*/
void MainComponent::processPitchButtonClicked(void)
{
	pitchStop.set(true);

	pitchEnabled = processEffectButtonClicked(processPitchButton, pitchEnabled);
	
	if (stopFlag.get() == false && pitchStop.get() == true)
	{
		pauseFlag.set(true);
		changeState(TransportState::Paused);
	}

	processAllEffects(10);

	pitchStop.set(false);
}

/** Applies pitch on its slider value change
*/
void MainComponent::processPitchSliderChange(void)
{
	//pitch effect changes buffers length, so it will cause access violation if playing is not stopped
	if (stopFlag.get() == false && pitchStop.get() == true)
	{
		pauseFlag.set(true);
		changeState(TransportState::Paused);
	}

	pitch.setInputBuffer(filterBuffer);
	pitch.setupPitch(pitchSlider.getValue());
	pitchBuffer = pitch.addPitch(pitchEnabled);

	processEffectClipping(pitchBuffer);

	pitchStop.set(false);
}


/** Toggles the gain effect state
*/
void MainComponent::processGainButtonClicked(void)
{
	gainEnabled = processEffectButtonClicked(processGainButton, gainEnabled);
	processAllEffects(11);
}

/** Applies gain on its slider value change
*/
void MainComponent::processGainSliderChange(void)
{
	gain.setInputBuffer(pitchBuffer);
	gain.setupGain(gainSlider.getValue());
	fileBuffer = gain.addGain(gainEnabled);

	processEffectClipping(fileBuffer);
}


/** Processing of all effects in series
* Order of the processing chain is not changeable
*/
void MainComponent::processAllEffects(int effectIndex)
{
	if (effectIndex < 1)
		processSoftclipDistortionSliderChange();
	if (effectIndex < 2)
		processHardclipDistortionSliderChange();
	if (effectIndex < 3)
		processFullrectDistortionSliderChange();
	if (effectIndex < 4)
		processHalfrectDistortionSliderChange();
	if (effectIndex < 5)
		processExtractorSliderChange();
	if (effectIndex < 6)
		processReverzSliderChange();
	if (effectIndex < 7)
		processStutterSliderChange();
	if (effectIndex < 8)
		processShifterSliderChange();
	if (effectIndex < 9)
		processReverbSliderChange();
	if (effectIndex < 10)
		processFiltersSliderChange();
	if (effectIndex < 11)
		processPitchSliderChange();
	if (effectIndex < 12)
		processGainSliderChange();

	thumbnail.reset(fileBuffer.getNumChannels(), 44100.0, fileBuffer.getNumSamples());
	thumbnail.addBlock(0, fileBuffer, 0, fileBuffer.getNumSamples());
}


/** Resets all effects to default state
*/
void MainComponent::resetEffectButtonClicked()
{
	gainSlider.setValue(gainSlider.getDoubleClickReturnValue());
	pitchSlider.setValue(pitchSlider.getDoubleClickReturnValue());
	stutterAmountSlider.setValue(stutterAmountSlider.getDoubleClickReturnValue());
	stutterChorusSlider.setValue(stutterChorusSlider.getDoubleClickReturnValue());
	stutterDelaySlider.setValue(stutterDelaySlider.getDoubleClickReturnValue());
	shifterAmountSlider.setValue(shifterAmountSlider.getDoubleClickReturnValue());
	shifterToneSlider.setValue(shifterToneSlider.getDoubleClickReturnValue());
	reverzAmountSlider.setValue(reverzAmountSlider.getDoubleClickReturnValue());
	reverzSkewSlider.setValue(reverzSkewSlider.getDoubleClickReturnValue());
	extractorIntensitySlider.setValue(extractorIntensitySlider.getDoubleClickReturnValue());
	extractorWidthSlider.setValue(extractorWidthSlider.getDoubleClickReturnValue());
	reverbBalanceSlider.setValue(reverbBalanceSlider.getDoubleClickReturnValue());
	reverbDampeningSlider.setValue(reverbDampeningSlider.getDoubleClickReturnValue());
	reverbSizeSlider.setValue(reverbSizeSlider.getDoubleClickReturnValue());
	reverbWidthSlider.setValue(reverbWidthSlider.getDoubleClickReturnValue());
	LowpassFreqSlider.setValue(LowpassFreqSlider.getDoubleClickReturnValue());
	LowpassQualitySlider.setValue(LowpassQualitySlider.getDoubleClickReturnValue());
	HighpassFreqSlider.setValue(HighpassFreqSlider.getDoubleClickReturnValue());
	HighpassQualitySlider.setValue(HighpassQualitySlider.getDoubleClickReturnValue());
	BandpassFreqSlider.setValue(BandpassFreqSlider.getDoubleClickReturnValue());
	BandpassQualitySlider.setValue(BandpassQualitySlider.getDoubleClickReturnValue());
	scdThresholdSlider.setValue(scdThresholdSlider.getDoubleClickReturnValue());
	hcdThresholdSlider.setValue(hcdThresholdSlider.getDoubleClickReturnValue());
	spectralResolutionSlider.setValue(spectralResolutionSlider.getDoubleClickReturnValue());

	if (lpfEnabled.get() == true)
		processLowpassFilterButtonClicked();
	if (hpfEnabled.get() == true)
		processHighpassFilterButtonClicked();
	if (bpfEnabled.get() == true)
		processBandpassFilterButtonClicked();
	if (reverbEnabled.get() == true)
		processReverbButtonClicked();
	if (hcdEnabled.get() == true)
		processHardclipDistortionButtonClicked();
	if (scdEnabled.get() == true)
		processSoftclipDistortionButtonClicked();
	if (hrdEnabled.get() == true)
		processHalfrectDistortionButtonClicked();
	if (frdEnabled.get() == true)
		processFullrectDistortionButtonClicked();
	if (extractorEnabled.get() == true)
		processExtractorButtonClicked();
	if (reverzEnabled.get() == true)
		processReverzButtonClicked();
	if (stutterEnabled.get() == true)
		processStutterButtonClicked();
	if (shifterEnabled.get() == true)
		processShifterButtonClicked();
	if (pitchEnabled.get() == true)
		processPitchButtonClicked();
	if (gainEnabled.get() == true)
		processGainButtonClicked();
}


/** Thumbnail has been changed
*/
void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &thumbnail)
		thumbnailChanged();
}


/** Called periodically on timer ticks period
*/
void MainComponent::timerCallback()
{
	//draw freq spectrum on timer tick (if the frame is ready)
	if (fftprocessor.nextFFTBlockReady)
	{
		fftprocessor.drawNextFrameOfSpectrum(spectralResolutionCoef);
		fftprocessor.nextFFTBlockReady = false;
	}

	//check for stop button state on timer tick
	if (true == stopFlag.get())
	{
		changeState(TransportState::Stopped);
		stopTimer();	
	}
	repaint();
}


/** Changing the state of the audio player.
* Each state sets the state flags for playback control.
* Paused state is the same as stopped, just without resetting the playback position.
* @param newState - new state for processing and deciding what to do
*/
void MainComponent::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;
		switch (state)
		{
		case TransportState::Playing:
			playButton.setButtonText("Pause");
			stopButton.setEnabled(true);
			//playButton.setEnabled(false);
			stopFlag.set(false);
			pauseFlag.set(false);
			startTimer(10);
			transportSource.start();
			break;

		case TransportState::Stopped:
			position = 0;
			transportSource.stop();
			transportSource.setPosition(0.0);
			stopButton.setEnabled(false);
			playButton.setButtonText("Play");
			playButton.setEnabled(true);
			break;

		case TransportState::Paused:
			stopButton.setEnabled(true);
			playButton.setButtonText("Play");
			playButton.setEnabled(true);
			break;
		}
	}
}


/** Paints the waveform window when the audio buffer is changed
*/
void MainComponent::thumbnailChanged(void)
{
	repaint();
}


/** Painting the waveform window if no file is loaded
* @param g - graphics context for painting of graphical components
* @param thumbnailBounds - bounds of the rectangle for waveform window
*/
void MainComponent::paintIfNoFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds)
{
	g.setColour(Colour((uint8)32, (uint8)16, (uint8)64, (uint8)255));
	g.fillRect(thumbnailBounds);
	g.setColour(Colours::white);
	g.drawFittedText("Open WAV / MP3 / FLAC file. (Or use drag and drop). \nMax 60 seconds... \nEnable tooltips for more info", thumbnailBounds, Justification::centred, 1);
}

/** Painting the waveform window of a loaded file and playing position marker.
* @param g - graphics context for painting of graphical components
* @param thumbnailBounds - bounds of the rectangle for waveform window
*/
void MainComponent::paintIfFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds)
{
	g.setColour(Colour((uint8)32, (uint8)16, (uint8)64, (uint8)255));
	g.fillRect(thumbnailBounds);
	g.setColour(Colours::white);
	auto audioLength = (float)thumbnail.getTotalLength();
	thumbnail.drawChannels(g, thumbnailBounds, 0.0 + zoomFactor + zoomPositionSeconds, audioLength - zoomFactor + zoomPositionSeconds, 1.0); //with performed zoom

	g.setColour(Colour((uint8)240, (uint8)236, (uint8)60, (uint8)255));
	auto audioPosition = (float)position/(float)adsetup.sampleRate;
	auto drawPosition = (audioPosition / audioLength) * (float)thumbnailBounds.getWidth() + (float)thumbnailBounds.getX();                                
	g.drawLine(drawPosition, (float)thumbnailBounds.getY(), drawPosition, (float)thumbnailBounds.getBottom(), 2.0f); //playing position marker
}


/** Processes keyboard key press event. Used for triggering of clicks on certain buttons.
* @param event - holds info about key event (which key was pressed)
*/
bool MainComponent::keyPressed(const KeyPress& press)
{
	//ModifierKeys modkeys = ModifierKeys::ModifierKeys();
	
	if (press == 'q' || press == 'Q')
	{
		playButton.triggerClick();
		return true;
	}

	if (press == 'w' || press == 'W')
	{
		stopButton.triggerClick();
		return true;
	}

	if (press == 'o' || press == 'O')
	{
		openButton.triggerClick();
		return true;
	}

	if (press == 's' || press == 'S')
	{
		saveToFileButton.triggerClick();
		return true;
	}

	return false;
}


/** Processes mouse button push event. Used for calculating a position for freq indicator and for the freq value at the click's position.
* @param event - holds info about mouse event (position,...)
*/
void MainComponent::mouseDown(const MouseEvent& event)
{
	int xcord = event.getMouseDownX() - 100;
	int ycord = event.getMouseDownY();

	//set x coordinate for drawing frequency marker line and repaint component
	mouseClickXCord = (float)xcord;
	mouseClickYCord = (float)ycord;

	//set position of audio playback on mouse click
	setPlaybackPosition();

	//set position of frequency indicator on mouse click
	setFreqIndicatorPosition();

	//update graphical content
	repaint();

	//compute frequency only in spectral window
	if (xcord > 0 && xcord < 930 && ycord < 410 && ycord > 240)
	{
		fftFreqDisplayText.setText(juce::String::String((int)cordValueToFreq(xcord)) + juce::String::String(" Hz"), juce::NotificationType::dontSendNotification);
	}

	MouseListener::mouseDown(event);
}


/** Processes mouse wheel movement (in both directions).
* @param event - holds info about mouse event (position,...)
* @param wheel - holds info about mouse wheel (amount of scrolling,...)
*/
void MainComponent::mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel)
{
	int xcord = event.getMouseDownX();
	int ycord = event.getMouseDownY();

	//ratio of mouse x coordinate to waveform display length
	float zoomPositionCoef = ((float)xcord - 100.0f) / 1090.0f;

	//setting the delta of zoom (to be relative to the buffer length)
	float zoomFactorDelta = fileBuffer.getNumSamples() / 2000000.0;

	//calculate only for x coords in waveform window
	if (xcord < 1190 && xcord > 100 && ycord < 160 && ycord > 10)
	{
		//zoom in on wheel up
		if (wheel.deltaY > 0.0f)
		{
			zoomFactor += zoomFactorDelta;

			if (zoomPositionCoef < 0.500f)
			{
				zoomPositionSeconds = zoomPositionSeconds - zoomFactorDelta * (1 + (((0.0f - 1.0f) / (0.5f - 0.0f)) * (zoomPositionCoef - 0.0f)));
			}
			else if (zoomPositionCoef > 0.500f)
			{
				zoomPositionSeconds = zoomPositionSeconds + zoomFactorDelta * (zoomPositionCoef * 2.0f - 1.0f);
			}
		}
		else if (wheel.deltaY < 0.0f) //zoom out on wheel down
		{
			zoomFactor -= zoomFactorDelta;
			if (zoomFactor < 0.0f)
			{
				zoomFactor = 0.0f; //dont zoom out more than original zoom level
			}

			//zoom in or out on specific area around mouse cursor
			if (zoomPositionCoef < 0.500f)
			{
				zoomPositionSeconds = zoomPositionSeconds + zoomFactorDelta * (1 + (((0.0f - 1.0f) / (0.5f - 0.0f)) * (zoomPositionCoef - 0.0f)));
			}
			else if (zoomPositionCoef > 0.500f)
			{
				zoomPositionSeconds = zoomPositionSeconds - zoomFactorDelta * (zoomPositionCoef * 2.0f - 1.0f);
			}

			//set the waveform into original aligned position after zooming out
			if (zoomFactor == 0.0f)
			{
				zoomPositionSeconds = 0.000f;
			}
		}
		
		repaint();
	}
}


/** Translates an atomic bool value to double value (1.0 if true / 0.0 if false)
* @param atomicValue - bool value in atomic format
*/
double MainComponent::atomicToDouble(Atomic<bool> atomicValue)
{
	return atomicValue.get() ? 1.0 : 0.0;
}

/** Inversely translates a double value to atomic bool value (0.0 if true / 1.0 if false)
* @param doubleValue - double value to translate
*/
bool MainComponent::doubleToBool(double doubleValue)
{
	
	return doubleValue == 1.0 ? false : true;
}


/** Helper function for opening Windows explorer window and opening the XML file
*/
void MainComponent::openXmlFile()
{
	chooser = std::make_unique<juce::FileChooser>("Select a preset to open...",
		juce::File{},
		"*.xml");

	auto chooserFlags = juce::FileBrowserComponent::openMode
		| juce::FileBrowserComponent::canSelectFiles;

	chooser->launchAsync(chooserFlags, [&](const FileChooser& fc)
	{
		auto file = chooser->getResult();
		if (file.getSize() != 0)
		{
			parseXmlElements(file);
		}
	});

}

/** Helper function for opening Windows explorer window and saving the XML file
*/
void MainComponent::saveXmlFile()
{
	chooser = std::make_unique<juce::FileChooser>("Select a file to save to...",
		juce::File{},
		"*.xml");

	auto chooserFlags = juce::FileBrowserComponent::saveMode
		| juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting;

	chooser->launchAsync(chooserFlags, [&](const FileChooser& fc)
	{
		auto file = chooser->getResult();

		saveXmlElements(file);
	});

}


/** Saves the state of all effect parameters into an XML file with UTF-8 encoding and prescribed structure
* @param file - xml formatted output file
*/
void MainComponent::saveXmlElements(File file)
{
	std::unique_ptr<XmlElement> gainElement = std::make_unique<XmlElement>("Gain");
	std::unique_ptr<XmlElement> pitchElement = std::make_unique<XmlElement>("Pitch");
	std::unique_ptr<XmlElement> lpfElement = std::make_unique<XmlElement>("LPF");
	std::unique_ptr<XmlElement> hpfElement = std::make_unique<XmlElement>("HPF");
	std::unique_ptr<XmlElement> bpfElement = std::make_unique<XmlElement>("BPF");
	std::unique_ptr<XmlElement> reverbElement = std::make_unique<XmlElement>("Reverb");
	std::unique_ptr<XmlElement> scdElement = std::make_unique<XmlElement>("SCD");
	std::unique_ptr<XmlElement> hcdElement = std::make_unique<XmlElement>("HCD");
	std::unique_ptr<XmlElement> frdElement = std::make_unique<XmlElement>("FRD");
	std::unique_ptr<XmlElement> hrdElement = std::make_unique<XmlElement>("HRD");
	std::unique_ptr<XmlElement> extractorElement = std::make_unique<XmlElement>("Extractor");
	std::unique_ptr<XmlElement> reverzElement = std::make_unique<XmlElement>("Reverz");
	std::unique_ptr<XmlElement> stutterElement = std::make_unique<XmlElement>("Stutter");
	std::unique_ptr<XmlElement> shifterElement = std::make_unique<XmlElement>("Shifter");

	std::unique_ptr<XmlElement> mainElement = std::make_unique<XmlElement>("Parameters");

	gainElement->setTagName("Gain");
	gainElement->setAttribute("Enabled", atomicToDouble(gainEnabled));
	gainElement->setAttribute("Value", gainSlider.getValue());

	pitchElement->setTagName("Pitch");
	pitchElement->setAttribute("Enabled", atomicToDouble(pitchEnabled));
	pitchElement->setAttribute("Value", pitchSlider.getValue());

	lpfElement->setTagName("LPF");
	lpfElement->setAttribute("Enabled", atomicToDouble(lpfEnabled));
	lpfElement->setAttribute("Cutoff", LowpassFreqSlider.getValue());
	lpfElement->setAttribute("Q", LowpassQualitySlider.getValue());

	hpfElement->setTagName("HPF");
	hpfElement->setAttribute("Enabled", atomicToDouble(hpfEnabled));
	hpfElement->setAttribute("Cutoff", HighpassFreqSlider.getValue());
	hpfElement->setAttribute("Q", HighpassQualitySlider.getValue());

	bpfElement->setTagName("BPF");
	bpfElement->setAttribute("Enabled", atomicToDouble(bpfEnabled));
	bpfElement->setAttribute("Cutoff", BandpassFreqSlider.getValue());
	bpfElement->setAttribute("Q", BandpassQualitySlider.getValue());

	reverbElement->setTagName("Reverb");
	reverbElement->setAttribute("Enabled", atomicToDouble(reverbEnabled));
	reverbElement->setAttribute("Balance", reverbBalanceSlider.getValue());
	reverbElement->setAttribute("Size", reverbSizeSlider.getValue());
	reverbElement->setAttribute("Width", reverbWidthSlider.getValue());
	reverbElement->setAttribute("Damp", reverbDampeningSlider.getValue());

	scdElement->setTagName("SCD");
	scdElement->setAttribute("Enabled", atomicToDouble(scdEnabled));
	scdElement->setAttribute("Threshold", scdThresholdSlider.getValue());

	hcdElement->setTagName("HCD");
	hcdElement->setAttribute("Enabled", atomicToDouble(hcdEnabled));
	hcdElement->setAttribute("Threshold", hcdThresholdSlider.getValue());

	frdElement->setTagName("FRD");
	frdElement->setAttribute("Enabled", atomicToDouble(frdEnabled));

	hrdElement->setTagName("HRD");
	hrdElement->setAttribute("Enabled", atomicToDouble(hrdEnabled));

	extractorElement->setTagName("Extractor");
	extractorElement->setAttribute("Enabled", atomicToDouble(extractorEnabled));
	extractorElement->setAttribute("Intensity", extractorIntensitySlider.getValue());
	extractorElement->setAttribute("Width", extractorWidthSlider.getValue());

	reverzElement->setTagName("Reverz");
	reverzElement->setAttribute("Enabled", atomicToDouble(reverzEnabled));
	reverzElement->setAttribute("Skew", reverzSkewSlider.getValue());
	reverzElement->setAttribute("Amount", reverzAmountSlider.getValue());

	stutterElement->setTagName("Stutter");
	stutterElement->setAttribute("Enabled", atomicToDouble(stutterEnabled));
	stutterElement->setAttribute("Amount", stutterAmountSlider.getValue());
	stutterElement->setAttribute("Chorus", stutterChorusSlider.getValue());
	stutterElement->setAttribute("Delay", stutterDelaySlider.getValue());

	shifterElement->setTagName("Shifter");
	shifterElement->setAttribute("Enabled", atomicToDouble(shifterEnabled));
	shifterElement->setAttribute("Amount", shifterAmountSlider.getValue());
	shifterElement->setAttribute("Tone", shifterToneSlider.getValue());

	mainElement->setTagName("Parameters");
	mainElement->addChildElement(gainElement.release());
	mainElement->addChildElement(pitchElement.release());
	mainElement->addChildElement(lpfElement.release());
	mainElement->addChildElement(hpfElement.release());
	mainElement->addChildElement(bpfElement.release());
	mainElement->addChildElement(reverbElement.release());
	mainElement->addChildElement(scdElement.release());
	mainElement->addChildElement(hcdElement.release());
	mainElement->addChildElement(frdElement.release());
	mainElement->addChildElement(hrdElement.release());
	mainElement->addChildElement(extractorElement.release());
	mainElement->addChildElement(reverzElement.release());
	mainElement->addChildElement(stutterElement.release());
	mainElement->addChildElement(shifterElement.release());

	mainElement->writeTo(file);


}

/** Reads and parses an XML preset file
* Parsed file needs to have correct parameter structure and XML format with UTF-8 encoding (presets created by the program have a correct structure)
* @param file - xml formatted input file
*/
void MainComponent::parseXmlElements(File file)
{
	std::unique_ptr<XmlElement> mainElement = std::make_unique<XmlElement>("Parameters");
	//XmlDocument* mainDocument = new XmlDocument(file);
	mainElement = (XmlDocument::parse(file));

	gainEnabled.set(doubleToBool(((mainElement->getChildByName("Gain"))->getAttributeValue(0)).getDoubleValue()));
	gainSlider.setValue(((mainElement->getChildByName("Gain"))->getAttributeValue(1)).getDoubleValue());
	processGainButtonClicked();

	pitchEnabled.set(doubleToBool(((mainElement->getChildByName("Pitch"))->getAttributeValue(0)).getDoubleValue()));
	pitchSlider.setValue(((mainElement->getChildByName("Pitch"))->getAttributeValue(1)).getDoubleValue());
	processPitchButtonClicked();

	lpfEnabled.set(doubleToBool(((mainElement->getChildByName("LPF"))->getAttributeValue(0)).getDoubleValue()));
	LowpassFreqSlider.setValue(((mainElement->getChildByName("LPF"))->getAttributeValue(1)).getDoubleValue());
	LowpassQualitySlider.setValue(((mainElement->getChildByName("LPF"))->getAttributeValue(2)).getDoubleValue());
	processLowpassFilterButtonClicked();

	hpfEnabled.set(doubleToBool(((mainElement->getChildByName("HPF"))->getAttributeValue(0)).getDoubleValue()));
	HighpassFreqSlider.setValue(((mainElement->getChildByName("HPF"))->getAttributeValue(1)).getDoubleValue());
	HighpassQualitySlider.setValue(((mainElement->getChildByName("HPF"))->getAttributeValue(2)).getDoubleValue());
	processHighpassFilterButtonClicked();

	bpfEnabled.set(doubleToBool(((mainElement->getChildByName("BPF"))->getAttributeValue(0)).getDoubleValue()));
	BandpassFreqSlider.setValue(((mainElement->getChildByName("BPF"))->getAttributeValue(1)).getDoubleValue());
	BandpassQualitySlider.setValue(((mainElement->getChildByName("BPF"))->getAttributeValue(2)).getDoubleValue());
	processBandpassFilterButtonClicked();

	reverbEnabled.set(doubleToBool(((mainElement->getChildByName("Reverb"))->getAttributeValue(0)).getDoubleValue()));
	reverbBalanceSlider.setValue(((mainElement->getChildByName("Reverb"))->getAttributeValue(1)).getDoubleValue());
	reverbSizeSlider.setValue(((mainElement->getChildByName("Reverb"))->getAttributeValue(2)).getDoubleValue());
	reverbWidthSlider.setValue(((mainElement->getChildByName("Reverb"))->getAttributeValue(3)).getDoubleValue());
	reverbDampeningSlider.setValue(((mainElement->getChildByName("Reverb"))->getAttributeValue(4)).getDoubleValue());
	processReverbButtonClicked();

	scdEnabled.set(doubleToBool(((mainElement->getChildByName("SCD"))->getAttributeValue(0)).getDoubleValue()));
	scdThresholdSlider.setValue(((mainElement->getChildByName("SCD"))->getAttributeValue(1)).getDoubleValue());
	processHardclipDistortionButtonClicked();

	hcdEnabled.set(doubleToBool(((mainElement->getChildByName("HCD"))->getAttributeValue(0)).getDoubleValue()));
	hcdThresholdSlider.setValue(((mainElement->getChildByName("HCD"))->getAttributeValue(1)).getDoubleValue());
	processHardclipDistortionButtonClicked();

	frdEnabled.set(doubleToBool(((mainElement->getChildByName("FRD"))->getAttributeValue(0)).getDoubleValue()));
	processFullrectDistortionButtonClicked();

	hrdEnabled.set(doubleToBool(((mainElement->getChildByName("HRD"))->getAttributeValue(0)).getDoubleValue()));
	processHalfrectDistortionButtonClicked();

	extractorEnabled.set(doubleToBool(((mainElement->getChildByName("Extractor"))->getAttributeValue(0)).getDoubleValue()));
	extractorIntensitySlider.setValue(((mainElement->getChildByName("Extractor"))->getAttributeValue(1)).getDoubleValue());
	extractorWidthSlider.setValue(((mainElement->getChildByName("Extractor"))->getAttributeValue(2)).getDoubleValue());
	processExtractorButtonClicked();

	reverzEnabled.set(doubleToBool(((mainElement->getChildByName("Reverz"))->getAttributeValue(0)).getDoubleValue()));
	reverzSkewSlider.setValue(((mainElement->getChildByName("Reverz"))->getAttributeValue(1)).getDoubleValue());
	reverzAmountSlider.setValue(((mainElement->getChildByName("Reverz"))->getAttributeValue(2)).getDoubleValue());
	processReverzButtonClicked();

	stutterEnabled.set(doubleToBool(((mainElement->getChildByName("Stutter"))->getAttributeValue(0)).getDoubleValue()));
	stutterAmountSlider.setValue(((mainElement->getChildByName("Stutter"))->getAttributeValue(1)).getDoubleValue());
	stutterChorusSlider.setValue(((mainElement->getChildByName("Stutter"))->getAttributeValue(2)).getDoubleValue());
	stutterDelaySlider.setValue(((mainElement->getChildByName("Stutter"))->getAttributeValue(3)).getDoubleValue());
	processStutterButtonClicked();

	shifterEnabled.set(doubleToBool(((mainElement->getChildByName("Shifter"))->getAttributeValue(0)).getDoubleValue()));
	shifterAmountSlider.setValue(((mainElement->getChildByName("Shifter"))->getAttributeValue(1)).getDoubleValue());
	shifterToneSlider.setValue(((mainElement->getChildByName("Shifter"))->getAttributeValue(2)).getDoubleValue());
	processShifterButtonClicked();
}


/** Processes clicking on the openPreset button. Called when this button is clicked.
*/
void MainComponent::openPresetButtonClicked(void)
{
	openXmlFile();
}

/** Processes clicking on the savePreset button. Called when this button is clicked.
*/
void MainComponent::savePresetButtonClicked(void)
{
	saveXmlFile();
}


/** Enables / disables showing of tooltips
* Tooltips contain brief info about the component under mouse cursor (not all components have own tooltip)
*/
void MainComponent::toggleTooltipButtonClicked()
{
	if (tooltipEnabled.get() == false)
	{
		tooltipEnabled.set(true);
		toggleTooltipButton.setColour(TextButton::buttonColourId, Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
		resetEffectButton.setTooltip("Resets every effect to its default state");
		fileOpenSaveLabel.setTooltip("Opens / saves a file");
		presetOpenSaveLabel.setTooltip("Opens / saves a preset file in XML format");
		bitDepthLabel.setTooltip("Shows current bit depth of the file (any changes will be applied upon saving the file)");
		clippingLabel.setTooltip("Shows if clipping occurs in the final output");
		toggleTooltipButton.setTooltip("Toggles showing of tooltips"); 
		processGainButton.setTooltip("Sets gain (-40dB, 40dB)");
		spectralResolutionLabel.setTooltip("Changes the distribution of the frequencies in the freq spectrum window");
		fftFreqDisplayLabel.setTooltip("Shows specific frequency in the spectrum window (click in the spectrum)");
		processPitchButton.setTooltip("Sets pitch (by multiplying)");
		processShifterButton.setTooltip("Toggles Shifter effect \nAmount - how many times is sample divided \nTone - pitch of the second part of every section");
		processStutterButton.setTooltip("Toggles Stutter effect \nAmount - how many times is sample divided \nDelay - delay time of the chorus \nChorus - amount of the chorus effect");
		processReverzButton.setTooltip("Toggles Reverz effect \nAmount - how many times is sample divided \nSkew - size of the reversed parts");
		processExtractorButton.setTooltip("Toggles Extractor effect \nIntensity - how many sections are deleted from the sample \nWidth - size of deleted sections");
		processSoftclipDistortionButton.setTooltip("Toggles Softclip effect \nThreshold - amount of the distortion");
		processHardclipDistortionButton.setTooltip("Toggles Hardclip effect \nThreshold - amount of the distortion");
		processFullrectDistortionButton.setTooltip("Toggles Fullrect effect (flips negative values)");
		processHalfrectDistortionButton.setTooltip("Toggles Halfrect effect (deletes negative values)");
		processReverbButton.setTooltip("Toggles Reverb effect");
		processLowpassFilterButton.setTooltip("Toggles lowpass filter");
		processHighpassFilterButton.setTooltip("Toggles highpass filter");
		processBandpassFilterButton.setTooltip("Toggles bandpass filter");
		playbackShiftLength.setTooltip("Enter value by which will be the play position shifted (in samples)");
		forwardPlaybackButton.setTooltip("Forwards the playback position by set value. \nYou can hold the button");
		backwardPlaybackButton.setTooltip("Backwards the playback position by set value. \nYou can hold the button");
		playButton.setTooltip("Plays a file \nYou can change playing position by clicking in the waveform window \nYou can zoom in/out with mouse wheel");
		loopButton.setTooltip("Toggles playback looping mode");

	}
	else if (tooltipEnabled.get() == true)
	{
		tooltipEnabled.set(false);
		toggleTooltipButton.setColour(TextButton::buttonColourId, Colours::darkgrey);
		resetEffectButton.setTooltip("");
		fileOpenSaveLabel.setTooltip("");
		presetOpenSaveLabel.setTooltip("");
		bitDepthLabel.setTooltip("");
		clippingLabel.setTooltip("");
		toggleTooltipButton.setTooltip("");
		processGainButton.setTooltip("");
		spectralResolutionLabel.setTooltip("");
		fftFreqDisplayLabel.setTooltip("");
		processPitchButton.setTooltip("");
		processShifterButton.setTooltip("");
		processStutterButton.setTooltip("");
		processReverzButton.setTooltip("");
		processExtractorButton.setTooltip("");
		processSoftclipDistortionButton.setTooltip("");
		processHardclipDistortionButton.setTooltip("");
		processFullrectDistortionButton.setTooltip("");
		processHalfrectDistortionButton.setTooltip("");
		processReverbButton.setTooltip("");
		playbackShiftLength.setTooltip("");
		forwardPlaybackButton.setTooltip("");
		backwardPlaybackButton.setTooltip("");
		playButton.setTooltip("");
		loopButton.setTooltip("");
		processLowpassFilterButton.setTooltip("");
		processHighpassFilterButton.setTooltip("");
		processBandpassFilterButton.setTooltip("");
	}
}