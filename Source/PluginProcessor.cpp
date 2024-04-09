/*
 ==============================================================================
 
 This file is part of the ambix Ambisonic plug-in suite.
 Copyright (c) 2013/2014 - Matthias Kronlachner
 www.matthiaskronlachner.com
 
 Permission is granted to use this software under the terms of:
 the GPL v2 (or any later version)
 
 Details of these licenses can be found at: www.gnu.org/licenses
 
 ambix is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 ==============================================================================
 */
#include <JuceHeader.h>

#include "ui/Resources.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

#ifdef __APPLE__
    #include <unistd.h>
#else
    #ifdef _WIN32
        #include <windows.h>/* defines _WINDOWS_ macro */
    #else
        #include <unistd.h>
        #include <dlfcn.h>
        #include <sys/mman.h>
    #endif
#endif

//==============================================================================
int WFSRendererAudioProcessor::s_ID = 0; // for counting id!

WFSRendererAudioProcessor::WFSRendererAudioProcessor()
: juce::AudioProcessor(juce::AudioProcessor::BusesProperties()
                       .withInput("WfsSources",  AudioChannelSet::discreteChannels(MAX_SOURCES))
                       .withInput("SyncIn",      AudioChannelSet::mono())
                       .withOutput("WfsOutputs", AudioChannelSet::discreteChannels(MAX_OUTPUTS))
                       .withOutput("SynchOut",   AudioChannelSet::mono())),
  m_parameters(*this) // esseci
{
    // x, y, size
    // if more than one channel add width parameter: all sources are aligned along with equal distance
    
    const int nSources       = getTotalNumInputChannels();
    m_numParameters          = nSources * 10;
    m_oscDecoderBusy         = false;
    m_outputDisabledTimeout  = 0;
    m_shmem.reset(new Shmem("wfs", 4096, true));

#ifndef _MSC_VER
    if (m_shmem->data() == nullptr)
    {
        std::cerr << "Cannot open shared memory.";
        m_shmem.reset(nullptr);
    }
    else
    {
        m_shmem->lock();
    }
#endif
}

WFSRendererAudioProcessor::~WFSRendererAudioProcessor()
{
    if (m_oscCodec)
    {
#if RENDERER_ID == 1
        auto cb = dynamic_cast<WfsOscCodecBroadcaster*>(m_oscCodec.get());
        cb->removeParameters();
#else
        auto rc = dynamic_cast<WfsOscCodecReceiver*>(m_oscCodec.get());
        rc->addListener(nullptr);
        
        while (m_oscDecoderBusy.load())
        {
            std::cout << "still processing\n";
        }
#endif
    }
    m_oscCodec = nullptr;
    
    WFSRendererAudioProcessor::s_ID--; // instance counter
    
    if (_sources)
    {
        delete [] _sources;
    }
    Resources::deleteInstance();
}

bool WFSRendererAudioProcessor::LoadConfiguration(const juce::File& configFile)
{
    if (! configFile.existsAsFile())
    {
        std::cerr << "Configuration file: "
                  << configFile.getFullPathName().toRawUTF8()
                  << " not found.\n";
        return false;
    }
        
    // Busy waiting until the current processing is termined...
    while (m_isProcessing)
    {
        SleepMs(1);
    }
    // ... then disable output for 2 seconds
    m_outputDisabledTimeout = (int)(2.0 / ((double)m_samplesPerBlock/getSampleRate()));
    
    std::cout << "Unloading Config..." << std::endl;
    UnloadConfiguration();
    m_layout.reset(new WFSlayout(configFile));
    m_parameters.setMaxDist(m_layout->maxdist());

    if (m_oscCodec)
    {
        auto bc = dynamic_cast<WfsOscCodecBroadcaster*>(m_oscCodec.get());

        if (bc)
        {
            bc->setParameters(&m_parameters);
            bc->sendConfigFilePath(configFile.getFullPathName());
        }
    }
    Source::_layout = m_layout.get();
    
    _sources = new Source [m_parameters.numberOfSources()];
    const int rid = m_parameters.rendererId();
    
    for (int i = 0; i < m_parameters.numberOfSources(); ++i)
    {
        _sources[i].init(rid);
    }

    std::cout << "Sample Rate: "      << m_sampleRate
              << " SamplesPerBlock: " << m_samplesPerBlock
              << " delta: "           << Source::_delta << std::endl;
    
    m_configLoaded = true;
    m_parameters.setLayoutFilePath(configFile);
    
    auto& msg = getChangeBroadcasterMessage();
    msg.messageID = WfsChangeBroadcaster::Message::Type::kLoadConfiguration;
    msg.contents  = configFile.getFullPathName();
    sendChangeMessage();
    
    const int periodMs = (int)(1000.0 * m_parameters.paramPeriod()
                                        / getSampleRate());
    startTimer(periodMs);
    return true;
}

