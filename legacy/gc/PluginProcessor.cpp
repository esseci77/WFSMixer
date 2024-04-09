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

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "DiracFxAU.h"

/*
#include <unistd.h>*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#ifdef __APPLE__
    #include <unistd.h>
    #include <dlfcn.h>
    #include <sys/mman.h>
//#if defined(WIN32) || defined(_Windows) || defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
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



WFSRendererAudioProcessor::WFSRendererAudioProcessor():
myProperties(),
box_preset_str("no preset loaded"),
configLoaded(false),
InputBuffer(INPUT_CHANNELS, 512),
isProcessing(false)
{
    // x, y, size
    // if more than one channel add width parameter: all sources are aligned along with equal distance
#ifdef __APPLE__
    presetDir = presetDir.getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Application Support").getChildFile("WFSRenderer").getChildFile("config");
#else
presetDir = presetDir.getSpecialLocation(File::userApplicationDataDirectory).getChildFile("WFSRenderer").getChildFile("config");
#endif
	SearchPresets(presetDir);
    
    NumParameters = INPUT_CHANNELS*5;
    
    PropertiesFile::Options prop_options;
    
    if(RENDERER_ID==1)
        prop_options.applicationName = "settings1";
    if(RENDERER_ID==2)
        prop_options.applicationName = "settings2";
    if(RENDERER_ID==3)
        prop_options.applicationName = "settings3";
    
    prop_options.commonToAllUsers = false;
    prop_options.filenameSuffix = "xml";
    prop_options.folderName = "WFSRenderer/settings";
    prop_options.storageFormat = PropertiesFile::storeAsXML;
    // options.storageFormat = PropertiesFile::storeAsBinary;
    prop_options.ignoreCaseOfKeyNames = true;
    prop_options.osxLibrarySubFolder = "Application Support";
    
    myProperties.setStorageParameters(prop_options);
}

WFSRendererAudioProcessor::~WFSRendererAudioProcessor()
{
    WFSRendererAudioProcessor::s_ID--; // instance counter
    if(configLoaded){
        oscIn(false);
#if WITH_OSC
        
        oscOut(false);
#endif
        delete layout;
        myProperties.closeFiles();
    }
    
}



void WFSRendererAudioProcessor::SearchPresets(File SearchFolder)
{
    _presetFiles.clear();
    
    SearchFolder.findChildFiles(_presetFiles, File::findFiles, true, "*.xml");
    std::cout << "Found preset files: " << _presetFiles.size() << std::endl;
    
}


void WFSRendererAudioProcessor::LoadPreset(unsigned int preset)
{
    if (preset < (unsigned int)_presetFiles.size())
    {
        // ScheduleConfiguration(_presetFiles.getUnchecked(preset));
        LoadConfiguration(_presetFiles.getUnchecked(preset));
    }
}

void WFSRendererAudioProcessor::LoadPresetByName(String presetName)
{
    Array <File> files;
    presetDir.findChildFiles(files, File::findFiles, true, presetName);
    
    if (files.size())
    {
        LoadConfiguration(files.getUnchecked(0)); // Load first result
        box_preset_str = files.getUnchecked(0).getFileNameWithoutExtension();
    }
    
}


void WFSRendererAudioProcessor::LoadConfiguration(File configFile)
{
    if (!configFile.existsAsFile())
    {
        
        return;
    }
    
    // unload first....
    if (configLoaded) {
        
        while (isProcessing) {
		#ifdef __APPLE__
			usleep(1000);
		#else
            #ifdef _WIN32
                Sleep(1);
            #else
                usleep(1000);
            #endif
		#endif

        }
        
        std::cout << "Unloading Config..." << std::endl;
        UnloadConfiguration();
        layout = new WFSlayout(configFile);
        maxdist = layout->maxdist();
        
    }else{
        
        osc_in = false;
        osc_in_port="0";
        
//        char in[10];
//        std::sprintf(in, "%d", OSC_IN_PORT);
//        osc_in_port = in;
                osc_in_port = myProperties.getUserSettings()->getValue("osc_in_port", "7130");
//        osc_in = true;
                osc_in = myProperties.getUserSettings()->getBoolValue("osc_in", true);
        std::cout<< "osc_in_port: "<<osc_in_port<<std::endl;
        oscReceiver.reset(new OSCReceiver);
        oscIn(osc_in);
        
#if WITH_OSC
        osc_out = false;
        //	osc_in_port = "7120";
//        char out[20];
//        osc_out_ip = "127.0.0.1";
        osc_out_ip = myProperties.getUserSettings()->getValue("osc_out_ip", "localhost");
        
//        std::sprintf(out, "%d;%d", OSC_OUT_PORT,OSC_OUT_PORT2);
//        osc_out_port = out;
                osc_out_port = myProperties.getUserSettings()->getValue("osc_out_port", "7130");
        //
        
//        osc_interval = 1;
                osc_interval = myProperties.getUserSettings()->getIntValue("osc_out_interval", 50);
        //
//        osc_out =true;
                osc_out = myProperties.getUserSettings()->getBoolValue("osc_out", true);
        std::cout<< "osc_out_port: "<<osc_out_port<<std::endl;
        oscOut(osc_out);
        
        //        osc_error = "OSC: not receiving";
        
        
        
        //        startTimer(30); // osc send rate
#endif
        activePreset = configFile.getFileName(); // store filename only, on restart search preset folder for it!
        box_preset_str = configFile.getFileNameWithoutExtension();
        
        layout = new WFSlayout(configFile);
        maxdist = layout->maxdist();
        
    }
    
    Source::_fsamp = SampleRate;
    Source::_fsize = 512;
    
    Source::_delta = 8191; //to be verified
    Source::_modul = 32768*32;
    //     Source::_noutp = OUTPUT_CHANNELS;
    Source::_layout = layout;
    
    //Source::_cindex = _cindex;
    _nsrce = INPUT_CHANNELS;
    _sources = new Source [_nsrce];
   // _sources[0].processor = this;
    std::cout << "Sample Rate: "<<SampleRate<<" SamplesPerBlock: "<<SamplesPerBlock<<" delta: "<< Source::_delta << std::endl;
    
    configLoaded = true;
    _configFile = configFile;
    
    
    for (int i=0; i < _nsrce; i++) {
        
//        AudioParameterFloat* xParam;
//        AudioParameterFloat* yParam;
//        AudioParameterFloat* SizeParam;
//        AudioParameterFloat* XMvParam;
//        AudioParameterFloat* YMvParam;
//        AudioParameterFloat* SpeedParam;
        
       
        AudioParameterFloat* xParam = new AudioParameterFloat("xParam"+std::to_string(i), "xParam"+std::to_string(i), 0.0f, 1.0f, 0.5f);
        AudioParameterFloat* yParam = new AudioParameterFloat("yParam"+std::to_string(i), "YParam"+std::to_string(i), 0.0f, 1.0f, 0.5f);
//        AudioParameterFloat* SizeParam = new AudioParameterFloat("SizeParam"+i, "SizeParam"+i, 0.0f, 1.0f, 0.9f);
        AudioParameterFloat* XMvParam = new AudioParameterFloat("XMvParam"+std::to_string(i), "XMvParam"+std::to_string(i), 0.0f, 1.0f, 0.5f);
        AudioParameterFloat* YMvParam = new AudioParameterFloat("YMvParam"+std::to_string(i), "YMvParam"+std::to_string(i), 0.0f, 1.0f, 0.5f);
        AudioParameterFloat* SpeedParam = new AudioParameterFloat("SpeedParam"+std::to_string(i), "SpeedParam"+std::to_string(i), 0.0f, 1.0f, 0.5f);
        
        
        
        std::cout << "PARAMS: "<<this->getNumParameters()<<" AP: "<<AudioProcessor::getNumParameters() << std::endl;
        
        
        addParameter(xParam);
        addParameter(yParam);
//        addParameter(SizeParam);
        addParameter(XMvParam);
        addParameter(YMvParam);
        addParameter(SpeedParam);
        
        std::cout << "PARAMS: "<<this->getNumParameters()<<" AP: "<<AudioProcessor::getNumParameters() << std::endl;
        

    }
    
    
    
    //    sendChangeMessage(); // notify editor
    
    
}



void WFSRendererAudioProcessor::UnloadConfiguration()
{
    // delete configuration
    configLoaded = false;
    
}

void WFSRendererAudioProcessor::ReloadConfiguration()
{
    if (configLoaded)
        LoadConfiguration(_configFile);
}



#if WITH_OSC

void WFSRendererAudioProcessor::timerCallback() // check if new values and call send osc
{
 
    if (osc_out)
    {
   
        if (_x_param != x_param ||
            _y_param != y_param ||
            _x_mv_param != x_mv_param ||
            _y_mv_param != y_mv_param ||
            _speed_param != speed_param){
           
            sendOSC();
        }
    }
    
}

void WFSRendererAudioProcessor::sendOSC() // send osc data
{
    
    if (osc_out)
    {
        OSCMessage mymsg = OSCMessage("/wfs_rend");
        mymsg.addInt32(RENDERER_ID); // source id
        mymsg.addString("test"); // name... currently unused
        mymsg.addFloat32(2.0f); // distance... currently unused
        mymsg.addFloat32(maxdist*(x_param-0.5f)); // azimuth -180....180°
        mymsg.addFloat32(maxdist*(y_param-0.5f)); // elevation -180....180°
        mymsg.addFloat32(x_mv_param);
        mymsg.addFloat32(y_mv_param);
        mymsg.addFloat32(speed_param);
        
        
        
        if(osc_in)
        {
            
            mymsg.addInt32(osc_in_port.getIntValue()); // osc receiver udp port
            
            /*
             
             //            std::cout << "osc_in: " <<  << std::endl;
             //          [gc]
             //            lo_send(addr,"/ambi_enc", "fsffffffi", (float)m_id, "test", 2.0f, 360.f*(x_param-0.5f), 360.f*(y_param-0.5f), size_param, dpk, rms, osc_in_port.getIntValue());
             lo_send(addr,"/ambi_enc", "fsffffffi", (float)m_id, "test", 2.0f, layout->maxdist()*(x_param-0.5f), layout->maxdist()*(y_param-0.5f), size_param, dpk, rms, osc_in_port.getIntValue());
             lo_send(addr2,"/ambi_enc", "fsffffffi", (float)m_id, "test", 2.0f, layout->maxdist()*(x_param-0.5f), layout->maxdist()*(y_param-0.5f), size_param, dpk, rms, osc_in_port.getIntValue());
             } else {
             //            [gc]
             //            lo_send(addr,"/ambi_enc", "fsffffff", (float)m_id, "test", 2.0f, 360.f*(x_param-0.5f), 360.f*(y_param-0.5f), size_param, dpk, rms);
             lo_send(addr,"/ambi_enc", "fsffffff", (float)m_id, "test", 2.0f, layout->maxdist()*(x_param-0.5f),layout->maxdist()*(y_param-0.5f), size_param, dpk, rms);
             lo_send(addr2,"/ambi_enc", "fsffffffi", (float)m_id, "test", 2.0f, layout->maxdist()*(x_param-0.5f), layout->maxdist()*(y_param-0.5f), size_param, dpk, rms);
             */
        }
        for (int i = 0; i < oscSenders.size(); i++) {
            oscSenders.getUnchecked(i)->send(mymsg);
        }
        
        
        _x_param = x_param; // change buffers
        _y_param = y_param;
        _x_mv_param = x_mv_param;
        _y_mv_param = y_mv_param;
        _speed_param = speed_param;
    }
}








