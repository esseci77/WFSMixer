/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

//#include "../JuceLibraryCode/JuceHeader.h"


//////////////////////////////////////////////
// CHANGE AMBISONICS ORDER IN AppConfig.h !!!!
//////////////////////////////////////////////

#include "JuceHeader.h"

//#include "AmbixEncoder.h"

//#include "MyMeterDsp/MyMeterDsp.h"
#include "engine/source.h"
#include "wfslayout.h"

#define _2PI 6.2831853071795

//==============================================================================
/**
 */
class WFSRendererAudioProcessor  : public AudioProcessor,
#if WITH_OSC
public Timer,

#endif
private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::RealtimeCallback>,
public ChangeBroadcaster
{
public:
    //==========================================================================
    WFSRendererAudioProcessor();
    ~WFSRendererAudioProcessor();
    
    //==========================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    
    //==========================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;
    
    //==========================================================================
    const String getName() const;
    
    int getNumParameters();
    
    float getParameter (int index);
    void setParameter (int index, float newValue);
    
    const String getParameterName (int index);
    const String getParameterText (int index);
    const String getParameterLabel(int index);
    
    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;
    
    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;
    
    void calcX();
    float automVol(float db);
    float slideVol(float volu);
    
    
    WFSlayout *layout;
    
    float x_param     = 0.5f; // for multiple inputs this is the center
    float y_param     = 0.5f;
    float x_mv_param  = 0.5f;
    float y_mv_param  = 0.5f;
    float speed_param = 0.25f;
    
    
#if WITH_ADVANCED_CONTROL
    void calcNewParameters(double SampleRate, int BufferLength);
#endif
   /*enum param{
        xParam,
        yParam,
        XMvParam,
        YMvParam,
        SpeedParam,
    };*/
  /*  enum Parameters
    {
        xParam,
        yParam,
        SizeParam,
        XMvParam,
        YMvParam,
        SpeedParam,
        
        //[gc] only for debugging
        T1_startIpol,
        PA_delay1,
        PB_delay2,
        
        GA_startIpol,
        GB_startIpol,
        T1_endIpol,
        GA_endIpol,
        GB_endIpol,
        Dt_smp,
        DgA_smp,
        DgB_smp,
        T1_decimal,
        T2_decimal_recipr,
        K_initial,
        A_outp,
        B_outp,
        DtTot,
   
    };

*/
    //============================+=============================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);
    
    //==========================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    
    
    int m_id; // id of this instance
    static int s_ID; // global instance counter
    
    
    
    
    
    ApplicationProperties myProperties;
    // JUCE OSC
    void oscMessageReceived (const OSCMessage& message);
    
    
    void changeTimer(int time);
    
    int osc_interval;
    std::unique_ptr<OSCReceiver> oscReceiver;
    
    OwnedArray<OSCSender> oscSenders;
    
    void oscIn(bool arg); // activate osc in
    bool osc_in;
    String osc_error;
    String osc_in_port;
    
    
    
    
    
    //    lo_server_thread st;
    //
#if WITH_OSC
    void timerCallback(); // call osc send in timer callback
    
    void sendOSC(); // send osc data
    
    void oscOut(bool arg); // activate osc out
    
    
    // osc stuff
    
    bool osc_out;
    bool osc_out2;
    
    String osc_out_ip, osc_out_port, osc_out_port2;
    
    
    //	lo_address addr, addr2;
#endif
    
    
    
    
    File presetDir; // where to search for presets
    File lastDir; // for open file dialog...
    
    String activePreset; // store filename
    
    Array<File> _presetFiles;
    
    String box_preset_str;
    bool configLoaded; // is a configuration successfully loaded?
    bool _load_ir; // normally false, true if you don't want to load new impulse resonses (for fast decoder matrix switching)
    
    File _configFile;
    
    
    void SearchPresets(File SearchFolder);
    
    void LoadPreset(unsigned int preset);
    
    void LoadPresetByName(String presetName);
    
    void LoadConfiguration(File configFile); // do the loading
    
    void UnloadConfiguration();
    void ReloadConfiguration();
    
    
    
    
    
private:
    //    OwnedArray<AmbixEncoder> AmbiEnc;
    
    double SampleRate;
    int SamplesPerBlock;
    unsigned int NumParameters;
    void *mDiracFx;

    
    float _x_param     = 0.5f; // for multiple inputs this is the center
    float _y_param     = 0.5f;
    float _x_mv_param  = 0.5f;
    float _y_mv_param  = 0.5f;
    float _speed_param = 0.25f;
    
    
    
    float size_param;
    float width_param;  // arrange sources with equal angular distance
    
    // last osc value sent...
    
    // advanced parameter control
    //float x_set_param, x_set_rel_param,
    //float y_set_param, y_set_rel_param,
    
    AudioSampleBuffer InputBuffer;
    
    //MyMeterDsp _my_meter_dsp;
    
    float rms; // rms of W channel
    float dpk; // peak value of W channel
   
    /*float pA_delay1;
    float pB_delay2;
    float t1_startIpol;
    float gA_startIpol;
    float gB_startIpol;
    float t1_endIpol;
    float gA_endIpol;
    float gB_endIpol;
    float dt_smp;
    float dgA_smp;
    float dgB_smp;
    float t1_decimal;
    float t2_decimal_recipr;
    int k_initial;
    float a_outp;
    float b_outp;
    float dtTot;
    */
    float maxdist;
    int count;
    bool isProcessing;
    //--------------------------------------------------------------------------
    //[gc]
    //--------------------------------------------------------------------------
    int      _nsrce;
    
    Source  *_sources;
    
    
    
    /*
     void calcParams();
     
     Array<float> ambi_gain;
     Array<float> _ambi_gain; // buffer for gain ramp
     */
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WFSRendererAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
