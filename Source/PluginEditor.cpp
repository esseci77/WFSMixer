
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
#include "WfsLookAndFeel.h"
#include "ui/TeragonGuiComponents.h"
#include "ui/LCDComponent.h"
#include "PluginEditor.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

WFSRendererAudioProcessorEditor::WFSRendererAudioProcessorEditor (WFSRendererAudioProcessor* ownerFilter,
                                                                  teragon::ResourceCache* resourceCache)
  : juce::AudioProcessorEditor (ownerFilter)
{
    
    float maxdist  = 2.0f; // [gc] maximum distance considering the room and the
                           //     space around it, according to xml layout config.
    float dsecsrce = 0.1f;
    
    if(ownerFilter->isConfigLoaded())
    {
		maxdist = getProcessor()->layout()->maxdist();
        dsecsrce = getProcessor()->layout()->dsecsrce();
    }
    auto laf = WfsLookAndFeel::getInstance();
    setLookAndFeel(laf);
    
    m_title.reset(new Label("title",
                            RENDERER_ID == 1 ? "WFSMixer" : "WFSrenderer"));
    m_title->setColour(Label::ColourIds::textColourId, juce::Colours::white);
    m_title->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(m_title.get());

    juce::String str("ID: ");
    str << ownerFilter->params().rendererId();
    m_idLabel.reset(new juce::Label ("idLabel", str));
    addAndMakeVisible (m_idLabel.get());
    m_idLabel->setJustificationType (Justification::centredRight);
    m_idLabel->setEditable (false, false, false);
    m_idLabel->setColour (Label::textColourId, Colour (0xff888888));
    m_idLabel->setColour (TextEditor::textColourId, Colours::black);
    m_idLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    m_versionLabel.reset(new juce::Label("versionLabel", ProjectInfo::versionString));
    addAndMakeVisible(m_versionLabel.get());
    m_versionLabel->setJustificationType (Justification::centredRight);
    m_versionLabel->setEditable (false, false, false);
    m_versionLabel->setColour (Label::textColourId, Colour (0xff888888));
    m_versionLabel->setColour (TextEditor::textColourId, Colours::black);
    m_versionLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
#if RENDERER_ID == 1
    const int nSources = ownerFilter->getTotalNumInputChannels();
    
    m_wfsCanvas.reset(new WfsSourcesCanvas(nSources, ownerFilter));
    addAndMakeVisible (m_wfsCanvas.get());
    
    m_horzSlider.reset(new Slider ("xposSlider"));
    addAndMakeVisible (m_horzSlider.get());
    m_horzSlider->setTooltip ("xpos");

    //[gc] range suggested by salabianca.cc maxdist and 0.12 is the distance
    //     between 2 speakers of the array
    m_horzSlider->setRange (-maxdist/2, maxdist/2, dsecsrce);
    m_horzSlider->setSliderStyle (Slider::LinearHorizontal);
    m_horzSlider->setTextBoxStyle (Slider::TextBoxRight, false, 40, 20);
    m_horzSlider->setColour (Slider::thumbColourId, Colours::grey);
    m_horzSlider->setColour (Slider::textBoxTextColourId, Colours::black);
    m_horzSlider->setColour (Slider::textBoxBackgroundColourId, Colours::white);
    m_horzSlider->addListener (this);

    m_vertSlider.reset(new Slider ("yposSlider"));
    addAndMakeVisible (m_vertSlider.get());
    m_vertSlider->setTooltip ("ypos");

    //[gc] range suggested by salabianca.cc maxdist and 0.12 is the distance
    //      between 2 speakers of the array
    m_vertSlider->setRange(-maxdist/2, maxdist/2, dsecsrce);
    m_vertSlider->setSliderStyle  (Slider::LinearVertical);
    m_vertSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 41, 20);
    m_vertSlider->setColour (Slider::thumbColourId, Colours::grey);
    m_vertSlider->setColour (Slider::textBoxTextColourId, Colours::black);
    m_vertSlider->setColour (Slider::textBoxBackgroundColourId, Colours::white);
    m_vertSlider->setDoubleClickReturnValue(true, 0);
    m_vertSlider->addListener (this);
            
    m_mixerViewport.reset(new juce::Viewport("MixerViewport"));
    addAndMakeVisible(m_mixerViewport.get());
    