void WFSRendererAudioProcessor::oscOut(bool arg)
{
    /*    if (arg) {
     addr = lo_address_new(osc_out_ip.toUTF8(), osc_out_port.toUTF8());
     addr2 = lo_address_new(osc_out_ip.toUTF8(), osc_out_port2.toUTF8());
     std::cout << "OSC sending on ports: " << osc_out_port << ", "<< osc_out_port2 <<std::endl;
     arg = true;
     } else { // turn off osc out
     arg = false;
     }
     return arg;*/
    
    if (osc_out) {
        stopTimer();
        
        oscSenders.clear();
        
        osc_out = false;
        
    }
    
    if (arg)
    {
        bool suc = false;
        
        // parse all ip/port configurations
        
        String tmp_out_ips = osc_out_ip.trim();
        String tmp_out_ports = osc_out_port.trim();
        
        String tmp_ip;
        String tmp_port;
        
        while (tmp_out_ips.length() > 0 || tmp_out_ports.length() > 0) {
            
            if (tmp_out_ips.length() > 0)
                tmp_ip = tmp_out_ips.upToFirstOccurrenceOf(";", false, false);
            
            if (tmp_out_ports.length() > 0)
                tmp_port = tmp_out_ports.upToFirstOccurrenceOf(";", false, false);
            
            // create new sender
            
            if (tmp_ip.equalsIgnoreCase("localhost"))
                tmp_ip = "127.0.0.1";
            
            oscSenders.add(new OSCSender());
            suc = oscSenders.getLast()->connect(tmp_ip, tmp_port.getIntValue()) ? true : suc;
            
            
            // std::cout << "add sender: " << tmp_ip << " " << tmp_port << " success: " << suc << std::endl;
            
            // trim
            tmp_out_ips = tmp_out_ips.fromFirstOccurrenceOf(";", false, false).trim();
            
            tmp_out_ports = tmp_out_ports.fromFirstOccurrenceOf(";", false, false).trim();
            
        }
        
        if (suc)
        {
            osc_out = true;
            startTimer(osc_interval); // osc send rate
        }
        
        
    }
    
}

