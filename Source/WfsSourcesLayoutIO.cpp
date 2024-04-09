/*
  ==============================================================================

    WfsSourcesLayoutIO.cpp
    Created: 21 Jan 2022 11:24:08am
    Author:  Simone Campanini

  ==============================================================================
*/

#include "WfsSourcesLayoutIO.h"


WfsSourcesLayoutIO::WfsSourcesLayoutIO(WFSRendererAudioProcessor* processor)
 : m_processor(processor)
{
    
}

bool WfsSourcesLayoutIO::load(const juce::File& /*f*/)
{
    auto& p = m_processor->params();
    
    
    return true;
}

bool WfsSourcesLayoutIO::save(const juce::File& /*f*/)
{
    return true;
}
