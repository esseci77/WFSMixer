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
#include "SphereOpenGL.h"

#define DEG2RAD 0.01745329252
#define _3PI_4 3*M_PI/4

double constrainAngle(double x){
    x = fmod(x + M_PI,2*M_PI);
    if (x < 0)
        x += 2*M_PI;
    return x - M_PI;
}

#if 0
// ----------------------------------------------------------------------------
// SolidSphere implementation
// ----------------------------------------------------------------------------
SolidSphere::SolidSphere(float radius, unsigned int rings, unsigned int sectors)
{
    float const R = 1.f/(float)(rings-1);
    float const S = 1.f/(float)(sectors-1);
    int r, s;
    
    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator n = normals.begin();
    std::vector<GLfloat>::iterator t = texcoords.begin();
    
    for(r = 0; r < (int)rings; r++) for(s = 0; s < (int)sectors; s++)
    {
        float const y = sinf( -(float)M_PI_2 + (float)M_PI * r * R );
        float const x = cosf(2*(float)M_PI * s * S) * sinf( (float)M_PI * r * R );
        float const z = sinf(2*(float)M_PI * s * S) * sinf( (float)M_PI * r * R );
        
        *t++ = s*S;
        *t++ = r*R;
        
        *v++ = x * radius;
        *v++ = y * radius;
        *v++ = z * radius;
        
        *n++ = x;
        *n++ = y;
        *n++ = z;
    }
    
    indices.resize(rings * sectors * 4);
    std::vector<GLushort>::iterator i = indices.begin();
    
    for(r = 0; r < (int)rings-1; r++) for(s = 0; s < (int)sectors-1; s++)
    {
        *i++ = r * sectors + s;
        *i++ = r * sectors + (s+1);
        *i++ = (r+1) * sectors + (s+1);
        *i++ = (r+1) * sectors + s;
    }
}

void SolidSphere::draw()
{
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        
    glTranslatef(m_x, m_y, m_z);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
    glNormalPointer(GL_FLOAT, 0, &normals[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
    glDrawElements(GL_QUADS, (GLsizei)indices.size(),
                   GL_UNSIGNED_SHORT, &indices[0]);
    glPopMatrix();
}

void SolidSphere::setPosition(const float x, const float y, const float z)
{
    m_x = (GLfloat)x;
    m_y = (GLfloat)y;
    m_z = (GLfloat)z;
}


// ----------------------------------------------------------------------------
// SphereOpenGL implementation
// ----------------------------------------------------------------------------

SphereOpenGL::SphereOpenGL(int numberOfSources)
{
    for(int i = 0; i < numberOfSources; i++)
    {
        m_wfsSources.push_back(SolidSphere(0.03f, 12, 24));
    }
    
    openGLContext.setRenderer (this);
    openGLContext.setComponentPaintingEnabled (true);
    openGLContext.setContinuousRepainting(true); // manually repaint ?    
    openGLContext.attachTo (*this);
}

SphereOpenGL::~SphereOpenGL()
{
    openGLContext.detach();
}

void SphereOpenGL::paint (Graphics& g)
{
    auto area = getLocalBounds();
    float maxdist = 30;
    float rw = 5.0f; // default room width
    float rl = 5.0f; // default room length
    
    if (processor->layout())
    {
        maxdist = processor->layout()->maxdist();
        rw = processor->layout()->lenfrontwall();
        rl = processor->layout()->lensidewall();
    }
    const float m2px = area.getWidth() / maxdist; // convert meters to pixel
    
#if 0
    //[gc] at the moment the size of the room isn't configurable
    float l1 = 3.6590;
    float l2 = 2.1590;

    setSource(l1,0);
    l1*=(mscale*2);
    l2*=(mscale*2);
#endif

    float d = 10.0f;
    juce::Rectangle<float> shapeBox;
    g.setColour (Colour (0xff95a52a));
    
    while (d <= maxdist)
    {
        shapeBox.setSize(d * m2px, d * m2px);
        shapeBox.setCentre(area.getCentreX(), area.getCentreY());
        g.drawEllipse(shapeBox, 3.0f);
        
        d += 10.0f;
    }
    g.drawLine(0, area.getHeight() / 2, area.getWidth(), area.getHeight() / 2);
    g.drawLine(area.getWidth() / 2, 0, area.getWidth() / 2, area.getHeight());
    
    auto w = m2px * rw;
    auto l = m2px * rl;
    
    shapeBox.setSize(w, l);
    shapeBox.setCentre(area.getCentreX(), area.getCentreY());
    
    g.setColour (Colour (0xff333333));
    g.fillRect(shapeBox);
}

void SphereOpenGL::renderOpenGL()
{
    OpenGLHelpers::clear (Colours::black.withAlpha (1.f));
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    const float desktopScale = (float) openGLContext.getRenderingScale();
    
    glViewport (0, 0,
                roundToInt (desktopScale * getWidth()),
                roundToInt (desktopScale * getHeight()));
    
    // glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    
    // glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    
    // set light parameters
    GLfloat mat_specular[4] = { 0.18f, 0.18f, 0.18f, 1.f };
    GLfloat mat_shininess[] = { 40.f };
    GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.f };
    GLfloat light0_ambient[] = { 0.0f, 0.0f, 0.0f, 1.f };
    GLfloat light0_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.f };
    GLfloat light0_specular[] = { 0.85f, 0.85f, 0.85f, 1.f };
    
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0);
  
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);// todo include this into spotlight node
    
    
    
    // draw source
      glColor4f(0,1,0.0,1.0);