void WFSRendererAudioProcessor::changeTimer(int time)
{
    if (osc_out)
    {
        stopTimer();
        osc_interval = time;
        startTimer(time);
    }
    
}

#endif


// this is called if an OSC message is received
void WFSRendererAudioProcessor::oscMessageReceived (const OSCMessage& message)
{
    // /ambi_enc_set <id> <distance> <azimuth> <elevation> <size>
    
    // parse the message for int and float
    float val[8];
    
    for (int i=0; i < jmin(8,message.size()); i++) {
        
        val[i] = 0.5f;
        
        // get the value wheter it is a int or float value
        if (message[i].getType() == OSCTypes::float32)
        {
            val[i] = (float)message[i].getFloat32();
        }
        else if (message[i].getType() == OSCTypes::int32)
        {
            val[i] = (float)message[i].getInt32();
        }
        
    }
   
    
    //    setParameterNotifyingHost(Ambix_encoderAudioProcessor::AzimuthParam, jlimit(0.f, 1.f, (val[2]+180.f)/360.f) );
  
    setParameterNotifyingHost(0, jlimit(0.f, 1.f,(val[3]+maxdist*0.5f)/maxdist));
    //    setParameterNotifyingHost(Ambix_encoderAudioProcessor::ElevationParam, jlimit(0.f, 1.f, (val[3]+180.f)/360.f) );
    
    setParameterNotifyingHost(1, jlimit(0.f, 1.f, (val[4]+maxdist*0.5f)/maxdist));
    setParameterNotifyingHost(2, val[5]);
    setParameterNotifyingHost(3, val[6]);
    setParameterNotifyingHost(4, val[7]);
    //    setParameterNotifyingHost(Ambix_encoderAudioProcessor::SizeParam, jlimit(0.f, 1.f, val[4]));
//    setParameterNotifyingHost(WFSRendererAudioProcessor::SizeParam, jlimit(0.f, 1.f, val[5]));
    
    
}

