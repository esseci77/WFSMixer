/*
  ==============================================================================

    WfsSourcesLayoutIO.h
    Created: 21 Jan 2022 11:24:08am
    Author:  Simone Campanini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class WfsSourcesLayoutIO
{
public:
    WfsSourcesLayoutIO(WFSRendererAudioProcessor* processor);
    
    bool load(const juce::File& f);
    bool save(const juce::File& f);
    
private:
    WFSRendererAudioProcessor* m_processor = nullptr;
};
