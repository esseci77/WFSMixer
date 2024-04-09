/*
  ==============================================================================

    WfsLookAndFeel.cpp
    Created: 15 Feb 2021 10:36:32am
    Author:  Simone Campanini

  ==============================================================================
*/

#include "WfsLookAndFeel.h"

juce_ImplementSingleton(WfsLookAndFeel)

WfsLookAndFeel::WfsLookAndFeel()
 : m_comfortaaBold(Font(Typeface::createSystemTypefaceFor(BinaryData::ComfortaaBold_ttf,
                                                          BinaryData::ComfortaaBold_ttfSize))),
   m_comfortaaLight(Font(Typeface::createSystemTypefaceFor(BinaryData::ComfortaaLight_ttf,
                                                           BinaryData::ComfortaaLight_ttfSize))),
   m_comfortaaRegular(Font(Typeface::createSystemTypefaceFor(BinaryData::ComfortaaRegular_ttf,
                                                             BinaryData::ComfortaaRegular_ttfSize))),
   m_neuropolitical(Font(Typeface::createSystemTypefaceFor(BinaryData::neuropolitical_rg_ttf,
                                                           BinaryData::neuropolitical_rg_ttfSize))),
   m_ledcalculator(Font(Typeface::createSystemTypefaceFor(BinaryData::LEDCalculator_ttf,
                                                        BinaryData::LEDCalculator_ttfSize)))
{
    
}

const juce::Font& WfsLookAndFeel::getRegularFont()
{
    return m_comfortaaRegular;
}

const juce::Font& WfsLookAndFeel::getBoldFont()
{
    return m_comfortaaBold;
}

const juce::Font& WfsLookAndFeel::getLightFont()
{
    return m_comfortaaLight;
}

const juce::Font& WfsLookAndFeel::getMainTitleFont()
{
    return m_neuropolitical;
}

const juce::Font& WfsLookAndFeel::getLedDisplayFont()
{
    return m_ledcalculator;
}

juce::Font WfsLookAndFeel::getLabelFont(Label& label)
{
    if (label.getName() == "title")
    {
        // return getMainTitleFont().withHeight(kTitleFontSize);
        return getRegularFont().withHeight(kTitleFontSize);
    }
    else if (label.getName() == "versionLabel")
    {
        return getLightFont().withHeight(20.0f);
    }

    return getLightFont().withHeight(24.0f);
}

juce::Font WfsLookAndFeel::getTextButtonFont(juce::TextButton& /*button*/,
                                             int /*buttonHeight*/)
{
    return getLightFont().withHeight(20.0f);
}

juce::Font WfsLookAndFeel::getSliderPopupFont(juce::Slider& /*slider*/)
{
    return getLightFont();
}