/*
 // /ambi_enc_set <id> <distance> <x> <y> <size>
 int ambi_enc_set_handler(const char *path, const char *types, lo_arg **argv, int argc,
 void *data, void *user_data)
 {
 WFSRendererAudioProcessor *me = (WFSRendererAudioProcessor*)user_data;
 //	[gc]
 //    me->setParameterNotifyingHost(WFSRendererAudioProcessor::xParam, (argv[2]->f+180.f)/360.f);
 //    me->setParameterNotifyingHost(WFSRendererAudioProcessor::yParam, (argv[3]->f+180.f)/360.f);
 
 //std::cout << "received something. x:" << argv[3]->f << "el: "<<argv[4]->f << "size: "<< argv[5]->f <<std::endl;
 me->setParameterNotifyingHost(WFSRendererAudioProcessor::xParam, (argv[3]->f+me->layout->maxdist()/2)/me->layout->maxdist());
 me->setParameterNotifyingHost(WFSRendererAudioProcessor::yParam, (argv[4]->f+me->layout->maxdist()/2)/me->layout->maxdist());
 
 me->setParameterNotifyingHost(WFSRendererAudioProcessor::SizeParam, argv[5]->f);
 
 return 0;
 }
 
 void error(int num, const char *msg, const char *path)
 {
 std::cout << "liblo server error " << num << "in path " << path << ": " << msg << std::endl;
 }
 */
