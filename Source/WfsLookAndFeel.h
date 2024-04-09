/*
  ==============================================================================

    WfsLookAndFeel.h
    Created: 15 Feb 2021 10:36:32am
    Author:  Simone Campanini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#if RENDERER_ID == 1
#  define kTitleFontSize  48.0f
#  define kWindowWidth  1400
#  define kWindowHeight  750

#else
#  define kTitleFontSize  40.0f
#  define kWindowWidth   900
#  define kWindowHeight  132

#endif


class WfsLookAndFeel : public juce::LookAndFeel_V4,
                       public juce::DeletedAtShutdown
{
    const juce::Font m_comfortaaBold;
    const juce::Font m_comfortaaLight;
    const juce::Font m_comfortaaRegular;
    const juce::Font m_neuropolitical;
    const juce::Font m_ledcalculator;
    
    
public:
    WfsLookAndFeel();
    
    const juce::Font& getRegularFont();
    const juce::Font& getBoldFont();
    const juce::Font& getLightFont();
    const juce::Font& getMainTitleFont();
    const juce::Font& getLedDisplayFont();
    
    Font getLabelFont(Label& label) override;
    Font getTextButtonFont(TextButton& button, int buttonHeight) override;
    Font getSliderPopupFont(Slider& slider) override;
    
    juce_DeclareSingleton(WfsLookAndFeel, false);
};
