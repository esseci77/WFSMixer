/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "WfsOscCodec.h"

#include "engine/atimeref.h"
#include "engine/shmem.h"
#include "engine/source.h"
#include "engine/wfslayout.h"

#define _2PI 6.2831853071795

#ifdef __APPLE__
#define SleepMs(x)  usleep(x * 1000);

#elif defined _WIN32
#define SleepMs(x)  Sleep(x);

#else
#define SleepMs(x)  usleep(x * 1000);

#endif



//==============================================================================
/**
 * @brief Just a \c ChangeBroadcaster with an integrated message
 */

class WfsChangeBroadcaster : public juce::ChangeBroadcaster
{
  public:
    struct Message
    {
        enum Type
        {
            kNull,
            kUpdateUI,
            kLoadConfiguration
        };
        
        Type messageID = kNull;
        juce::String   contents;
    };

    Message& getChangeBroadcasterMessage() { return m_message; }
    
  private:
    Message m_message;
};

//==============================================================================
// forwards
class WfsOscCodec;

// -----------------------------------------------------------------------------
// WFSRendererAudioProcessor definition
// -----------------------------------------------------------------------------
class WFSRendererAudioProcessor  : public AudioProcessor,
                                   public juce::AudioProcessorParameter::Listener,
                                   public juce::ModalComponentManager::Callback,
                                   public juce::Timer,
                                   public WfsOscCodec::Listener,
                                   public WfsChangeBroadcaster
{
public:
    //==========================================================================
    WFSRendererAudioProcessor();
    ~WFSRendererAudioProcessor();
    
    //==========================================================================
    /**
     * @brief Callback: riceve le variazioni dei parametri associati.
     */
    void parameterValueChanged  (int parameterIndex,
                                 float newValue) override;
    
    /**
     * @brief Callback: riceve le variazione delle gesture associate (NON USATA).
     */
    void parameterGestureChanged(int parameterIndex,
                                 bool gestureIsStarting) override;
    
    void timerCallback() override;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
    void processBlock (juce::AudioBuffer<float>& buffer,
                       juce::MidiBuffer& midiMessages) override;
    
    //==========================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==========================================================================
    const String getName() const override;
    
    int getNumParameters() override { return m_numParameters; };
    
    //============================+=============================================
    int  getNumPrograms   () override;
    int  getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void      changeProgramName (int index, const String& newName) override;
    
    //==========================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    
    bool isInputChannelStereoPair  (int /*index*/) const override { return false; }
    bool isOutputChannelStereoPair (int /*index*/ ) const override { return false; }
    
    bool acceptsMidi () const override;
    bool producesMidi() const override;
    bool   silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    bool startOSC();
    void stopOSC();
    bool isOSCActive() const;
    
    void oscMessageDispatched(WfsOscCodec::Message& message) override;
    
    /**
     * @brief Callback of the contextual menu.
     */
    void modalStateFinished(int returnValue) override;
    
    bool LoadConfiguration(const juce::File& configFile); // do the loading
    void UnloadConfiguration();
    bool ReloadConfiguration();
    
    WFSlayout* layout() { return m_layout.get(); }
    bool isConfigLoaded() const { return m_configLoaded; }

    WFSRendererParameters& params() { return m_parameters; }
    
private:
    std::unique_ptr<WFSlayout>   m_layout;
    std::unique_ptr<WfsOscCodec> m_oscCodec;
    WFSRendererParameters        m_parameters;
    
    long m_cycle = 0;
    static int s_ID; // global instance counter
    
    bool m_isProcessing   = false;
    bool m_configLoaded   = false; // is a configuration successfully loaded?
    
    double       m_sampleRate            = 0.0;
    int          m_samplesPerBlock       = 0;
    unsigned int m_numParameters         = 0;
    
    
    juce::AudioBuffer<float> m_inputBuffer;
    std::atomic<bool>  m_oscDecoderBusy;
    std::atomic<int>   m_outputDisabledTimeout;
    
    Modeldata m_modelData;
#if RENDERER_ID == 1
    Atimetx  m_timetx;
#endif
    Atimerx  m_timerx;
    juce::AudioBuffer<float> m_syncSignal; // dummy buffer that simulates
                                           // the original behaviour of a
                                           // dedicated jack audio sync port
    
    enum Mode { INIT, MUTE, TEST, REND };
    int m_mode = 0;
    
    std::unique_ptr<Shmem> m_shmem;
    
    //--------------------------------------------------------------------------
    //[gc]
    //--------------------------------------------------------------------------
    Source  *_sources = nullptr;
        
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WFSRendererAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
