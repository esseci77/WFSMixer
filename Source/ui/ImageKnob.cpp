/*
 * Copyright (c) 2013 - Teragon Audio LLC
 *
 * Permission is granted to use this software under the terms of either:
 * a) the GPL v2 (or any later version)
 * b) the Affero GPL v3
 *
 * Details of these licenses can be found at: www.gnu.org/licenses
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * ------------------------------------------------------------------------------
 *
 * This software uses the JUCE library.
 *
 * To release a closed-source product which uses JUCE, commercial licenses are
 * available: visit www.juce.com for more information.
 */

#include "ImageKnob.h"

namespace teragon
{
#ifdef USE_PARAM
ImageKnob::ImageKnob(ConcurrentParameterSet &parameters, const ParameterString &name,
                     const ResourceCache *resources, const String &imageName) :
Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox),
PluginParameterComponent(parameters, name, resources, imageName) {
    knobWidth = imageStates->normal.getWidth();
    filmstripImageCount = imageStates->normal.getHeight() / knobWidth - 1;
    knobArea.setX(0);
    knobArea.setY(0);
    knobArea.setWidth(knobWidth);
    knobArea.setHeight(knobWidth);

    setRange(0.0, 1.0);
    setValue(parameter->getScaledValue(), NotificationType::dontSendNotification);
}

void ImageKnob::onParameterUpdated(const Parameter* parameter) {
    juce::MessageManagerLock lock;
    setValue(parameter->getScaledValue(), NotificationType::dontSendNotification);
}

void ImageKnob::valueChanged() {
    onValueChanged(getValue());
}

void ImageKnob::paint(Graphics &g) {
    if(isMouseOver() && !isMouseButtonDown()) {
        onMouseOver();
    }

    const int y = (int)(filmstripImageCount * getValue()) * knobWidth;
    knobArea.setY(y);
    const Image knobFrame = imageStates->normal.getClippedImage(knobArea);
    const Image resizedKnobFrame = knobFrame.rescaled(knobWidth, knobWidth, Graphics::highResamplingQuality);
    g.drawImageAt(resizedKnobFrame, 0, 0);
}

#else // No parameters...
    
    ImageKnob::ImageKnob(const ResourceCache *resources, const String &imageName)
     : juce::Slider(juce::Slider::RotaryVerticalDrag,
                    juce::Slider::NoTextBox),
       ComponentWithCachedGraphics(resources, imageName)
    {
        
        knobWidth = imageStates->normal.getWidth();
        filmstripImageCount = imageStates->normal.getHeight() / knobWidth - 1;
        knobArea.setX(0);
        knobArea.setY(0);
        knobArea.setWidth(knobWidth);
        knobArea.setHeight(knobWidth);

        setRange(0.0, 1.0);
    }

    void ImageKnob::paint(Graphics &g)
    {
        const int y = (int)(filmstripImageCount * getValue()) * knobWidth;
        knobArea.setY(y);
        const Image knobFrame = imageStates->normal.getClippedImage(knobArea);
        const Image resizedKnobFrame = knobFrame.rescaled(knobWidth, knobWidth,
                                                          Graphics::highResamplingQuality);
        g.drawImageAt(resizedKnobFrame, 0, 0);
        
        if(isMouseOver() && !isMouseButtonDown())
        {
            //onMouseOver();
            auto r = getLocalBounds();
            g.setColour(juce::Colours::white);
            g.setFont(getLookAndFeel().getSliderPopupFont(*this).withHeight(22.0f));
            juce::String v = getTextFromValue(getValue()) + getTextValueSuffix();
            g.drawText(v, r.removeFromBottom(15), juce::Justification::centredBottom);
        }
    }

    juce::String ImageKnob::getTextFromValue(double value)
    {
        if (convert01Value)
        {
            value = convert01Value(value);
        }
        
        if (decimalPlaces > 0)
        {
            juce::String fs(value, decimalPlaces);
            return fs;
        }
        juce::String is((int)value);
        return is;
    }

#endif



} // namespace teragon
