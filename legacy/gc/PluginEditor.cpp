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


#include "PluginEditor.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
WFSRendererAudioProcessorEditor::WFSRendererAudioProcessorEditor (WFSRendererAudioProcessor* ownerFilter)
: AudioProcessorEditor (ownerFilter)
//#if INPUT_CHANNELS > 1
//sld_width (0),
//#endif
{
    
    if(ownerFilter->configLoaded){
        maxdist = getProcessor()->layout->maxdist();
        invdist = 1.0f/maxdist;
        hfdist = maxdist/2.0f;
        
        dsecsrce = getProcessor()->layout->dsecsrce();
        msize =  maxdist*mscale;
        
    }
    tooltipWindow.setMillisecondsBeforeTipAppears (700); // tooltip delay
    btn_open.reset(new TextButton ("new button"));
    addAndMakeVisible (btn_open.get());
    btn_open->setTooltip ("browse presets or open from file");
    btn_open->setButtonText ("open");
    btn_open->addListener (this);
    btn_open->setColour (TextButton::buttonOnColourId, Colours::blue);
    
    
    btn_preset_folder.reset(new TextButton ("new button"));
    addAndMakeVisible (btn_preset_folder.get());
    btn_preset_folder->setTooltip ("choose another preset folder");
    btn_preset_folder->setButtonText ("preset folder");
    btn_preset_folder->addListener (this);
    btn_preset_folder->setColour (TextButton::buttonOnColourId, Colours::blue);
    if(RENDERER_ID==1)
    {
        sld_y.reset(new Slider ("new slider"));
        addAndMakeVisible (sld_y.get());
        sld_y->setTooltip ("ypos");
        //    sld_y->setRange (-180, 180, 1);
        //[gc] range suggested by salabianca.cc maxdist and 0.12 is the distance between 2 speakers of the array
        if(ownerFilter->configLoaded) sld_y->setRange (-(maxdist/2), maxdist/2,dsecsrce);
        else sld_y->setRange (-1,1,0.1);
        sld_y->setSliderStyle (Slider::LinearVertical);
        sld_y->setTextBoxStyle (Slider::TextBoxBelow, false, 41, 20);
        sld_y->setColour (Slider::thumbColourId, Colours::grey);
        sld_y->setColour (Slider::textBoxTextColourId, Colours::black);
        sld_y->setColour (Slider::textBoxBackgroundColourId, Colours::white);
        sld_y->addListener (this);
        //    sld_y->setScrollWheelEndless(true); // added manually
        
        sld_x.reset(new Slider ("new slider"));
        addAndMakeVisible (sld_x.get());
        sld_x->setTooltip ("xpos");
        //    sld_x->setRange (-180, 180, 1);
        //[gc] range suggested by salabianca.cc maxdist and 0.12 is the distance between 2 speakers of the array
        if(ownerFilter->configLoaded) sld_x->setRange (-(maxdist/2), maxdist/2,dsecsrce);
        else sld_x->setRange (-1,1,0.1);
        sld_x->setSliderStyle (Slider::LinearHorizontal);
        sld_x->setTextBoxStyle (Slider::TextBoxRight, false, 40, 20);
        sld_x->setColour (Slider::thumbColourId, Colours::grey);
        sld_x->setColour (Slider::textBoxTextColourId, Colours::black);
        sld_x->setColour (Slider::textBoxBackgroundColourId, Colours::white);
        sld_x->addListener (this);
        //    sld_x->setScrollWheelEndless(true); // added manually
        
        
        sld_speed.reset(new Slider ("new slider"));
        addAndMakeVisible (sld_speed.get());
        sld_speed->setTooltip ("movement speed in m/sec");
        sld_speed->setRange (0, 360, 1);
        sld_speed->setSliderStyle (Slider::LinearHorizontal);
        sld_speed->setTextBoxStyle (Slider::TextBoxRight, false, 40, 20);
        sld_speed->setColour (Slider::thumbColourId, Colour (0xffe98273));
        sld_speed->setColour (Slider::textBoxTextColourId, Colours::black);
        sld_speed->setColour (Slider::textBoxBackgroundColourId, Colours::white);
        sld_speed->addListener (this);
        
        sld_y_move.reset(new Slider ("new slider"));
        addAndMakeVisible (sld_y_move.get());
        sld_y_move->setTooltip ("y movement speed");
        sld_y_move->setRange (0, 1, 0.01);
        sld_y_move->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        sld_y_move->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
        sld_y_move->setColour (Slider::trackColourId, Colour (0xff2d7dff));
        sld_y_move->setColour (Slider::rotarySliderFillColourId, Colours::white);
        sld_y_move->setColour (Slider::rotarySliderOutlineColourId, Colours::azure);
        sld_y_move->addListener (this);
        
        sld_x_move.reset(new Slider ("new slider"));
        addAndMakeVisible (sld_x_move.get());
        sld_x_move->setTooltip ("x movement speed");
        sld_x_move->setRange (0, 1, 0.01);
        sld_x_move->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        sld_x_move->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
        sld_x_move->setColour (Slider::trackColourId, Colour (0xff2d7dff));
        sld_x_move->setColour (Slider::rotarySliderFillColourId, Colours::white);
        sld_x_move->setColour (Slider::rotarySliderOutlineColourId, Colours::azure);
        sld_x_move->addListener (this);
        
        txt_x_move.reset(new TextEditor ("new slider"));
        addAndMakeVisible (txt_x_move.get());
        txt_x_move->setMultiLine (false);
        txt_x_move->setReturnKeyStartsNewLine (false);
        txt_x_move->setReadOnly (true);
        txt_x_move->setScrollbarsShown (false);
        txt_x_move->setCaretVisible (false);
        txt_x_move->setPopupMenuEnabled (false);
        txt_x_move->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_x_move->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_x_move->setText ("-180 m/s");
        
        txt_y_move.reset(new TextEditor ("new slider"));
        addAndMakeVisible (txt_y_move.get());
        txt_y_move->setTooltip ("y movement speed");
        txt_y_move->setMultiLine (false);
        txt_y_move->setReturnKeyStartsNewLine (false);
        txt_y_move->setReadOnly (true);
        txt_y_move->setScrollbarsShown (false);
        txt_y_move->setCaretVisible (false);
        txt_y_move->setPopupMenuEnabled (false);
        txt_y_move->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_y_move->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_y_move->setText ("-180 m/s");
    }
    mscale = 11;
    
    if(ownerFilter->configLoaded)
    {
        if(RENDERER_ID==1) msize = maxdist*mscale;
        else msize=130; //sets the size of the window
    }
    else
    {
        msize = 330;
    }
    
    if(RENDERER_ID==1)
    {
        sphere_opengl.reset(new SphereOpenGL(mscale));
        addAndMakeVisible (sphere_opengl.get());
        sphere_opengl->setName ("new OpenGl");
        sphere_opengl->setBounds(10, 30,msize,msize);
        sphere_opengl->processor = ownerFilter;
        //
        sphere_opengl->configLoaded(maxdist);
        sld_x->setDoubleClickReturnValue(true, 0);
        
        sld_y->setDoubleClickReturnValue(true, 0);
        sld_speed->setDoubleClickReturnValue(true, 90);
        sld_x_move->setDoubleClickReturnValue(true, 0.5f);
        sld_y_move->setDoubleClickReturnValue(true, 0.5f);
    }
    char id[10];
    std::sprintf(id, "ID: %d", RENDERER_ID);
    lbl_id.reset(new Label ("new label", TRANS(id)));
    addAndMakeVisible (lbl_id.get());
    lbl_id->setFont (Font (15.00f, Font::plain));
    lbl_id->setJustificationType (Justification::centredRight);
    lbl_id->setEditable (false, false, false);
    lbl_id->setColour (Label::textColourId, Colour (0xff888888));
    lbl_id->setColour (TextEditor::textColourId, Colours::black);
    lbl_id->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    if(RENDERER_ID != 1)
    {
        txt_x.reset(new TextEditor ("new text editor"));
        addAndMakeVisible (txt_x.get());
        txt_x->setMultiLine (false);
        txt_x->setReturnKeyStartsNewLine (false);
        txt_x->setReadOnly (true);
        txt_x->setScrollbarsShown (false);
        txt_x->setCaretVisible (true);
        txt_x->setPopupMenuEnabled (false);
        txt_x->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_x->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_x->setText ("xpos");
        
        txt_y.reset(new TextEditor ("new text editor"));
        addAndMakeVisible (txt_y.get());
        txt_y->setMultiLine (false);
        txt_y->setReturnKeyStartsNewLine (false);
        txt_y->setReadOnly (true);
        txt_y->setScrollbarsShown (false);
        txt_y->setCaretVisible (true);
        txt_y->setPopupMenuEnabled (false);
        txt_y->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_y->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_y->setText ("ypos");
        
        txt_maxspeed.reset(new TextEditor ("new text editor"));
        addAndMakeVisible (txt_maxspeed.get());
        txt_maxspeed->setMultiLine (false);
        txt_maxspeed->setReturnKeyStartsNewLine (false);
        txt_maxspeed->setReadOnly (true);
        txt_maxspeed->setScrollbarsShown (false);
        txt_maxspeed->setCaretVisible (true);
        txt_maxspeed->setPopupMenuEnabled (false);
        txt_maxspeed->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_maxspeed->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_maxspeed->setText ("max speed");
        
        txt_x_move.reset(new TextEditor ("new text editor"));
        addAndMakeVisible (txt_x_move.get());
        txt_x_move->setMultiLine (false);
        txt_x_move->setReturnKeyStartsNewLine (false);
        txt_x_move->setReadOnly (true);
        txt_x_move->setScrollbarsShown (false);
        txt_x_move->setCaretVisible (false);
        txt_x_move->setPopupMenuEnabled (false);
        txt_x_move->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_x_move->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_x_move->setText ("x move");
        
        txt_y_move.reset(new TextEditor ("new text editor"));
        addAndMakeVisible (txt_y_move.get());
        txt_y_move->setMultiLine (false);
        txt_y_move->setReturnKeyStartsNewLine (false);
        txt_y_move->setReadOnly (true);
        txt_y_move->setScrollbarsShown (false);
        txt_y_move->setCaretVisible (false);
        txt_y_move->setPopupMenuEnabled (false);
        txt_y_move->setColour (TextEditor::outlineColourId, Colour (0x706884ff));
        txt_y_move->setColour (TextEditor::shadowColourId, Colour (0x0));
        txt_y_move->setText ("y move");
        
    }
    
    //[UserPreSize]
    //[/UserPreSize]
    
    //TOTAL SIZE WINDOW
    if(RENDERER_ID!=1)
    {
        setSize (msize+220, msize+90);
    }
    else
    {
        setSize (msize+220, msize+90);
    }
    // register as change listener (gui/dsp sync)
    ownerFilter->addChangeListener(this);
    ownerFilter->sendChangeMessage(); // get status from dsp
    
    //    String str_id = "ID: ";
    //    str_id << ownerFilter->m_id;
    //    lbl_id->setText(str_id, dontSendNotification);
    lbl_id->setText(id, dontSendNotification);
}