#if 0
    float x = mPhi*invwidth*2;
    float y = mTheta*invheight*2;
    float z = 1.0f;
    

sphere_source.draw(x, y, z);
#else
    for(auto& s : m_wfsSources)
    {
      //  glColor4f(i/INPUT_CHANNELS,i/INPUT_CHANNELS,i/INPUT_CHANNELS,1.0);
        s.draw();
    }
#endif
    
}

void SphereOpenGL::newOpenGLContextCreated()
{
    
    
}

void SphereOpenGL::openGLContextClosing()
{
    
}

void SphereOpenGL::setSourcePosition(const int sourceIndex,
                                     const float x,
                                     const float y,
                                     const float z)
{
    
    auto maxdist = processor->layout()->maxdist();    
//    from (-maxdist/2, maxdist/2) to (-1, 1)
    m_wfsSources[sourceIndex].setPosition(2.0f * x / maxdist,
                                          2.0f * y / maxdist,
                                          z);
}

void SphereOpenGL::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        juce::PopupMenu m;
        m.addItem("Rotate layout", [this]
        {
            
        });
        m.addSeparator();
        m.addItem("Save sources layout", [this]
        {
            
        });
        m.addItem("Load sources layout", [this]
        {
            
        });
        m.show();
    }
    else
    {
        mouseDrag(e);
    }
}

void SphereOpenGL::mouseDrag(const juce::MouseEvent &e)
{
    auto area = getLocalBounds();
    
    float x = (float)e.getPosition().x;
    float y = (float)e.getPosition().y;
    const float w = (float)area.getWidth();
    const float h = (float)area.getHeight();
    
    x = 2.0f * x /w - 1.0f;           // convert to -1..1
    y = 2.0f * (1.0f - y /h) - 1.0f;  // invert axis and convert to -1..1
    
    m_wfsSources[m_selectedSource].setPosition(x, y);
    
    if (processor && processor->isConfigLoaded())
    {
        *processor->params()[m_selectedSource].xpos = 100.0f * (0.5f * x + 0.5f);
        *processor->params()[m_selectedSource].ypos = 100.0f * (0.5f * y + 0.5f);
    }
}

void SphereOpenGL::mouseUp(const juce::MouseEvent &e)
{
    //_mTheta = mTheta;
}


void SphereOpenGL::update()
{
    std::cout << "update";
}
#endif

// =========================================================================
// my own version....


WfsSourcesCanvas::WfsSourcesCanvas(const int numberOfSources,
                                   WFSRendererAudioProcessor* processor)
 : juce::Component("WfsSourceCanvas"),
   m_processor(processor)
{
    jassert(m_processor);
    
    for(int i = 0; i < numberOfSources; i++)
    {
        m_wfsSources.push_back(WfsSource(5.0f));
        m_wfsSources.back().setLabel(juce::String(i + 1));
    }
    m_layoutRotated = m_processor->params().isLayoutRotated();
}