void WFSRendererAudioProcessor::oscIn(bool arg)
{
    
    if (arg) {
        
        //        int port = 7200+m_id;
        int port = osc_in_port.getIntValue();
        
        //        Random rand(Time::currentTimeMillis());
        
        //        int trials = 0;
        //
        //        while (trials++ < 10)
        //        {
        //            // std::cout << "try to connect to port " << port << std::endl;
        
        
        if (oscReceiver->connect(port))
        {
            //                oscReceiver->addListener (this, "/ambi_enc_set");
            oscReceiver->addListener (this, "/wfs_rend");
            osc_in_port = String(port);
            osc_in = true;
            
            //                break;
        }
        
        //            port += rand.nextInt(999);
        
        //        }
        
    } else { // turn off osc
        
        osc_in = false;
        
        oscReceiver->removeListener(this);
        
        oscReceiver->disconnect();
        
    }
    /*
     if (arg) {
     
     st = lo_server_thread_new(osc_in_port.toUTF8(), error); //choose port by itself (free port)
     //                st = lo_server_thread_new(NULL, error); //choose port by itself (free port)
     
     //		lo_server_thread_add_method(st, "/ambi_enc_set", "fffff", ambi_enc_set_handler, this);
     lo_server_thread_add_method(st, "/ambi_enc", "fsffffffi", ambi_enc_set_handler, this);
     int res = lo_server_thread_start(st);
     
     if (res == 0)
     {
     osc_in_port = String(lo_server_thread_get_port(st));
     arg = true;
     osc_in = true;
     osc_error = "OSC: receiving on port ";
     osc_error += osc_in_port;
     std::cout << "" << osc_error << std::endl;
     
     } else {
     osc_error = "OSC: ERROR port is not free";
     //osc_error.formatted("OSC: ERROR %s", lo_address_errstr());
     arg = false;
     osc_in = false;
     }
     } else { // turn off osc
     if (st != NULL)
     {
     lo_server_thread_stop(st);
     #ifndef WIN32
     lo_server_thread_free(st); // this crashes in windows
     #endif
     }
     arg = false;
     osc_in = false;
     osc_error = "OSC: not receiving";
     }
     return arg;*/
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

bool WFSRendererAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool WFSRendererAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
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

void WFSRendererAudioProcessor::setCurrentProgram (int index)
{
}

const String WFSRendererAudioProcessor::getProgramName (int index)
{
    return String();
}

void WFSRendererAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void WFSRendererAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    SampleRate = sampleRate;
    SamplesPerBlock = samplesPerBlock;
    
    /* mDiracFx = DiracFxCreate(kDiracQualityBevst, SampleRate, 1);*/
    //    std::cout << "# output channels: " << getNumOutputChannels() << std::endl;
    
    //
    // init meter dsp
    //_my_meter_dsp.setAudioParams((int)SampleRate, samplesPerBlock);
    //_my_meter_dsp.setParams(0.5f, 20.0f);
    
    if(!configLoaded)LoadConfiguration(_presetFiles[0]);
    //#if WITH_OSC
    //    sendOSC();
    //#endif
}

void WFSRendererAudioProcessor::releaseResources()
{
    
    /* DiracFxReset(true, mDiracFx);*/
    
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}



void WFSRendererAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    count++;
    isProcessing=true;
    if (!configLoaded)
    {
        // if nothing is loaded just clear the buffer...
        buffer.clear();
        
    } else {
        
        int NumSamples = buffer.getNumSamples();
//        std::cout<<"NumSamples: "<<NumSamples<<std::endl;
#if WITH_ADVANCED_CONTROL
        // calculate new x and y parameters if move
        calcNewParameters(SampleRate, NumSamples);
#endif
        
        
        /*************************/
        //SEE old ambix_test part1
        /*************************/
        
        // clear input buffer and copy input samples
        InputBuffer.clear();
        
        //        for (int i=0; i < std::min(getNumInputChannels(), INPUT_CHANNELS); i++) {
        for (int i=0; i < INPUT_CHANNELS; i++) {
            InputBuffer.copyFrom(i, 0, buffer, i, 0, NumSamples);
            // std::cout << "copied buffer channel " << i << std::endl;
        }
        // clear output buffer
        buffer.clear();
        
        
        //[gc] xpos and ypos not normalized
        //        _sources[0].params((x_param-0.5f)*layout->maxdist(), -(y_param-0.5f)*layout->maxdist(), 1);
//        setParameterNotifyingHost(WFSRendererAudioProcessor::T1_startIpol, x_param);
//        setParameterNotifyingHost(WFSRendererAudioProcessor::GA_startIpol, y_param);
        
        //         _sources[0].params((x_param-0.5f)*maxdist, -(y_param-0.5f)*maxdist);
        
//        if(count%2)
     //[gc] even if params updated in another thread(es. timercallback of OSC), the click remains, so the frequency of params() call is not a problem
        for (int i=0; i < _nsrce; i++) {
            _sources[i].params((x_param-0.5f)*maxdist, -(y_param-0.5f)*maxdist);
            _sources[i].render(InputBuffer.getReadPointer(0),buffer.getArrayOfWritePointers(), NumSamples,0, -1);
        }
        
        /*//ALTERNATIVE RENDER SOURCE, WITH LESS CONTROLS, DIRECTLY IN PluginProcessor
         
         
         int opind;
         
         // Apply filters and write to delay lines.
         _sources[0]._filter.process (NumSamples, InputBuffer.getReadPointer(0), _sources[0]._delayA + _sources[0]._delind, _sources[0]._delayB + _sources[0]._delind);
         if (_sources[0]._delind == 0)
         {
         _sources[0]._delayA [_sources[0]._dsize] = _sources[0]._delayA [0];
         _sources[0]._delayB [_sources[0]._dsize] = _sources[0]._delayB [0];
         }
         _sources[0]._pars1 = _sources[0]._parlist.rd_ptr ();
         #ifdef TRACE
         puts ("-------");
         #endif
         opind = 0;
         _sources[0]._rmode = 3;
         if(_sources[0]._pars0){
         if (hypotf ((_sources[0]._pars1->_xpos) - (_sources[0]._pars0->_xpos), (_sources[0]._pars1->_ypos) - (_sources[0]._pars0->_ypos)) < 0.001f) // //[gc] if position is considered not changed
         {
         _sources[0]._rmode = 1;  //[gc] then hold the samples with same position and gain
         }
         
         }else{
         _sources[0]._rmode = 2;
         }
         
         switch (_sources[0]._rmode)
         {
         case 1: _sources[0].hold (buffer.getArrayOfWritePointers(), opind, NumSamples, -1); break;
         case 2: _sources[0].fade (buffer.getArrayOfWritePointers(), opind, NumSamples, -1); break;
         case 3: _sources[0].ipol (buffer.getArrayOfWritePointers(), opind, NumSamples, -1); break;
         //case 3: _sources[0].hold (buffer.getArrayOfWritePointers(), opind, NumSamples, -1); break;
         
         default:
         ;
         }
         _sources[0]._pars0 = _sources[0]._pars1;
         _sources[0]._pars1 = 0;
         if (_sources[0]._pars0) _sources[0]._parlist.rd_commit ();
         
         // 	Update delay lines write index.
         _sources[0]._delind += _sources[0]._fsize;
         _sources[0]._delind &= (_sources[0]._dsize - 1);
         
         */
        
        //SIMPLE OUTPUT FROM INPUT
        //        for (int i=0; i < OUTPUT_CHANNELS; i++) {
        //            buffer.addFrom(i, 0, InputBuffer, 0,  0, NumSamples);
        //            // std::cout << "copied buffer channel " << i << std::endl;
        //        }
        
        
        /*************************/
        //SEE old ambix_test part2
        /*************************/
        
        
    }
    
    isProcessing=false;
}

//==============================================================================
bool WFSRendererAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WFSRendererAudioProcessor::createEditor()
{
    return new WFSRendererAudioProcessorEditor (this);
}

//==============================================================================
void WFSRendererAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    
    XmlElement xml ("MYPLUGINSETTINGS");
    
    // add some attributes to it..
    for (int i=0; i < getNumParameters(); i++)
    {
        xml.setAttribute (String(i), getParameter(i));
    }
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void WFSRendererAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto xmlState = getXmlFromBinary (data, sizeInBytes);
    
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            for (int i=0; i < getNumParameters(); i++)
            {
                setParameter(i, xmlState->getDoubleAttribute(String(i)));
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WFSRendererAudioProcessor();
}