WFSRendererAudioProcessorEditor::~WFSRendererAudioProcessorEditor()
{
    WFSRendererAudioProcessor* ourProcessor = getProcessor();
    
    // remove me as listener for changes
    ourProcessor->removeChangeListener(this);
    
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]
    
    sld_y = nullptr;
    sld_x = nullptr;
    
    btn_open = nullptr;
    btn_preset_folder = nullptr;
    sld_speed = nullptr;
    sld_y_move = nullptr;
    sld_x_move = nullptr;
    txt_x_move = nullptr;
    txt_y_move = nullptr;
    
    if(RENDERER_ID!=1){
        txt_x = nullptr;
        txt_y = nullptr;
        txt_maxspeed = nullptr;
    }
    
    lbl_id = nullptr;\
    sphere_opengl = nullptr;
    
    
    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void WFSRendererAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]
    
    g.fillAll (Colour(0xffb7c6a0));
    g.setGradientFill (ColourGradient (Colour (0xff4e4e4e),
                                       (float) (proportionOfWidth (0.1143f)), (float) (proportionOfHeight (0.0800f)),
                                       Colour(0x99cc66),
                                       (float) (proportionOfWidth (0.6314f)), (float) (proportionOfHeight (0.5842f)),
                                       true));
    g.fillRect (0, 0, msize+160, msize+110);
    g.setColour (Colours::black);
    g.drawRect (0, 0, msize+220, msize+90, 1);
    // g.setColour (Colour (0xff6478c8));
    //g.fillRoundedRectangle (165.0f, 310.0f, 154.0f, 77.0f, 4.0000f);
    
    g.setColour (Colours::white);
    g.setFont (Font (17.2000f, Font::bold));
    g.drawText ("WFSRENDERER",
                -6, 2, 343, 30,
                Justification::centred, true);
    if(RENDERER_ID==1){
        
        g.setColour (Colours::white);
        g.setFont (Font (14.0000f, Font::plain));
        g.drawText ("ypos",
                    msize+60, msize+35, 48, 16,
                    Justification::centred, true);
        
        
        g.drawText ("xpos",
                    msize+60, msize+60, 48, 16,
                    Justification::centred, true);
        
        g.drawText ("max speed",
                    msize+70, msize-65, 87, 16,
                    Justification::centredLeft, true);
        
        
        // g.setColour (Colour (0xff6478c8));
        //g.fillRoundedRectangle (24.0f, 310.0f, 226.0f, 43.0f, 4.0000f);
        
        
         g.drawText ("x move",
         msize+70, msize-190, 78, 22,
         Justification::centredLeft, true);
        
        g.drawText ("y move",
        msize+70, msize-130, 78, 22,
        Justification::centredLeft, true);
        
        
        //[UserPaint] Add your own custom painting code here..
        //[/UserPaint]
    }else{
        g.setColour (Colours::white);
        g.setFont (Font (14.0000f, Font::plain));
        
        
        g.drawText ("xpos",
                    110, 45, 48, 16,
                    Justification::centredLeft, true);
        
        g.drawText ("ypos",
                    110, 75, 48, 16,
                    Justification::centredLeft, true);
        
        
        
        
        g.drawText ("x move",
                    110, 105, 78, 22,
                    Justification::centredLeft, true);
        
        g.drawText ("y move",
                    110, 135, 78, 22,
                    Justification::centredLeft, true);
        
        g.drawText ("max speed",
                    110, 165, 87, 16,
                    Justification::centredLeft, true);
        
    }
}

