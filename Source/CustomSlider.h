/*

	CustomSlider.cpp

*/
#include "../JuceLibraryCode/JuceHeader.h"

class CustomSlider : public Slider
{
public:
	CustomSlider() 
	{
		
	}

	int mouseposx;
	int mouseposy;

	void mouseDown(const MouseEvent& event) override
	{
		mouseposx = event.getMouseDownX();
		mouseposy = event.getMouseDownY();
		event.source.enableUnboundedMouseMovement(true);
		juce::Slider::mouseDown(event);

		
	}

	void mouseUp(const MouseEvent& event) override
	{
		Desktop::getInstance().setMousePosition(juce::Point<int>(mouseposx, mouseposy));
		event.source.enableUnboundedMouseMovement(false);
		juce::Slider::mouseUp(event);
	}

	/*void mouseDrag(const MouseEvent& event) override
	{

	}*/

	
};