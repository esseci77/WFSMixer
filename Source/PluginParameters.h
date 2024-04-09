/*
  ==============================================================================

    PluginParameters.h
    Created: 8 Jan 2021 1:00:46pm
    Author:  Simone Campanini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#define minusInfinitydB -72.0

#define DEFAULT_ADDRESS     "224.4.0.251"
#define DEFAULT_PORT_STRING "7130"
#define DEFAULT_PORT         7130

struct WFSSourceParameters
{
    juce::AudioParameterFloat*   xpos  = nullptr; // x0 position
    juce::AudioParameterFloat*   ypos  = nullptr; // y0 position
    juce::AudioParameterFloat*   xspd  = nullptr; // x linear speed
    juce::AudioParameterFloat*   yspd  = nullptr; // y linear speed
    juce::AudioParameterFloat*   speed = nullptr; // maximum linear
                                                  // speed    
    juce::AudioParameterFloat*   aspd    = nullptr; // angular speed
    juce::AudioParameterFloat*   radius  = nullptr;
    
    juce::AudioParameterFloat*   gain    = nullptr;
    juce::AudioParameterBool*    mute    = nullptr;
    juce::AudioParameterBool*    solo    = nullptr;
    
    float x       = 0.0; // x position after summing all motion components
    float y       = 0.0; // y position after summing all motion components
    float angPos  = 0.0; // angular position
    float sx      = 0.0; // linear displacement
    float sy      = 0.0; // linear displacement
};

class WFSRendererParameters
{
    AudioProcessorValueTreeState m_valueTreeState;
    
    std::vector<WFSSourceParameters> m_srcParams;
    int   m_nSources    = 0;
    int   m_paramPeriod = 2048; // this can be modified only on startup!
    int   m_paramOffset = 0;
    float m_maxdist     = 2.0;  // m
    
public:
    int numberOfSources() const { return m_nSources; }
    WFSSourceParameters& operator[](const int idx) { return m_srcParams[idx]; }
    
    bool thereAreSolos() const;
    
    /**
     * @brief Update parameters
     * @param dt Time interval in seconds
     */
    void update(const double dt);
    void update(double sampleRate, int bufferLength); // TBR
    
    void setRendererId(const int rid);
    int rendererId();
    
    void setMaxDist(const float md) { m_maxdist = md; }
    float maxDist() const { return m_maxdist; }
    
    void setParamPeriod(const float pp) { m_paramPeriod = (int)pp; }
    int paramPeriod() const { return m_paramPeriod; }

    void setParamOffset(const float po) { m_paramOffset = (int)po; }
    int paramOffset() const { return m_paramOffset; }

    void setLayoutFilePath(const juce::File& lfp);
    void setLayoutFilePath(const juce::String& lfp);
    juce::String layoutFilePath();
    
    void setLastDirectory(const juce::File& ld);
    void setLastDirectory(const juce::String& ld);
    juce::String lastDirectory();
    
    void setOscAddress(const juce::String& addr);
    juce::String oscAddress();
    
    void setOscPort(const int port);
    void setOscPort(const juce::String& port);
    int oscPort();
    
    void setLayoutRotated(const bool isRotated);
    bool isLayoutRotated();
    
    /**
     * @brief Save current sources status.
     */
    bool save(const juce::File& file);
    
    /**
     * @brief Load sources status.
     */
    bool load(const juce::File& file);
    
    /**
     * @brief The class constructor.
     */
    WFSRendererParameters(AudioProcessor& p);
    
    AudioProcessorValueTreeState& valueTree() { return m_valueTreeState; }
    
};