void WFSRendererAudioProcessorEditor::resized()
{
    btn_open->setBounds (msize+80, 64, 56, 24);//(500, 64, 56, 24);
    btn_preset_folder->setBounds (msize+80, 96, 94, 24);//(500, 96, 94, 24);
    lbl_id->setBounds (msize+131, 0, 57, 24);
    if(RENDERER_ID==1){
        sld_y->setBounds (msize+17, 25, 40, msize+30);
        sld_x->setBounds (5, msize+50, msize+52, 40);
        //sld_size->setBounds (32, 255, 29, 29);
        sld_speed->setBounds (msize+59, msize-50, 140, 40);
        sld_x_move->setBounds (msize+70, msize-170, 29, 29);
        sld_y_move->setBounds (msize+70,msize-110, 29, 29);
        txt_x_move->setBounds (msize+125, msize-165, 78, 22);
        txt_y_move->setBounds (msize+125, msize-105, 78, 22);
    
        
    }else{
        txt_x->setBounds (20, 45, 78, 22);
        txt_y->setBounds (20, 75, 78, 22);
        txt_x_move->setBounds (20, 105, 78, 22);
        txt_y_move->setBounds (20, 135, 78, 22);
        txt_maxspeed->setBounds (20, 165, 78, 22);
    }

}




void WFSRendererAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    WFSRendererAudioProcessor* ourProcessor = getProcessor();
    
    if(ourProcessor->configLoaded)
    {
        if (sliderThatWasMoved == sld_y.get())
        {
            //        ourProcessor->setParameterNotifyingHost(WFSRendererAudioProcessor::yParam, ((float)sld_y->getValue() + 180) / 360.f);
            //        [gc]
//            (WFSRendererAudioProcessor::yParam)- 0.5f) * maxdist
            ourProcessor->setParameterNotifyingHost(1, ((float)sld_y->getValue() + hfdist) * invdist);
         
        }
        else if (sliderThatWasMoved == sld_x.get())
        {
            //
            //        ourProcessor->setParameterNotifyingHost(0, ((float)sld_x->getValue() + 180) / 360.f);
            //        [gc]
            ourProcessor->setParameterNotifyingHost(0, 1-(((float)sld_x->getValue() + hfdist) * invdist));
        }
//        else if (sliderThatWasMoved == sld_size)
//        {
//            ourProcessor->setParameterNotifyingHost(WFSRendererAudioProcessor::SizeParam, (float)sld_size->getValue());
//        }
        
        else if (sliderThatWasMoved == sld_x_move.get())
        {
            ourProcessor->setParameterNotifyingHost(2, (float)sld_x_move->getValue());
            //std::cout<< "xp: "<< sld_x_move->getValue()<<std::endl;
        }
        else if (sliderThatWasMoved == sld_y_move.get())
        {
            ourProcessor->setParameterNotifyingHost(3, (float)sld_y_move->getValue());
        }
        else if (sliderThatWasMoved == sld_speed.get())
        {
            ourProcessor->setParameterNotifyingHost(4, (float)sld_speed->getValue()/360.f);
        }
    }
}


void WFSRendererAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster *source)
{
    WFSRendererAudioProcessor* ourProcessor = getProcessor();
    String x_mv;
    
    float x_mv_param = ourProcessor->getParameter(2);
    float speed_param = ourProcessor->getParameter(4);
    
    
    if (x_mv_param <= 0.48f)
    {
        x_mv << "-" << String((int)(pow(speed_param*360.f, (0.45f - x_mv_param)*2.22222f)+0.5f)).substring(0, 5) << " m/s"; // from 0->90m/sec
    } else if (x_mv_param >= 0.52f) {
        x_mv << String((int)(pow(speed_param*360.f, (x_mv_param - 0.55f)*2.22222f) + 0.5f)).substring(0, 5) << " m/s";
    } else {
        x_mv << "0 m/s";
    }
    
    
    
    String y_mv;
    
    float y_mv_param = ourProcessor->getParameter(3);
    
    if (y_mv_param <= 0.48f)
    {
        y_mv << "-" << String((int)(pow(speed_param*360.f, (0.45f - y_mv_param)*2.22222f) + 0.5f)).substring(0, 4) << " m/s"; // from 0->90m/sec
    } else if (y_mv_param >= 0.52f) {
        y_mv << String((int)(pow(speed_param*360.f, (y_mv_param - 0.55f)*2.22222f) + 0.5f)).substring(0, 4) << " m/s";
    } else {
        y_mv << "0 m/s";
    }
    
    txt_x_move->setText(x_mv);
    txt_y_move->setText(y_mv);
    
    
    if(RENDERER_ID==1){
        if(ourProcessor->configLoaded){
            //[gc]
            //    sld_x->setValue((ourProcessor->getParameter(WFSRendererAudioProcessor::xParam) - 0.5f) * 360.f, dontSendNotification);
            //    sld_y->setValue((ourProcessor->getParameter(WFSRendererAudioProcessor::yParam) - 0.5f) * 360.f, dontSendNotification);
            sld_x->setValue(((1-(ourProcessor->getParameter(0)))- 0.5f) * maxdist, dontSendNotification);
            sld_y->setValue((ourProcessor->getParameter(1)- 0.5f) * maxdist, dontSendNotification);

            sphere_opengl->setSource(((1-(ourProcessor->getParameter(0)))- 0.5f) * maxdist, (ourProcessor->getParameter(1) - 0.5f) * maxdist);
            
            
            sld_speed->setValue(ourProcessor->getParameter(4)*360, dontSendNotification);
            
            
            sld_x_move->setValue(x_mv_param, dontSendNotification);
            
            
            sld_y_move->setValue(y_mv_param, dontSendNotification);
            
            
        }
    }else{
        if(ourProcessor->configLoaded){
        String x,y,max;
        x<<(((1-(ourProcessor->getParameter(0)))- 0.5f) * maxdist);
        y<<(((ourProcessor->getParameter(1))- 0.5f) * maxdist);
        max<<(ourProcessor->getParameter(4)*360);
        
        txt_maxspeed->setText(max);
        
        txt_x->setText(x);
        txt_y->setText(y);
        }
    }
}


void WFSRendererAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
{
    WFSRendererAudioProcessor* ourProcessor = getProcessor();
    
    if (buttonThatWasClicked == btn_open.get())
    {
        UpdatePresets();
        popup_presets.showMenuAsync(PopupMenu::Options().withTargetComponent (btn_open.get()), ModalCallbackFunction::forComponent (menuItemChosenCallback, this));
    }
    else if (buttonThatWasClicked == btn_preset_folder.get())
    {
        FileChooser myChooser ("Please select the new preset folder...",
                               ourProcessor->presetDir,
                               "*.config");
        
        if (myChooser.browseForDirectory())
        {
            
            File mooseFile (myChooser.getResult());
            ourProcessor->presetDir = mooseFile;
            
            ourProcessor->SearchPresets(mooseFile);
            
            ourProcessor->lastDir = mooseFile.getParentDirectory();
            UpdatePresets();
        }
    }
    
#if BINAURAL_DECODER
    if (buttonThatWasClicked == tgl_load_irs)
    {
        ourProcessor->_load_ir = tgl_load_irs->getToggleState();
    }
#endif
    
}