#else
    m_outputButton.reset(new juce::TextButton("togglSpeakers"));
    m_outputButton->setButtonText("speakers");
    m_outputButton->setTooltip("Toggle wfs speakers");
    m_outputButton->addListener(this);
    addAndMakeVisible(m_outputButton.get());
#endif
    
    tooltipWindow.setMillisecondsBeforeTipAppears (700); // tooltip delay
    m_openButton.reset(new juce::TextButton ("openButton"));
    m_openButton->setTooltip ("browse presets or open from file");
    m_openButton->setButtonText ("layout");
    m_openButton->addListener (this);
    addAndMakeVisible (m_openButton.get());
    
    m_activateOscButton.reset(new juce::TextButton("osc"));
    addAndMakeVisible(m_activateOscButton.get());
    m_activateOscButton->addListener(this);
    
    m_oscLight.reset(new teragon::IndicatorLight(resourceCache));
    addAndMakeVisible(m_oscLight.get());

    m_lcdComponent.reset(new LCDComponent(2));
    addAndMakeVisible(m_lcdComponent.get());
    
    // initializations
    const int outs = ownerFilter->getTotalNumOutputChannels();
    const int  rid = ownerFilter->params().rendererId();
    
#if RENDERER_ID == 1
    m_mixerViewport->setViewedComponent(new WfsMixerComponent(ownerFilter,
                                                              resourceCache));
    if (outs > 64)
    {
        m_activateOscButton->setEnabled(false);
    }
#else
    m_openButton->setEnabled(false); // no load layout for slaves!
#endif

    str = "wfs speakers: ";
    str << (rid - 1) * outs + 1;
    str << " - " << rid * outs;
    m_lcdComponent->setText(1, str, true);
    
    if (getProcessor()->layout())
    {
        str = "Loaded layout: ";
        str << getProcessor()->layout()->configFilePath();
    }
    else
    {
        str = "** layout not loaded **";
    }
    m_lcdComponent->setText(0, str);
    toggleOscStatusLabel(ownerFilter->isOSCActive());
    m_oscLight->setLightOn(ownerFilter->isOSCActive());
    
    ownerFilter->addChangeListener(this);
    ownerFilter->sendChangeMessage(); // get status from dsp
    
    setResizable(true, true);
    setSize(kWindowWidth, kWindowHeight);
}

WFSRendererAudioProcessorEditor::~WFSRendererAudioProcessorEditor()
{
    // remove me as listener for changes
    getProcessor()->removeChangeListener(this);
    setLookAndFeel(nullptr);
    WfsLookAndFeel::deleteInstance();
}

void WFSRendererAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (juce::Colour(0xff26272a));
}

void WFSRendererAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce(10, 0);
    
#if RENDERER_ID == 1
    const int canvasSide = area.getHeight() - 30 - 90;
    
    auto hstrip = area.removeFromTop(30);
    hstrip.removeFromLeft(canvasSide + 40);
    m_idLabel->setBounds(hstrip);

    hstrip = area.removeFromBottom(60);
    auto titleBox = hstrip.removeFromLeft(canvasSide + 40);
    m_title->setBounds(titleBox.removeFromLeft(canvasSide));
    m_versionLabel->setBounds(titleBox.removeFromBottom(20));
    
    hstrip.reduce(10, 4);
    auto buttonsPanel = hstrip.removeFromLeft(120);
    m_openButton->setBounds(buttonsPanel.removeFromTop(26).reduced(3, 3));
    m_oscLight->setBounds(buttonsPanel.removeFromRight(26).reduced(5, 5));
    m_activateOscButton->setBounds(buttonsPanel.reduced(3, 3));
    
    hstrip.removeFromLeft(10);
    m_lcdComponent->setBounds(hstrip);
    
    auto sidePanel = area.removeFromLeft(canvasSide + 40);
    m_horzSlider->setBounds(sidePanel.removeFromBottom(30));
    m_vertSlider->setBounds(sidePanel.removeFromRight(40));
    m_wfsCanvas->setBounds(sidePanel);
        
    m_mixerViewport->setBounds(area);
    
