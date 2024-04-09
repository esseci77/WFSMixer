/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_334E74D91B2532D2__
#define __JUCE_HEADER_334E74D91B2532D2__

//[Headers]     -- You can add your own extra header files here --
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

#include "JuceHeader.h"
#include "ResourceCache.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Resources  //: public juce::DeletedAtShutdown
{
public:
    //==============================================================================
    Resources ();
    ~Resources();

    teragon::ResourceCache* cache() { return m_cache.get(); }
    
  private:
    std::unique_ptr<teragon::ResourceCache> m_cache = nullptr;
    
  public:
    // Binary resources:
    static const char* indicator_light_active_png;
    static const int indicator_light_active_pngSize;
    static const char* indicator_light_inactive_png;
    static const int indicator_light_inactive_pngSize;
    static const char* push_button_normal_png;
    static const int push_button_normal_pngSize;
    static const char* push_button_pressed_png;
    static const int push_button_pressed_pngSize;
    static const char* slider_thumb_png;
    static const int slider_thumb_pngSize;
    static const char* slider_well_png;
    static const int slider_well_pngSize;
    static const char* toggle_button_normal_png;
    static const int toggle_button_normal_pngSize;
    static const char* toggle_button_pressed_png;
    static const int toggle_button_pressed_pngSize;
    static const char* large_knob_135frames_png;
    static const int large_knob_135frames_pngSize;
    static const char* small_knob_135frames_png;
    static const int small_knob_135frames_pngSize;


    juce_DeclareSingleton(Resources, false);
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_334E74D91B2532D2__
