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

#ifndef __WFSRenderer__SphereOpenGL__
#define __WFSRenderer__SphereOpenGL__

#include <iostream>
#include <JuceHeader.h>
#include "PluginProcessor.h"


//#define _USE_MATH_DEFINES
#include <math.h>

#if 0

#ifdef __APPLE__
  #include <OpenGL/gl.h> //OS x libs
  #include <OpenGL/glu.h>
#else
    #ifdef _WIN32
      #include <windows.h>
    #endif
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

class SolidSphere
{
protected:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;
    
    GLfloat m_x = 0.0;
    GLfloat m_y = 0.0;
    GLfloat m_z = 0.0;
    
public:
    SolidSphere(float radius, unsigned int rings, unsigned int sectors);
    
    void draw();
    void setPosition(const float x, const float y, const float z = 1.0f);
};

class SphereOpenGL : public Component,
                     public OpenGLRenderer
{
    
public:
    SphereOpenGL(int numberOfSources = 1);
    ~SphereOpenGL();
    
    void paint (Graphics&);
    
    void renderOpenGL();
    void newOpenGLContextCreated();
    void openGLContextClosing();
    
    void mouseDown (const MouseEvent& e);
    void mouseUp (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    
    
    WFSRendererAudioProcessor* processor = nullptr;
    
    void update();
    /**
      * @brief Set position for a wfs sound source
     * @param sourceIndex The index of the chosen source
     * @param x coordinate normalized to /c maxdist
     * @param y coordinate normalized to /c maxdist
     * @param z coordinate normalized to /c maxdist
     */
    void setSourcePosition(const int sourceIndex,
                           const float x,
                           const float y,
                           const float z = 1.0);

private:
    
    OpenGLContext openGLContext;
    std::vector<SolidSphere> m_wfsSources;
    
    int m_selectedSource = 0;    
    bool _first_run;
};
#endif

// =========================================================================
// my own version....

class WfsSource
{
public:
    WfsSource(const float radius) : m_r(radius) { }
    
    void setPosition(const float x, const float y) { m_pt.x = x; m_pt.y = y; }
    void setLabel(const juce::String& l)   { m_label  = l; }
    void setColour(const juce::Colour& c ) { m_colour = c; }
    
    juce::Point<float> m_pt;
    float m_r = 4.0;
    juce::String m_label;
    juce::Colour m_colour = { juce::Colours::cyan };
    
};

/**
 * @brief Panel with sources.
 */
class WfsSourcesCanvas : public juce::Component
{
public:
    WfsSourcesCanvas(const int numberOfsources,
                     WFSRendererAudioProcessor* processor);
    
    void paint(juce::Graphics& g) override;
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    
    /**
      * @brief Set position for a wfs sound source
     * @param sourceIndex The index of the chosen source
     * @param x coordinate normalized to /c maxdist
     * @param y coordinate normalized to /c maxdist
     * @param z coordinate normalized to /c maxdist
     */
    void setSourcePosition(const int sourceIndex,
                           const float x,
                           const float y,
                           const float z = 1.0);
    
    ///! @brief \c True if the canvas layout is rotated of 90 deg counter-clockwise.
    bool isLayoutRotated() const { return m_layoutRotated; }
    
private:
    WFSRendererAudioProcessor* m_processor = nullptr;
    std::vector<WfsSource> m_wfsSources;
    int m_selectedSource = 0;
    bool m_layoutRotated = true;
    
};

#endif /* defined(__WFSRenderer__SphereOpenGL__) */