#else
    m_idLabel->setBounds(area.removeFromTop(30));
    
    auto bottomPanel = area.removeFromBottom(50);
    m_title->setBounds(bottomPanel.removeFromLeft(area.getWidth() - 120));
    m_outputButton->setBounds(bottomPanel.removeFromTop(20).reduced(3,3));
    
    auto buttonsPanel = area.removeFromLeft(120);
    m_openButton->setBounds(buttonsPanel.removeFromTop(26).reduced(3, 3));
    buttonsPanel = buttonsPanel.removeFromTop(26);
    m_oscLight->setBounds(buttonsPanel.removeFromRight(26).reduced(5, 5));
    m_activateOscButton->setBounds(buttonsPanel.reduced(3, 3));
    
    m_lcdComponent->setBounds(area.removeFromTop(52).reduced(3, 3));
    
#endif
}

#if RENDERER_ID == 1

void WFSRendererAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    WFSRendererAudioProcessor* ourProcessor = getProcessor();
    
    if(ourProcessor->isConfigLoaded())
    {
        if (sliderThatWasMoved == m_vertSlider.get())
        {
            auto maxdist = getProcessor()->layout()->maxdist();
            
            if (!m_wfsCanvas->isLayoutRotated())
            {
                // normalize to percentage
                auto xpos = 100.0 * (0.5 + (float)m_vertSlider->getValue() / maxdist);
                *ourProcessor->params()[0].xpos = (float)xpos;
            }
            else
            {
                // normalize to percentage
                auto ypos = 100.0 * (0.5 + (float)m_vertSlider->getValue() / maxdist);
                *ourProcessor->params()[0].ypos = (float)ypos;
            }
        }
        else if (sliderThatWasMoved == m_horzSlider.get())
        {
            auto maxdist = getProcessor()->layout()->maxdist();
            
            if (!m_wfsCanvas->isLayoutRotated())
            {
                auto ypos = 100.0 * (0.5 - (float)m_horzSlider->getValue() / maxdist);
                *ourProcessor->params()[0].ypos = (float)ypos;
            }
            else
            {
                auto xpos = 100.0 * (0.5 + (float)m_horzSlider->getValue() / maxdist);
                *ourProcessor->params()[0].xpos = (float)xpos;
            }
        }
    }
}
#endif

void WFSRendererAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == m_openButton.get())
    {
        /*
        WFSRendererAudioProcessor* p = getProcessor();
        
        juce::FileChooser fc ("Please select the preset file to load...",
                              p->params().lastDirectory(),
                              "*.xml");
        if (fc.browseForFileToOpen())
        {
            juce::File f (fc.getResult());

            p->LoadConfiguration(f);
            p->params().setLastDirectory(f.getParentDirectory());
        }*/
        
        m_fileChooser =
            std::make_unique<juce::FileChooser>("Please select the preset file to load...",
                                                getProcessor()->params().lastDirectory(),
                                                "*.xml");
        auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
         
        m_fileChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)
        {
            juce::File theFile (chooser.getResult());
            
            if (theFile.getFullPathName().isNotEmpty())
            {
                WFSRendererAudioProcessor* p = getProcessor();
                p->LoadConfiguration(theFile);
                p->params().setLastDirectory(theFile.getParentDirectory());

            }
        });

    }
    
    else if (buttonThatWasClicked == m_activateOscButton.get())
    {
        WFSRendererAudioProcessor* p = getProcessor();
                
        if (! p->isOSCActive())
        {
            if (showOSCDialog())
            {
                if (p->startOSC())
                {
                    toggleOscStatusLabel(true);
                    m_oscLight->setLightOn(true);
                }
                else
                {
                    juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon,
                                                      "WFSRenderer",
                                                      "OSC connection error.");
                }
            }
            repaint();
        }
        else
        {
            p->stopOSC();
            toggleOscStatusLabel(false);
            m_oscLight->setLightOn(false);
        }
    }
    