void WFSRendererAudioProcessor::UnloadConfiguration()
{
    // delete configuration
    m_configLoaded = false;
}

bool WFSRendererAudioProcessor::ReloadConfiguration()
{
    if (m_configLoaded)
    {
        juce::File configFile = m_parameters.layoutFilePath();
        return LoadConfiguration(configFile);
    }
    return false;
}

bool WFSRendererAudioProcessor::startOSC()
{
    auto addr = m_parameters.oscAddress();
    int port  = m_parameters.oscPort();
    
#if RENDERER_ID == 1
    m_oscCodec.reset(new WfsOscCodecBroadcaster);
    
    if (! m_oscCodec->connect(port, addr))
    {
        m_oscCodec = nullptr;
        return false;
    }
    auto bc = dynamic_cast<WfsOscCodecBroadcaster*>(m_oscCodec.get());
    
    if (m_layout)
    {
        bc->setParameters(&m_parameters);
    }
    
#else
    m_oscCodec.reset(new WfsOscCodecReceiver);
    
    if (! m_oscCodec->connect(port, addr))
    {
        m_oscCodec = nullptr;
        return false;
    }
    auto rc = dynamic_cast<WfsOscCodecReceiver*>(m_oscCodec.get());
    rc->addListener(this);
#endif
    return true;
}

void WFSRendererAudioProcessor::stopOSC()
{
    if (m_oscCodec)
    {
        m_oscCodec = nullptr;
    }
}

bool WFSRendererAudioProcessor::isOSCActive() const
{
    if (m_oscCodec)
    {
        return m_oscCodec->isActive();
    }
    return false;
}

void WFSRendererAudioProcessor::oscMessageDispatched(WfsOscCodec::Message& message)
{
#if RENDERER_ID > 1
    m_oscDecoderBusy = true;
    
    message.init();
    
    const char* cmd = message.getAddress();
    
    if (cmd == nullptr)
    {
        std::cerr << "Invalid command.\n";
        return;
    }
    
    if(strstr(cmd, "wfs_rend" ) != nullptr)
    {
        int32_t i;
        int len;
        const char *s;
        float  f;
        message.getInt(&i);           // sender ID:   unused
        message.getString(&s, &len);  // test string: unused
        message.getFloat(&f);         // distance:    unused
        
        for (int n = 0; n < m_parameters.numberOfSources(); ++n)
        {
            message.getFloat(&f);
            m_parameters[n].x = f;
            
            message.getFloat(&f);
            m_parameters[n].y = f;

            message.getFloat(&f);
            *m_parameters[n].gain = f;

            message.getInt(&i);
            *m_parameters[n].mute = (bool)i;
            
            message.getInt(&i);
            *m_parameters[n].solo = (bool)i;
        }
        message.getInt(&i); // the sender local port.
    }
    else if(strstr(cmd, "wfs_load" ) != nullptr)
    {
        int32_t i;
        int len;
        const char *s;
        message.getInt(&i);           // sender ID
        message.getString(&s, &len);  // configuration file path
        
        if (i == 1) // only the master can send the config file path.
        {
            // s contains the
            juce::File configFile = juce::String(s);
            LoadConfiguration(configFile);
        }
    }
    // idle message doesn't need decoding...
    
    m_oscDecoderBusy = false;
#endif
}

void WFSRendererAudioProcessor::modalStateFinished(int returnValue)
{
    switch(returnValue)
    {
        case 100: // load pressed
            break;
        case 101: // save pressed
            break;
    }
    
}

//==============================================================================
const String WFSRendererAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

const String WFSRendererAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String WFSRendererAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool WFSRendererAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WFSRendererAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool WFSRendererAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double WFSRendererAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WFSRendererAudioProcessor::getNumPrograms()
{
    return 0;
}

int WFSRendererAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WFSRendererAudioProcessor::setCurrentProgram (int /*index*/)
{
    
}

const String WFSRendererAudioProcessor::getProgramName (int /*index*/)
{
    return String();
}

void WFSRendererAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void WFSRendererAudioProcessor::parameterValueChanged  (int parameterIndex,
                                                        float newValue)
{
#if RENDERER_ID > 1
    std::cout << "[" << RENDERER_ID << "] parameter " << parameterIndex
              << " changed to " << newValue << "\n";
#endif
}

void WFSRendererAudioProcessor::parameterGestureChanged(int /*parameterIndex*/,
                                                        bool /*gestureIsStarting*/ )
{
    
}

