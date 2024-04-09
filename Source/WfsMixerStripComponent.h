/*
  ==============================================================================

    WfsMixerStripComponent.h
    Created: 15 Feb 2021 10:34:03am
    Author:  Simone Campanini

  ==============================================================================
*/

#pragma once

#include "ui/TeragonGuiComponents.h"


#define kTopBorder             12
#define kKnobWidth             66
#define kKnobHeight            66
#define kMuteButtonHeight      20 //40
#define kStripWidth            80
#define kStripLabelHeight      14
#define kControlLabelsHeight   11
#define kControlLabelClearance  6
#define kKnobsClearance        12

// -----------------------------------------------------------------------------

class WfsToggleButton : public juce::TextButton
{
  public:
    WfsToggleButton(const juce::String& buttonText,
                    const juce::Colour& onStatusColour = juce::Colours::red);
    
    void set(juce::AudioParameterBool* param) { m_param = param; }
    
  protected:
    void paintButton(juce::Graphics& g,
                     bool isMouseOverButton,
                     bool isButtonDown) override;
    
    juce::Colour m_onStatusColour;
    juce::Colour m_offStatusColour;
    juce::AudioParameterBool* m_param = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WfsToggleButton);
};

// -----------------------------------------------------------------------------

class WfsAudioProcessor;
class WfsRendererParameters;

class WfsMixerStripComponent : public juce::Component,
                               public juce::Slider::Listener,
                               public juce::Button::Listener
{
public:
    WfsMixerStripComponent(const int stripIndex,
                           teragon::ResourceCache* resourceCache,
                           const juce::String& name = "MixerStrip");
    
    void setParameters(WFSRendererParameters * param);
    void update();
    
protected:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved)   override;
    void buttonClicked     (juce::Button* buttonThatWasClicked) override;
    
    //juce::TooltipWindow                     m_tooltip;
    
    std::unique_ptr<WfsToggleButton>        m_soloButton;
    std::unique_ptr<WfsToggleButton>        m_muteButton;
    
    std::unique_ptr<teragon::ImageKnobSmall> m_xspdKnob;
    std::unique_ptr<teragon::ImageKnobSmall> m_yspdKnob;
    std::unique_ptr<teragon::ImageKnobSmall> m_mspdKnob;
    std::unique_ptr<teragon::ImageKnobSmall> m_cspdKnob;
    std::unique_ptr<teragon::ImageKnobSmall> m_radsKnob;
    std::unique_ptr<teragon::ImageKnobSmall> m_gainKnob;
    
  private:
    int   m_sourceIndex = -1;

    WFSRendererParameters* m_parameters = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WfsMixerStripComponent);
};

// ==========================================================================

class WfsMixerComponent : public juce::Component
{
public:
    WfsMixerComponent(juce::AudioProcessor* processor,
                      teragon::ResourceCache* resourceCache);
    
    void resized() override;
    
    void update();
private:
    juce::OwnedArray<WfsMixerStripComponent> m_strips;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WfsMixerComponent);
};