void WfsSourcesCanvas::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    float maxdist = 30;
    
    juce::Path roomShape;
    juce::Point<float> firstSpeakerPosition;
    
    // define room shape
    if (m_processor->layout())
    {
        maxdist = m_processor->layout()->maxdist();
        auto rc = m_processor->layout()->roomCorners();
        
        roomShape.startNewSubPath(rc[0].x, rc[0].y);
        
        for (auto i = 1; i < rc.size(); ++i)
        {
            roomShape.lineTo(rc[i].x, rc[i].y);
        }
        roomShape.closeSubPath();
        
        auto cd = m_processor->layout()->chandata();
        
        firstSpeakerPosition.x = cd[0]._xpos;
        firstSpeakerPosition.y = cd[0]._ypos;
    }
    else
    {
        // default room is 5 x 5 m
        roomShape.startNewSubPath(2.5, 2.5);
        roomShape.lineTo(2.5, -2.5);
        roomShape.lineTo(-2.5, -2.5);
        roomShape.lineTo(-2.5, 2.5);
        roomShape.closeSubPath();
    }
    
    const float m2px = area.getWidth() / maxdist; // convert meters to pixel

    float d = 10.0f;
    auto shapeBox = roomShape.getBounds();
    const auto rw = shapeBox.getWidth();
    const auto rh = shapeBox.getHeight();

    // plot axes and distance circles
//    g.setColour (juce::Colour (0xff95a52a));
    g.setColour(juce::Colours::yellow);
    while (d <= maxdist)
    {
        shapeBox.setSize(d * m2px, d * m2px);
        shapeBox.setCentre((float)area.getCentreX(), 
                           (float)area.getCentreY());
        g.drawEllipse(shapeBox, 1.0f);
        
        d += 10.0f;
    }
    const float aw = (float)area.getWidth();
    const float ah = (float)area.getHeight();
    
    g.drawLine(0, 0.5f * ah, aw, 0.5f * ah);
    g.drawLine(0.5f * aw, 0, 0.5f * aw, ah);

    auto w = m2px * rw;
    auto l = m2px * rh;

    shapeBox.setSize(w, l);
    shapeBox.setCentre((float)area.getCentreX(), 
                       (float)area.getCentreY());
    
    juce::AffineTransform matrix;

    if (!m_layoutRotated)
    {
        // Angelo likes it...
        matrix = juce::AffineTransform::rotation(float(M_PI_2));
    }
     matrix = matrix.followedBy(juce::AffineTransform::scale(m2px,
                                                           -m2px).translated(0.5f * aw,
                                                                             0.5f * ah));
    // plot the room's contour and fill
    // g.setColour (Colour (0xff444444));
    g.setColour(juce::Colours::grey);
    g.fillPath(roomShape, matrix);
    // g.setColour (Colour (0xff333333));
    g.setColour(juce::Colours::brown);
    g.strokePath(roomShape,
                 juce::PathStrokeType(4.0f),
                 matrix);
    
    // plot the first loudspeaker with a red circle
    juce::Rectangle<float> r;
    if (   firstSpeakerPosition.x != 0.0
        || firstSpeakerPosition.y != 0.0)
    {
        matrix.transformPoint(firstSpeakerPosition.x,
                              firstSpeakerPosition.y);
        r.setSize(10.0f, 10.0f);
        r.setCentre(firstSpeakerPosition);
        g.setColour(juce::Colours::red);
        g.fillEllipse(r);
    }
    
    // write the x-axis and y-axis labels
    g.setFont(20.0f);
    r.setWidth(10.0f);
    r.setHeight(10.0f);

    if (!m_layoutRotated)
    {
        r.setX(0.5f * aw - 15.0f);
        r.setY(5.0f);
        g.drawText("x", r, juce::Justification::centredTop);

        r.setX(5.0f);
        r.setY(0.5f * ah + 5.0f);
        g.drawText("y", r, juce::Justification::centredTop);
    }
    else
    {
        r.setX(aw - 15.0f);
        r.setY(0.5f * ah + 5.0f);
        g.drawText("x", r, juce::Justification::centredTop);
        
        r.setX(0.5f * aw - 15.0f);
        r.setY(5.0f);
        g.drawText("y", r, juce::Justification::centredTop);
    }
    g.setFont(20.0f);
    
    // plot the sources (circles, cyan) and their numbers
    for (int i = 0; i < m_wfsSources.size(); ++i)
    {
        if(!(* m_processor->params()[i].mute))
        {
            auto& ws = m_wfsSources[i];
        
            const float d_i = 3.0 * ws.m_r;
            r.setSize(d_i, d_i);
            r.setCentre(!m_layoutRotated ? 0.5f * (1.0f - ws.m_pt.y) * ah
                                        : 0.5f * (ws.m_pt.x + 1.0f) * aw,
                        !m_layoutRotated ? 0.5f * (1.0f - ws.m_pt.x) * aw
                                        : 0.5f * (1.0f - ws.m_pt.y) * ah);
            g.setColour (ws.m_colour);
            g.fillEllipse(r);
        
            r.translate(0, d_i + 3);
            g.drawText(ws.m_label, r, juce::Justification::centredTop);
        }
    }
}

