/*
  ==============================================================================

    WfsMixerStripComponent.cpp
    Created: 15 Feb 2021 10:34:03am
    Author:  Simone Campanini

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WfsLookAndFeel.h"
#include "PluginProcessor.h"
#include "WfsMixerStripComponent.h"

static double twoPi = 2* M_PI;

// -----------------------------------------------------------------------------
WfsToggleButton::WfsToggleButton(const juce::String& buttonText,
                                 const juce::Colour& onStatusColour)
 : juce::TextButton(buttonText),
   m_onStatusColour(onStatusColour)
{
    m_offStatusColour = findColour(juce::TextButton::buttonColourId);
    
}

void WfsToggleButton::paintButton(juce::Graphics& g,
                                  bool isMouseOverButton,
                                  bool isButtonDown)
{
    if (m_param)
    {
        setColour(juce::TextButton::buttonColourId,
                  *m_param ? m_onStatusColour : m_offStatusColour);
    }
    TextButton::paintButton(g, isMouseOverButton, isButtonDown);
}


// -----------------------------------------------------------------------------

WfsMixerStripComponent::WfsMixerStripComponent(const int stripIndex,
                                               teragon::ResourceCache* resourceCache,
                                               const juce::String& stripName)
 : juce::Component(stripName + juce::String(stripIndex)),
   m_sourceIndex(stripIndex)
{
    jassert(resourceCache != nullptr);
    
    juce::String indexStr(stripIndex);
    
    m_soloButton.reset(new WfsToggleButton("s",
                                           juce::Colour(0xffeebb40)));
    m_soloButton->setConnectedEdges(juce::Button::ConnectedOnLeft);
    addAndMakeVisible(m_soloButton.get());
    
    m_muteButton.reset(new WfsToggleButton("m",
                                           juce::Colour(0xffdd7230)));
    m_muteButton->setConnectedEdges(juce::Button::ConnectedOnRight);
    addAndMakeVisible(m_muteButton.get());

    // Horizontal linear velocity: fine control of linear speed along x axis.
    m_xspdKnob.reset(new teragon::ImageKnobSmall(resourceCache));
    m_xspdKnob->setDoubleClickReturnValue(true, 0.5);
    m_xspdKnob->setPopupDisplayEnabled(true, true, m_xspdKnob.get());
    addAndMakeVisible(m_xspdKnob.get());
    
    m_xspdKnob->convert01Value = [this](double value01) -> double
    {
        const double mspd = 36.0 * this->m_mspdKnob->getValue();
        const double  xs = value01 - 0.5;
        const double  vx = (xs != 0.0 ?   std::copysign(1.0, xs)
                                        * std::pow(mspd, std::abs(xs) / 0.5)
                                      : 0.0);
        return vx;
    };

    // Vertical linear velocity: fine control of linear speed along y axis.
    m_yspdKnob.reset(new teragon::ImageKnobSmall(resourceCache));
    m_yspdKnob->setDoubleClickReturnValue(true, 0.5);
    addAndMakeVisible(m_yspdKnob.get());

    m_yspdKnob->convert01Value = [this](double value01) -> double
    {
        const double mspd = 36.0 * this->m_mspdKnob->getValue();
        const double  ys = value01 - 0.5;
        const double  vy = (ys != 0.0 ?   std::copysign(1.0, ys)
                                        * std::pow(mspd, std::abs(ys) / 0.5)
                                      : 0.0);
        return vy;
    };

    // Maximum linear speed: coarse control of linear velocity.
    m_mspdKnob.reset(new teragon::ImageKnobSmall(resourceCache));
    addAndMakeVisible(m_mspdKnob.get());

    m_mspdKnob->convert01Value = [](double value01) -> double
    {
        return 36.0 * value01;
    };
    
    // Angular speed
    m_cspdKnob.reset(new teragon::ImageKnobSmall(resourceCache));
    m_cspdKnob->setDoubleClickReturnValue(true, 0.5);
    m_cspdKnob->setDecimalPlaces(1);
    addAndMakeVisible(m_cspdKnob.get());

    m_cspdKnob->convert01Value = [](double value01) -> double
    {
        // max angular speed is 2pi rad/s, convert to angular speed
        const double as = value01 - 0.5;
        const double  w = (as != 0.0 ?   std::copysign(1.0, as)
                                       * std::pow(twoPi, std::abs(as) / 0.5)
                                     : 0.0);
        return w;
    };
    
    // Radius of circular motion.
    m_radsKnob.reset(new teragon::ImageKnobSmall(resourceCache));
    m_radsKnob->setDecimalPlaces(2);
    addAndMakeVisible(m_radsKnob.get());
    
    // Source gain.
    m_gainKnob.reset(new teragon::ImageKnobSmall(resourceCache));
    m_gainKnob->setDecimalPlaces(0);
    addAndMakeVisible(m_gainKnob.get());

    m_gainKnob->convert01Value = [](double value01) -> double
    {
        const double G = minusInfinitydB * (1.0f - value01);
        return G;
    };

    m_soloButton->addListener(this);
    m_muteButton->addListener(this);
    m_xspdKnob->addListener(this);
    m_yspdKnob->addListener(this);
    m_mspdKnob->addListener(this);
    m_cspdKnob->addListener(this);
    m_radsKnob->addListener(this);
    m_gainKnob->addListener(this);
    
    const int height =   kTopBorder
                       + kStripLabelHeight + kKnobsClearance
                       + kMuteButtonHeight + kKnobsClearance
                       + 6 * (kControlLabelsHeight
                              + kControlLabelClearance
                              + kKnobHeight
                              + kKnobsClearance);
    setSize(kStripWidth, height);
}

void WfsMixerStripComponent::setParameters(WFSRendererParameters * param)
{
    m_parameters = param;
    
    if (param)
    {
        m_soloButton->set((*m_parameters)[m_sourceIndex].solo);
        m_muteButton->set((*m_parameters)[m_sourceIndex].mute);
        update();
    }
    else
    {
        m_soloButton->set(nullptr);
        m_muteButton->set(nullptr);
    }
}

void WfsMixerStripComponent::update()
{
    if (m_parameters)
    {
        jassert(m_sourceIndex >= 0);
        
        float value = *(*m_parameters)[m_sourceIndex].xspd / 100.0f;
        m_xspdKnob->setValue(value, dontSendNotification);
        
        value = *(*m_parameters)[m_sourceIndex].yspd / 100.0f;
        m_yspdKnob->setValue(value, dontSendNotification);
        
        value = *(*m_parameters)[m_sourceIndex].speed / 100.0f;
        m_mspdKnob->setValue(value, dontSendNotification);
        
        value = *(*m_parameters)[m_sourceIndex].aspd / 100.0f;
        m_cspdKnob->setValue(value, dontSendNotification);

        value = *(*m_parameters)[m_sourceIndex].radius / 100.0f;
        m_radsKnob->setValue(value, dontSendNotification);

        value = 1.0f - (*(*m_parameters)[m_sourceIndex].gain / float(minusInfinitydB));
        m_gainKnob->setValue(value, dontSendNotification);
    }
}

void WfsMixerStripComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    auto laf  = WfsLookAndFeel::getInstance();
    
    g.fillAll (Colour(0xff26272a));
    g.setColour(juce::Colours::white);
        
    area.removeFromTop(kTopBorder);
    
//    g.setFont(laf->getLightFont().withHeight(28.0f)); 
    g.setFont(laf->getBoldFont().withHeight(28.0f));
    g.drawText(juce::String(m_sourceIndex + 1),
               area.removeFromTop(kStripLabelHeight),
               juce::Justification::centred);
    
    area.removeFromTop(kKnobsClearance + kMuteButtonHeight + kKnobsClearance);
    
    // g.setFont(laf->getLightFont().withHeight(20.0));
    g.setFont(laf->getBoldFont().withHeight(20.0));
    g.drawText("gain",
               area.removeFromTop(kControlLabelsHeight),
               juce::Justification::centred);

    area.removeFromTop(kControlLabelClearance + kKnobHeight + kKnobsClearance);
    
    g.drawText("x spd",
               area.removeFromTop(kControlLabelsHeight),
               juce::Justification::centred);
    
    area.removeFromTop(kControlLabelClearance + kKnobHeight + kKnobsClearance);

    g.drawText("y spd",
               area.removeFromTop(kControlLabelsHeight),
               juce::Justification::centred);
    
    area.removeFromTop(kControlLabelClearance + kKnobHeight + kKnobsClearance);
    
    g.drawText("max spd",
               area.removeFromTop(kControlLabelsHeight),
               juce::Justification::centred);
    
    area.removeFromTop(kControlLabelClearance + kKnobHeight + kKnobsClearance);

    g.drawText("ang spd",
               area.removeFromTop(kControlLabelsHeight),
               juce::Justification::centred);
    
    area.removeFromTop(kControlLabelClearance + kKnobHeight + kKnobsClearance);

    g.drawText("radius",
               area.removeFromTop(kControlLabelsHeight),
               juce::Justification::centred);
    
    g.drawLine(float(getWidth() - 1), float(kTopBorder),
               float(getWidth() - 1), float(getHeight() - kTopBorder));
}

void WfsMixerStripComponent::resized()
{
    auto area = getLocalBounds();
    area.reduce((kStripWidth - kKnobWidth) / 2, 0);
    
    area.removeFromTop(kTopBorder + kStripLabelHeight + kKnobsClearance);

    auto buttonsStrip = area.removeFromTop(20).reduced(5, 0);
    m_muteButton->setBounds(buttonsStrip.removeFromLeft((kKnobWidth - 10)/ 2));
    m_soloButton->setBounds(buttonsStrip);
    
    area.removeFromTop(kKnobsClearance + kControlLabelsHeight + kControlLabelClearance);
    m_gainKnob->setBounds(area.removeFromTop(kKnobHeight));

    area.removeFromTop(kKnobsClearance + kControlLabelsHeight + kControlLabelClearance);
    m_xspdKnob->setBounds(area.removeFromTop(kKnobHeight));
    
    area.removeFromTop(kKnobsClearance + kControlLabelsHeight + kControlLabelClearance);
    m_yspdKnob->setBounds(area.removeFromTop(kKnobHeight));
    
    area.removeFromTop(kKnobsClearance + kControlLabelsHeight + kControlLabelClearance);
    m_mspdKnob->setBounds(area.removeFromTop(kKnobHeight));
    
    area.removeFromTop(kKnobsClearance + kControlLabelsHeight + kControlLabelClearance);
    m_cspdKnob->setBounds(area.removeFromTop(kKnobHeight));
    
    area.removeFromTop(kKnobsClearance + kControlLabelsHeight + kControlLabelClearance);
    m_radsKnob->setBounds(area.removeFromTop(kKnobHeight));
    
}

 void WfsMixerStripComponent::sliderValueChanged (Slider* sliderThatWasMoved)
 {
     // NDSC: some all parameters values are converted in 0,100 range here....
     const float value = (float)sliderThatWasMoved->getValue();
     
     if(m_parameters)
     {
         if (sliderThatWasMoved == m_xspdKnob.get())
         {
             *(*m_parameters)[m_sourceIndex].xspd = 100.0f * value;
         }
         else if (sliderThatWasMoved == m_yspdKnob.get())
         {
             *(*m_parameters)[m_sourceIndex].yspd = 100.0f * value;
         }
         else if (sliderThatWasMoved == m_mspdKnob.get())
         {
             *(*m_parameters)[m_sourceIndex].speed = 100.0f * value;
         }
         else if (sliderThatWasMoved == m_cspdKnob.get())
         {
             *(*m_parameters)[m_sourceIndex].aspd = 100.0f * value;
         }
         else if (sliderThatWasMoved == m_radsKnob.get())
         {
             *(*m_parameters)[m_sourceIndex].radius = 100.0f * value;
         }
         else if (sliderThatWasMoved == m_gainKnob.get())
         {
             *(*m_parameters)[m_sourceIndex].gain = float(minusInfinitydB) * (1.0f - value);
         }
     }
 }

void WfsMixerStripComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == m_soloButton.get())
    {
        bool soloed = *(*m_parameters)[m_sourceIndex].solo;
        soloed = ! soloed;
        
        *(*m_parameters)[m_sourceIndex].solo = soloed;
        
        DBG("solo " + juce::String(soloed ? "on" : "off"));
        repaint();
    }
    else if (buttonThatWasClicked == m_muteButton.get())
    {
        bool muted = *(*m_parameters)[m_sourceIndex].mute;
        muted = ! muted;
        
        *(*m_parameters)[m_sourceIndex].mute = muted;
        
        DBG("mute " + juce::String(muted ? "on" : "off"));
        repaint();
    }
}


// ==========================================================================

WfsMixerComponent::WfsMixerComponent(juce::AudioProcessor* processor,
                                     teragon::ResourceCache* resourceCache)
 : juce::Component("WfsMixerComponent")
{
    jassert(resourceCache != nullptr);
    
    auto wp = dynamic_cast<WFSRendererAudioProcessor*>(processor);
    const int nSources = wp->getTotalNumInputChannels();
    
    for (int i = 0; i < nSources; ++i)
    {
        auto stripe = new WfsMixerStripComponent(i, resourceCache);
        stripe->setParameters(&wp->params());
        m_strips.add(stripe);
        addAndMakeVisible(stripe);
    }
    const int h = m_strips[0]->getHeight();
    const int w = nSources * kStripWidth;
    setSize(w, h);
}

void WfsMixerComponent::resized()
{
    auto h = m_strips[0]->getHeight();
    juce::Rectangle<int> area(0, 0, m_strips.size() * kStripWidth, h);
    
    for (int i = 0; i < m_strips.size(); ++i)
    {
        m_strips[i]->setBounds(area.removeFromLeft(kStripWidth));
    }
}

void WfsMixerComponent::update()
{
    for (int i = 0; i < m_strips.size(); ++i)
    {
        m_strips[i]->update();
    }
}