void WFSRendererAudioProcessorEditor::UpdatePresets()
{
    WFSRendererAudioProcessor* ourProcessor = getProcessor();
    
    popup_submenu.clear(); // contains submenus
    
    popup_presets.clear(); // main menu
    
    String lastSubdir;
    StringArray Subdirs; // hold name of subdirs
    
    int j = 1;
    
    for (int i=0; i < ourProcessor->_presetFiles.size(); i++) {
        
        // add separator for new subfolder
        String subdir = ourProcessor->_presetFiles.getUnchecked(i).getParentDirectory().getFileName();
        
        if (!lastSubdir.equalsIgnoreCase(subdir)) {
            
            popup_submenu.add(new PopupMenu());
            Subdirs.add(subdir);
            
            j++;
            lastSubdir = subdir;
        }
        
        // add item to submenu
        // check if this preset is loaded
        
        if (ourProcessor->_configFile == ourProcessor->_presetFiles.getUnchecked(i))
        {
            popup_submenu.getLast()->addItem(i+1, ourProcessor->_presetFiles.getUnchecked(i).getFileNameWithoutExtension(), true, true);
        } else {
            popup_submenu.getLast()->addItem(i+1, ourProcessor->_presetFiles.getUnchecked(i).getFileNameWithoutExtension());
        }
        
        
    }
    
    // add all subdirs to main menu
    for (int i=0; i < popup_submenu.size(); i++) {
        if (Subdirs.getReference(i) == ourProcessor->_configFile.getParentDirectory().getFileName())
        {
            popup_presets.addSubMenu(Subdirs.getReference(i), *popup_submenu.getUnchecked(i), true, nullptr, true);
        } else {
            popup_presets.addSubMenu(Subdirs.getReference(i), *popup_submenu.getUnchecked(i));
        }
        
    }
    
    popup_presets.addItem(-1, String("open from file..."));
    
}


void WFSRendererAudioProcessorEditor::menuItemChosenCallback (int result, WFSRendererAudioProcessorEditor* demoComponent)
{
    // std::cout << "result: " << result << std::endl;
    
    WFSRendererAudioProcessor* ourProcessor = demoComponent->getProcessor();
    
    
    // file chooser....
    if (result == 0)
    {
        // do nothing
    }
    else if (result == -1)
    {
        FileChooser myChooser ("Please select the preset file to load...",
                               ourProcessor->lastDir,
                               "*.xml");
        if (myChooser.browseForFileToOpen())
        {
            
            File mooseFile (myChooser.getResult());
            //ourProcessor->ScheduleConfiguration(mooseFile);
            ourProcessor->LoadConfiguration(mooseFile);
            demoComponent->loadedLayout();
            ourProcessor->lastDir = mooseFile.getParentDirectory();
            demoComponent->maxdist = ourProcessor->layout->maxdist();
            demoComponent->dsecsrce = ourProcessor->layout->dsecsrce();
            
        }
    }
    else // load preset
    {
        //[gc]
        //        demoComponent->stopTimer();
        ourProcessor->LoadPreset(result - 1);
        demoComponent->loadedLayout();
        demoComponent->maxdist = ourProcessor->layout->maxdist();
        demoComponent->dsecsrce = ourProcessor->layout->dsecsrce();
        // popup_presets
    }
    
}


void WFSRendererAudioProcessorEditor::loadedLayout(){
    maxdist = getProcessor()->layout->maxdist();
    
    invdist = 1.0f/maxdist;
    hfdist = maxdist/2.0f;
    dsecsrce = getProcessor()->layout->dsecsrce();
    
    msize =  maxdist*mscale;
    if(RENDERER_ID==1){
        sld_y->setRange (-( maxdist/2), maxdist/2, dsecsrce);
        sld_x->setRange (-( maxdist/2), maxdist/2, dsecsrce);
        sphere_opengl->setBounds(10, 30,msize,msize);
    }
    sphere_opengl->configLoaded(maxdist);
    UpdateText();
    UpdatePresets();
}


void WFSRendererAudioProcessorEditor::UpdateText()
{
    //    Ambix_binauralAudioProcessor* ourProcessor = getProcessor();
    
}