void WFSRendererAudioProcessor::timerCallback()
{    
    const double dt = m_parameters.paramPeriod() / getSampleRate();
    m_parameters.update(dt);
    
    const auto  dmax = m_parameters.maxDist();
    const bool solos = m_parameters.thereAreSolos();
        
    for (int i = 0; i < m_parameters.numberOfSources(); i++)
    {
        auto& s = m_modelData._sources[i];
        auto& p = m_parameters[i];
        s._xpos = 0.01f * (p.x - 50.0f) * dmax;
        s._ypos = 0.01f * (p.y - 50.0f) * dmax;
        s._gain = juce::Decibels::decibelsToGain<float>(*p.gain,
                                                        minusInfinitydB);
        if (*p.mute || (solos && ! *p.solo))
        {
            s._gain = 0.0f;
        }
    }

    getChangeBroadcasterMessage().messageID = WfsChangeBroadcaster::Message::Type::kUpdateUI;
    sendChangeMessage();
}

void WFSRendererAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    m_sampleRate = sampleRate;
    m_samplesPerBlock = samplesPerBlock;
    
    m_inputBuffer.setSize(m_parameters.numberOfSources(), samplesPerBlock);
    m_syncSignal.setSize(1, samplesPerBlock);
    
    Source::_fsamp  = (float)sampleRate;
    Source::_fsize  = samplesPerBlock;
    Source::_delta  = m_parameters.paramPeriod();
    Source::_modul  = m_timerx.modul(); // !!!!!!
    Source::_layout = m_layout.get();
        
    if(! m_configLoaded)
    {
        juce::File f = m_parameters.layoutFilePath();
        LoadConfiguration(f);
    }
}

void WFSRendererAudioProcessor::releaseResources()
{
    
    /* DiracFxReset(true, mDiracFx);*/
    
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void WFSRendererAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& /*midiMessages*/)
{
    m_isProcessing = true;
    
    if (!m_configLoaded)
    {
        ++m_cycle;

        // if nothing is loaded just clear the buffer...
        buffer.clear();
    }
    else
    {
        int t = -1;
        //int m = MUTE;
        
//#if OUTPUT_CHANNELS == 64
        if (m_timerx.synced ()) // this code appears only in wfsrender
        {
            t = m_timerx.timer ();
            //m = _mode;
        }
//#else
        
//#endif
        ++m_cycle;
        
#if RENDERER_ID == 1
        auto md = (Modeldata*)m_shmem->data();
        
        if (md != nullptr)
        {
            md->_header._timer = m_timetx.timer();
        }
#endif
        const int numSamples = buffer.getNumSamples();
        const int nSources = getTotalNumInputChannels();
        
        for (int i=0; i < nSources; i++)
        {
            m_inputBuffer.copyFrom(i, 0, buffer.getReadPointer(i), numSamples);
        }
        
        // clear output buffer
        buffer.clear();
                        
        if (m_outputDisabledTimeout > 0)
        {
            // when timeout is set, no processing will be done.
            m_outputDisabledTimeout--;
        }
        else
        {
            for (int i = 0; i < nSources; i++)
            {
                _sources[i].params(&m_modelData._header, &m_modelData._sources[i]);
                _sources[i].render(m_inputBuffer.getReadPointer(i),
                                   buffer.getArrayOfWritePointers(),
                                   numSamples,
#if OUTPUT_CHANNELS == 64
                                   t, -1);
#else
                                   0, -1);
#endif
            }
        }
        auto ptime = m_syncSignal.getWritePointer(0);
        
#if RENDERER_ID == 1
        m_timetx.process(numSamples, ptime);
#endif
        m_timerx.process(numSamples, ptime);
        
#if 0 //RENDERER_ID > 1
        if (m_cycle % 10 == 0)
        {
            float rmsout = buffer.getRMSLevel(0, 0, numSamples);
            float mag = buffer.getMagnitude(0, 0, numSamples);

            std::cout << " o " << rmsout << " m " << mag << "\n";
        }
#endif
    }
    m_isProcessing = false;
}

//==============================================================================
bool WFSRendererAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WFSRendererAudioProcessor::createEditor()
{
    auto cache = Resources::getInstance()->cache();
    return new WFSRendererAudioProcessorEditor (this, cache);
}

//==============================================================================
void WFSRendererAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::ValueTree state = m_parameters.valueTree().copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void WFSRendererAudioProcessor::setStateInformation (const void* data,
                                                     int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr && xmlState->hasTagName(m_parameters.valueTree().state.getType()))
    {
        m_parameters.valueTree().replaceState(ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WFSRendererAudioProcessor();
}


