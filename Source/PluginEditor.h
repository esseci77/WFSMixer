/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED


//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SphereOpenGL.h"
#include "WfsMixerStripComponent.h"

//[/Headers]

class LCDComponent;
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
/**
 //[Comments]
 An auto-generated component, created by the Jucer.
 
 Describe your class and how it works here!
 //[/Comments]
 */
class WFSRendererAudioProcessorEditor  : public AudioProcessorEditor,
#if RENDERER_ID == 1
                                         public Slider::Listener,
#endif
                                         public ChangeListener,
                                         public Button::Listener,
                                         public Timer
{
public:
    //==========================================================================
    WFSRendererAudioProcessorEditor (WFSRendererAudioProcessor* ownerFilter,
                                     teragon::ResourceCache* resourceCache);
    ~WFSRendererAudioProcessorEditor();
    
    //==========================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    double speedToValue(const double s) const;
    double valueToSpeed(const double v) const;
    //[/UserMethods]
    
    void paint (Graphics& g) override;
    void resized() override;
#if RENDERER_ID == 1
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
#endif
    void buttonClicked (Button* buttonThatWasClicked) override;
    void changeListenerCallback (ChangeBroadcaster *source) override;
    void timerCallback() override;
    static void menuItemChosenCallback (int result,
                                        WFSRendererAudioProcessorEditor* demoComponent);
    
    /**
     * @brief Called every time the user interface has to be updated.
     * @param configFilePath The configuration file path. If this string is not empty, the interface
     * will be updated loading the new parameters.
     */
    void updateUI(const juce::String& configFilePath = juce::String());
    bool showOSCDialog();
    void toggleOscStatusLabel(const bool status);
    
    void loadLayout();
    void saveLayout();
    
private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]
    
    //==========================================================================
    
    std::unique_ptr<juce::Label>      m_idLabel;
    std::unique_ptr<juce::Label>      m_title;
    std::unique_ptr<juce::Label>      m_versionLabel;
    std::unique_ptr<juce::TextButton> m_openButton;
    
    std::unique_ptr<juce::TextButton>        m_activateOscButton;
    std::unique_ptr<teragon::IndicatorLight> m_oscLight;
    std::unique_ptr<LCDComponent>            m_lcdComponent;
    
    std::unique_ptr<juce::FileChooser>       m_fileChooser;
    
#if RENDERER_ID == 1
    std::unique_ptr<WfsSourcesCanvas> m_wfsCanvas;
    std::unique_ptr<juce::Slider>     m_vertSlider;
    std::unique_ptr<juce::Slider>     m_horzSlider;
    std::unique_ptr<juce::Viewport>   m_mixerViewport;
#else
    std::unique_ptr<juce::TextButton>     m_outputButton;
#endif
            
    teragon::ResourceCache* resourceCache = nullptr;
        
    WFSRendererAudioProcessor* getProcessor() const
    {
        return static_cast <WFSRendererAudioProcessor*> (getAudioProcessor());
    }
    
    TooltipWindow tooltipWindow;
    
    //===========================+==============================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WFSRendererAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]



#endif  // PLUGINEDITOR_H_INCLUDED
