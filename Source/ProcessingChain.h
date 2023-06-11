/*
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>




class ProcessingChain : public TopLevelWindow, public ListBoxModel, public ListBox
{
public:
    //==============================================================================
    ProcessingChain ();
    ~ProcessingChain() override;

    void okButtonClicked(void);

    /*void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool isRowSelected) override;
    int getNumRows() override;*/

    void paint (juce::Graphics& g) override;
    void resized() override;

    enum ChainOrder
    {
        hcdistortion,
        frdistortion,
        hrdistortion,
        extractor,
        shifter,
        reverb,
        filter,
        gain
    };


private:
    TextButton okButton;


    //ListBox chainListBox;
    //ListBoxModel *chainListBoxModel;

    //void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool isRowSelected) override;
    //int getNumRows() override;

    //ListBoxSource lss;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessingChain)
};


