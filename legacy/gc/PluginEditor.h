/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED


//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "SphereOpenGL.h"

//[/Headers]



//==============================================================================
/**
 //[Comments]
 An auto-generated component, created by the Jucer.
 
 Describe your class and how it works here!
 //[/Comments]
 */
class WFSRendererAudioProcessorEditor  : public AudioProcessorEditor,
public Slider::Listener,
public ChangeListener,
public Button::Listener
{
public:
    //==============================================================================
    WFSRendererAudioProcessorEditor (WFSRendererAudioProcessor* ownerFilter);
    ~WFSRendererAudioProcessorEditor();
    
    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]
    
    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);
    void changeListenerCallback (ChangeBroadcaster *source);
    static void menuItemChosenCallback (int result, WFSRendererAudioProcessorEditor* demoComponent);
    void UpdatePresets();
    void UpdateText();
    void loadedLayout();
private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]
    
    //==============================================================================
    std::unique_ptr<Slider> sld_y;
    std::unique_ptr<Slider> sld_x;
    

    std::unique_ptr<TextButton> btn_open;
    PopupMenu popup_presets;
    OwnedArray<PopupMenu> popup_submenu;
    std::unique_ptr<TextButton> btn_preset_folder;
    std::unique_ptr<Slider> sld_speed;
    std::unique_ptr<Slider> sld_y_move;
    std::unique_ptr<Slider> sld_x_move;
    
    
    std::unique_ptr<TextEditor> txt_x_move;
    std::unique_ptr<TextEditor> txt_y_move;
    
    //ONLY FOR RENDERER 2 AND 3
    std::unique_ptr<TextEditor> txt_x;
    std::unique_ptr<TextEditor> txt_y;
    std::unique_ptr<TextEditor> txt_maxspeed;
    
    
    std::unique_ptr<Label> lbl_id;
    
    std::unique_ptr<SphereOpenGL> sphere_opengl;
    int msize; //[gc] size of the grahpic xy pad
    int mscale; // [gc] this is the scaling factor of the maxdist in layout, to be applied to graphics.
    float maxdist;// [gc] maximum distance considering the room and the space around it, according to xml layout config.
    float invdist, hfdist; 
    float dsecsrce;
    WFSRendererAudioProcessor* getProcessor() const
    {
        return static_cast <WFSRendererAudioProcessor*> (getAudioProcessor());
    }
    
    TooltipWindow tooltipWindow;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WFSRendererAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]



#endif  // PLUGINEDITOR_H_INCLUDED