void WfsSourcesCanvas::setSourcePosition(const int sourceIndex,
                                         const float x,
                                         const float y,
                                         const float /*z*/)
{
    
    auto maxdist = m_processor->layout()->maxdist();
//    from (-maxdist/2, maxdist/2) to (-1, 1)
    m_wfsSources[sourceIndex].setPosition(2.0f * x / maxdist,
                                          2.0f * y / maxdist);
}

void WfsSourcesCanvas::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        juce::PopupMenu m;
        m.addItem("Rotate layout", [this]
        {
            m_layoutRotated = ! m_layoutRotated;
            m_processor->params().setLayoutRotated(m_layoutRotated);
            repaint();
        });
        m.addSeparator();
        m.addItem("Save sources layout", [this]
        {
            auto ed = dynamic_cast<WFSRendererAudioProcessorEditor*>(m_processor->getActiveEditor());
            ed->saveLayout();
        });
        m.addItem("Load sources layout", [this]
        {
            auto ed = dynamic_cast<WFSRendererAudioProcessorEditor*>(m_processor->getActiveEditor());
            ed->loadLayout();
        });
        m.show();
        return;
    }
    auto area = getLocalBounds();
    
    float x = (float)e.getPosition().x;
    float y = (float)e.getPosition().y;
    const float w = (float)area.getWidth();
    const float h = (float)area.getHeight();

    juce::Point<float> pm;
                                                                                       
    pm.setXY(!m_layoutRotated ? 2.0f * (1.0f - y/h) - 1.0f
                             : 2.0f * x/w - 1.0f,           // convert to -1..1
             !m_layoutRotated ? 2.0f * (1.0f - x/w) - 1.0f
                             : 2.0f * (1.0f - y/h) - 1.0f); // invert axis and
                                                            // convert to -1..1)
    float dmin = 1e12f;
    int   imin = -1;
    
    for (int i = 0; i < m_wfsSources.size(); ++i)
    {
        const float d = m_wfsSources[i].m_pt.getDistanceSquaredFrom(pm);
        
        if (d < dmin)
        {
            dmin = d;
            imin = i;
        }
    }
    jassert(imin >= 0);
    m_selectedSource = imin;
    repaint();
}

void WfsSourcesCanvas::mouseDrag(const juce::MouseEvent &e)
{
    auto area = getLocalBounds();
    
    const float x = (float)e.getPosition().x;
    const float y = (float)e.getPosition().y;
    const float w = (float)area.getWidth();
    const float h = (float)area.getHeight();
    
    const float xp = !m_layoutRotated ? 2.0f * (1.0f - y/h) - 1.0f
                                     : 2.0f * x/w - 1.0f;           // convert to -1..1
    const float yp = !m_layoutRotated ? 2.0f * (1.0f - x/w) - 1.0f
                                     : 2.0f * (1.0f - y/h) - 1.0f;  // invert axis and
                                                                    // convert to -1..1
    m_wfsSources[m_selectedSource].setPosition(xp, yp);
    
    if (m_processor && m_processor->isConfigLoaded())
    {
        *m_processor->params()[m_selectedSource].xpos = 100.0f * (0.5f * xp + 0.5f);
        *m_processor->params()[m_selectedSource].ypos = 100.0f * (0.5f * yp + 0.5f);
    }
    repaint();
}
