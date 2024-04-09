/*
  ==============================================================================

    LCDComponent.h
    Created: 15 Feb 2021 11:11:28pm
    Author:  Simone Campanini

  ==============================================================================
*/

#pragma once

class LCDComponent : public juce::Component
{
  public:
    LCDComponent(const int numberOfLines,
                 const juce::String& name = "LCDComponent");
    
    void paint(juce::Graphics& g) override;
    
    void setText(const int line,
                 const juce::String& textToDisplay,
                 const bool isRightAligned = false);
    
private:
    juce::StringArray m_linesOfText;
    juce::StringArray m_rightAlignedLinesOfText;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LCDComponent);
};
