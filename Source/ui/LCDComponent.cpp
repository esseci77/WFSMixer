/*
  ==============================================================================

    LCDComponent.cpp
    Created: 15 Feb 2021 11:11:28pm
    Author:  Simone Campanini

  ==============================================================================
*/
#include <JuceHeader.h>
#include "../WfsLookAndFeel.h"
#include "LCDComponent.h"


LCDComponent::LCDComponent(const int numberOfLines,
                           const juce::String& name)
: juce::Component(name)
{
    for(int i = 0; i < numberOfLines; ++i)
    {
        m_linesOfText.add("*");
        m_rightAlignedLinesOfText.add("");
    }
}


void LCDComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    g.setColour(juce::Colour(0xff94a49e));
    
    auto flArea = area.toFloat();
    g.fillRoundedRectangle(flArea, 5.0f);
    
    const int nLines = m_linesOfText.size();
    
    area.reduce(10, 10);
    const int lineHeight = (area.getHeight() - 5 * (nLines - 1)) / nLines;
    
    auto laf = WfsLookAndFeel::getInstance();
    //g.setFont(laf->getLedDisplayFont().withHeight((float)lineHeight*2.0));
    g.setFont(laf->getRegularFont().withHeight((float)lineHeight * 2.0));
    g.setColour(juce::Colour(0xff232725));
    
    for (int i = 0; i < nLines; ++i)
    {
        if (m_rightAlignedLinesOfText[i].isEmpty())
        {
            g.drawText(m_linesOfText[i],
                       area.removeFromTop(lineHeight),
                       juce::Justification::centredLeft);
        }
        else
        {
            auto line = area.removeFromTop(lineHeight);

            g.drawText(m_linesOfText[i],
                       line.removeFromLeft(line.getWidth() / 2),
                       juce::Justification::centredLeft);

            g.drawText(m_rightAlignedLinesOfText[i],
                       line,
                       juce::Justification::centredRight);
        }
        area.removeFromTop(5);
    }
    
}

void LCDComponent::setText(const int line,
                           const juce::String& textToDisplay,
                           const bool isRightAligned)
{
    if (isRightAligned)
    {
        m_rightAlignedLinesOfText.set(line, textToDisplay);
    }
    else
    {
        m_linesOfText.set(line, textToDisplay);
    }
    repaint();
}
