/*

    CustomLookAndFeel.cpp

*/
#include "../JuceLibraryCode/JuceHeader.h"

class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        LookAndFeel_V4::setColour(juce::Slider::thumbColourId, Colour((uint8)150, (uint8)150, (uint8)255, (uint8)255));
        LookAndFeel_V4::setColour(juce::Slider::trackColourId, Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
        LookAndFeel_V4::setColour(Slider::textBoxOutlineColourId, Colours::black);
        LookAndFeel_V4::setColour(Slider::textBoxHighlightColourId, Colours::black);
        LookAndFeel_V4::setColour(TextButton::textColourOffId, Colours::white);
        LookAndFeel_V4::setColour(Slider::textBoxOutlineColourId, Colour((uint8)70, (uint8)60, (uint8)100, (uint8)255));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(Colour((uint8)100, (uint8)100, (uint8)150, (uint8)255));
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::black);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);

        juce::Path pp;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        pp.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        pp.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(Colour((uint8)150, (uint8)150, (uint8)255, (uint8)255));
        g.fillPath(pp);

        LookAndFeel_V4::setColour(Slider::textBoxOutlineColourId, Colours::black);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
        bool, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        auto edge = 2;

        buttonArea.removeFromTop(edge);

        // shadow
        g.setColour(juce::Colours::black);
        g.fillRect(buttonArea);

        auto offset = isButtonDown ? -edge / 2 : -edge;
        buttonArea.translate(0, offset);

        g.setColour(backgroundColour);
        g.fillRoundedRectangle(buttonArea.toFloat(), 4);
        g.drawRoundedRectangle(buttonArea.toFloat(), 4, 1);
        //g.fillRect(buttonArea);
    }

    void CustomLookAndFeel::drawLabel(Graphics& g, Label& label)
    {
        //g.fillAll(label.findColour(Label::backgroundColourId));
        g.setColour(label.findColour(Label::backgroundColourId));
        g.fillRoundedRectangle(label.getLocalBounds().toFloat(), 5);

        if (!label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font(LookAndFeel_V2::getLabelFont(label));

            g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
            g.setFont(font);

            auto textArea = LookAndFeel_V2::getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
            //textArea.setWidth(textArea.getWidth() + 4);

            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
                label.getMinimumHorizontalScale());

            g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
        }
        else if (label.isEnabled())
        {
            g.setColour(label.findColour(Label::outlineColourId));
        }

        //g.drawRect(label.getLocalBounds());
        g.drawRoundedRectangle(label.getLocalBounds().toFloat(), 5, 1);
    }

    Font CustomLookAndFeel::getTextButtonFont(TextButton&, int buttonHeight)
    {
        
        //return { jmin(15.0f, (float)buttonHeight * 0.6f) };
        return Font::Font("Montserrat", jmin(15.0f, (float)buttonHeight * 0.6f), Font::plain);
    }

    void CustomLookAndFeel::drawButtonText(Graphics& g, TextButton& button,
        bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        Font font(getTextButtonFont(button, button.getHeight()));
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
            : TextButton::textColourOffId)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
        const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = roundToInt(font.getHeight() * 0.6f);
        const int leftIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent + 6;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                leftIndent - 3, yIndent, textWidth, button.getHeight() - yIndent * 2,
                Justification::centred, 2);
    }

};