#if RENDERER_ID > 1
    else if (buttonThatWasClicked == m_outputButton.get())
    {
        WFSRendererAudioProcessor* p = getProcessor();
        
        if(p->isConfigLoaded())
        {
            DBG("You shouldn't be there...");
            return;
        }
        const int outs = p->getTotalNumOutputChannels();
        int rid = p->params().rendererId() + 1;
        
        if (rid * outs > 192)
        {
            rid = 2;
        }
        p->params().setRendererId(rid);
        
        juce::String str = "wfs speakers: ";
        str << (rid - 1) * outs + 1;
        str << " - " << rid * outs;
        m_lcdComponent->setText(1, str, true);
        
        str = "ID: ";
        str << rid;
        m_idLabel->setText(str, dontSendNotification);
    }
#endif
    
}

void WFSRendererAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster *source)
{
    auto* wcb = dynamic_cast<WfsChangeBroadcaster*>(source);
    
    jassert(wcb);
    auto& msg = wcb->getChangeBroadcasterMessage();
    
    if (msg.messageID == WfsChangeBroadcaster::Message::Type::kLoadConfiguration)
    {
        updateUI(msg.contents);
    }
    else
    {
        updateUI();
    }
}

void WFSRendererAudioProcessorEditor::timerCallback()
{
    updateUI();
}

void WFSRendererAudioProcessorEditor::updateUI(const juce::String& configFilePath)
{
    // called when the UI should be updated (i.e. OSC message)
    
    if (configFilePath.isNotEmpty())
    {
        juce::String s = "Loaded layout: ";
        s << configFilePath;
        m_lcdComponent->setText(0, s);

#if RENDERER_ID == 1
        const float maxdist = getProcessor()->layout()->maxdist();
        const float dsecsrce = getProcessor()->layout()->dsecsrce();

        m_vertSlider->setRange (-maxdist/2, maxdist/2, dsecsrce);
        m_horzSlider->setRange (-maxdist/2, maxdist/2, dsecsrce);

        if (m_wfsCanvas)
        {
            m_wfsCanvas->repaint();
        }
    }
    else
    {
        WFSRendererAudioProcessor* ourProcessor = getProcessor();
        WFSRendererParameters& params = ourProcessor->params();
        
        if (ourProcessor->layout() == nullptr)
        {
            std::cout << "Layout not loaded.\n";
            return;
        }
        float xpos= 0.01f * params[0].x; // convert to 0 - 1 values from
        float ypos= 0.01f * params[0].y; // percentages.
        auto maxdist = getProcessor()->layout()->maxdist();

        if(ourProcessor->isConfigLoaded())
        {
            float h;
            float v;
            
            if (!m_wfsCanvas->isLayoutRotated())
            {
                h = (0.5f - ypos) * maxdist;
                v = (xpos - 0.5f) * maxdist;
            }
            else
            {
                h = (xpos - 0.5f) * maxdist;
                v = (ypos - 0.5f) * maxdist;
            }
            
            m_horzSlider->setValue(h, dontSendNotification);
            m_vertSlider->setValue(v, dontSendNotification);
        }
        
        for (int i = 0; i < params.numberOfSources(); ++i)
        {
            const float xpos_i = 0.01f * params[i].x; // convert to 0 - 1 values from
            const float ypos_i = 0.01f * params[i].y; // percentages.
            auto maxdist_i = ourProcessor->layout()->maxdist();

            if(ourProcessor->isConfigLoaded())
            {
                m_wfsCanvas->setSourcePosition(i, (xpos_i - 0.5f) * maxdist_i,
                                                  (ypos_i - 0.5f) * maxdist_i);
            }
        }
        auto mixer = dynamic_cast<WfsMixerComponent*>(m_mixerViewport->getViewedComponent());
        mixer->update();
#else
        m_outputButton->setEnabled(false);
#endif
    }
    repaint();
}


