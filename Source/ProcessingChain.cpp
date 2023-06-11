/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.1.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "ProcessingChain.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ProcessingChain::ProcessingChain () : juce::TopLevelWindow("Processing Chain", true)
{
    
	okButton.setButtonText("OK");
	okButton.setColour(TextButton::buttonColourId, Colour((uint8)32, (uint8)0, (uint8)64, (uint8)255));
	okButton.setColour(TextButton::textColourOffId, Colours::white);
	okButton.setEnabled(true);
	okButton.onClick = [this] {  okButtonClicked(); };
	TopLevelWindow::addAndMakeVisible(&okButton);
	
	
	//ListBoxSource* lss = new ListBoxSource();

	/*lss.setBounds(50, 50, 500, 450);*/
	
	//ListBoxModel* chainListBoxModel;
	//ListBox chainListBox;
	//chainListBox.setModel(ListBoxModel("model"));
	//chainListBox.updateContent();
	/*chainListBox.setColour(ListBox::backgroundColourId, Colours::white);
	chainListBox.setColour(ListBox::textColourId, Colour((uint8)32, (uint8)16, (uint8)64, (uint8)255));
	chainListBox.setBounds(50, 50, 500, 450);
	TopLevelWindow::addAndMakeVisible(&chainListBox);*/

	TopLevelWindow::setSize (600, 400);

}

ProcessingChain::~ProcessingChain()
{

}

void ProcessingChain::okButtonClicked(void)
{
	delete this;
}



/*void ProcessingChain::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool isRowSelected)
{
	Rectangle<float> rect;
	rect.setBounds(0, 0, 80, 25);
	switch (row) 
	{
		case 0:
			g.drawText("hcdistortion", rect, Justification::centredLeft, true);
			break;
		case 1:
			g.drawText("hcdistortion", rect, Justification::centredLeft, true);
			break;
		case 2:
			g.drawText("hcdistortion", rect, Justification::centredLeft, true);
			break;
		case 3:
			g.drawText("hcdistortion", rect, Justification::centredLeft, true);
			break;
		case 4:
			g.drawText("hcdistortion", rect, Justification::centredLeft, true);
			break;
		case 5:
			g.drawText("hcdistortion", rect, Justification::centredLeft, true);
			break;

	}
}*/

//==============================================================================
void ProcessingChain::paint (juce::Graphics& g)
{
	
	g.fillAll(Colour((uint8)16, (uint8)16, (uint8)32, (uint8)255));


}

void ProcessingChain::resized()
{
	okButton.setBounds(Component::getWidth() - 70, Component::getHeight() - 70, 50, 50);
}



class ListBoxSource : public ListBox,
	public ListBoxModel
{

public:
	ListBoxSource()
		: ListBox("d+d source", 0)
	{
		// tells the ListBox that this object supplies the info about its rows.

		setModel(this);
		setMultipleSelectionEnabled(false);
	}

	//==============================================================================

	// The following methods implement the necessary virtual functions from ListBoxModel,
	// telling the listbox how many rows there are, painting them, etc.

	int getNumRows()
	{
		return 6;
	}

	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
	{
		if (rowIsSelected)
			g.fillAll(Colours::lightblue);

		g.setColour(Colours::black);
		g.setFont(height * 0.7f);

		g.drawText("Row Number " + String(rowNumber + 1), 5, 0, width, height,
			Justification::centredLeft, true);
	}

	//==============================================================================
	// this just fills in the background of the listbox

	void paint(Graphics& g)
	{
		g.fillAll(Colours::white.withAlpha(0.7f));
	}
};




/*void MainComponent::processingChainButtonClicked(void)
{
	if (processchainwindow)
	{
		processchainwindow->TopLevelWindow::toFront(true);
	}	
	else
	{
		//processchainwindow = new juce::TopLevelWindow::SafePointer<ProcessingChain>;
		processchainwindow->TopLevelWindow::setLookAndFeel(&customLookAndFeel);
		processchainwindow->TopLevelWindow::centreWithSize(600, 400);
		processchainwindow->TopLevelWindow::setVisible(true);
		
	}
}*/