#if 0  // Not used anymore...
double WFSRendererAudioProcessorEditor::speedToValue(const double s) const
{
    /*
     * let smax = speed * 360
     * exponential converter (-smax...0...smax) m/s -> (0..0.5..1)
     */
    const double speed = 360.0 * getProcessor()->params()[0].speed->get();
    const double  vm05 = 0.5 * std::log(std::abs(s)) / std::log(speed);
    const double   out = std::copysign(1.0, s) * vm05 * 0.5;
    return out;
}

double WFSRendererAudioProcessorEditor::valueToSpeed(const double v) const
{
    /*
     * let smax = speed * 360
     * exponential converter (0..0.5..1) -> (-smax...0...smax) m/s
     */
    const double speed = 360.0 * getProcessor()->params()[0].speed->get();
    const double  vm05 = v - 0.5;
    const double   out = std::copysign(1.0, vm05) * std::pow(speed,
                                                             std::abs(vm05) / 0.5);
    return std::round(out);
}
#endif


bool WFSRendererAudioProcessorEditor::showOSCDialog()
{
    juce::String msg;
    juce::String def;
    auto& params = getProcessor()->params();
    
    msg = "Enter multicast and local port UDP connection";
    
    auto addr = params.oscAddress();
    auto port = params.oscPort();
    def << addr << ":" << port;
    
    juce::AlertWindow aw("Activate OSC communication", msg,
                         juce::AlertWindow::InfoIcon);
    
    aw.addTextEditor("addrPort", def);
    aw.addButton("Ok",     0);
    aw.addButton("Cancel", 1);
    
    if (aw.runModalLoop() == 1)
    {
        return false;
    }
    
    auto addrPort = aw.getTextEditor("addrPort")->getText();
        
    //TODO check if value are consistent!!!
    
    juce::StringArray tokens;
    tokens.addTokens(addrPort, ":", "\"'");
    
    params.setOscAddress(tokens[0]);
    params.setOscPort(tokens[1]);
        
    return true;
}

void WFSRendererAudioProcessorEditor::toggleOscStatusLabel(const bool status)
{
    juce::String msg;
    
    if (status)
    {
        auto& params = getProcessor()->params();
        auto addr = params.oscAddress();
        auto port = params.oscPort();
        msg << "OSC: " << addr << ":" << port;
    }
    else
    {
        msg << "OSC: inactive";
    }
    m_lcdComponent->setText(1, msg);
}

void WFSRendererAudioProcessorEditor::loadLayout()
{
    m_fileChooser =
        std::make_unique<juce::FileChooser>("Please choose a Wfs sources layout file:",
                                            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                            "*.wfslayout");
    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
     
    m_fileChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)
    {
        juce::File theFile (chooser.getResult());
        
        if (theFile.getFullPathName().isNotEmpty())
        {
            getProcessor()->params().load(theFile);
        }
    });
}

void WFSRendererAudioProcessorEditor::saveLayout()
{
    m_fileChooser =
        std::make_unique<juce::FileChooser>("Please choose a Wfs sources layout file:",
                                            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                            "*.wfslayout");
    auto folderChooserFlags =   FileBrowserComponent::saveMode
                              | FileBrowserComponent::canSelectFiles
                              | FileBrowserComponent::warnAboutOverwriting;
     
    m_fileChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)
    {
        juce::File theFile (chooser.getResult());
        
        if (theFile.getFullPathName().isNotEmpty())
        {
            getProcessor()->params().save(theFile);
        }
    });
}
