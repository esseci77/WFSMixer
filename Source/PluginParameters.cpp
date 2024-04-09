#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameters.h"


// ---------------------------------------------------------------[ esseci ]----

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(juce::AudioProcessor* p)
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (int i = 0; i < p->getTotalNumInputChannels(); ++i)
    {
        juce::String paramId("XPos");
        juce::String label("X position");
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 0
                                                               label,
                                                                 0.0f,
                                                               100.0f,
                                                                50.0f));
        paramId = "YPos";
        label   = "Y position";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 1
                                                               label,
                                                                 0.0f,
                                                               100.0f,
                                                                50.0f));
        paramId = "XSpeed";
        label   = "X speed";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 2
                                                               label,
                                                                 0.0f,
                                                               100.0f,
                                                                50.0f));
        paramId = "YSpeed";
        label   = "Y speed";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 3
                                                               label,
                                                                 0.0f,
                                                               100.0f,
                                                                50.0f));
        paramId = "MaxSpeed";
        label   = "Max Speed";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 4
                                                               label,
                                                               0.0f,
                                                             100.0f,
                                                              25.0f));
        paramId = "AngSpeed";
        label   = "Ang speed";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 5
                                                               label,
                                                                 0.0f,
                                                               100.0f,
                                                                50.0f));
        paramId = "Radius";
        label   = "Radius";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 6
                                                               label,
                                                                 0.0f,
                                                               100.0f,
                                                                25.0f));
        paramId = "Gain";
        label   = "Gain";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterFloat>(paramId,     // 7
                                                               label,
                                                               float(minusInfinitydB),
                                                                0.0f,
                                                               -3.0f));
        paramId = "Mute";
        label   = "Mute";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterBool>(paramId,      // 8
                                                              label,
                                                              false));
        paramId = "Solo";
        label   = "Solo";
        paramId << i;
        label   << i + 1;
        params.push_back(std::make_unique<AudioParameterBool>(paramId,      // 9
                                                              label,
                                                              false));
    }
    return { params.begin(), params.end() };
}

WFSRendererParameters::WFSRendererParameters(juce::AudioProcessor& p)
  : m_valueTreeState(p, nullptr, "PARAMETERS", createParameterLayout(&p))
{
    m_nSources = MAX_SOURCES; //p.getTotalNumInputChannels();
    m_srcParams.resize(m_nSources);
    
    for (int i = 0; i < m_nSources; ++i)
    {
        juce::String paramId("XPos");
        paramId << i;
        m_srcParams[i].xpos  = (AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "YPos";
        paramId << i;
        m_srcParams[i].ypos  = (AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "XSpeed";
        paramId << i;
        m_srcParams[i].xspd  = (AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "YSpeed";
        paramId << i;
        m_srcParams[i].yspd  = (AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "MaxSpeed";
        paramId << i;
        m_srcParams[i].speed =(AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "AngSpeed";
        paramId << i;
        m_srcParams[i].aspd =(AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "Radius";
        paramId << i;
        m_srcParams[i].radius =(AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "Gain";
        paramId << i;
        m_srcParams[i].gain =(AudioParameterFloat*)m_valueTreeState.getParameter(paramId);
        paramId = "Mute";
        paramId << i;
        m_srcParams[i].mute =(AudioParameterBool*)m_valueTreeState.getParameter(paramId);
        paramId = "Solo";
        paramId << i;
        m_srcParams[i].solo =(AudioParameterBool*)m_valueTreeState.getParameter(paramId);
    }
}

bool WFSRendererParameters::thereAreSolos() const
{
    for (auto& p : m_srcParams)
    {
        if (*p.solo)
        {
            return true;
        }
    }
    return false;
}

void WFSRendererParameters::update(double sampleRate, int bufferLength)
{
    const double dt = (double)bufferLength/sampleRate;
    update(dt);
}

void WFSRendererParameters::update(const double dt)
{
    static const float twoPi = 2.0 * M_PI;
    
    // [esseci] using juce valueTreeStateParameters (get rid of deprecations)
    
    for (auto i = 0; i < m_srcParams.size(); ++i)
    {
        auto& p = m_srcParams[i];
        // p.sx = 0.0f;
        // p.sy = 0.0f;
        
        // max speed is 36 m/s ~= 130 km/h (* 360.0);
        const float maxSpd = *p.speed * 0.01f * 36.0f;
        
        // convert radius to (0.1, maxdist) range
        const float r = *p.radius * 0.01f * (m_maxdist/2.0f - 0.1f) + 0.1f;
        
        // convert speeds to (-0.5, +0.5) range
        float xs = *p.xspd * 0.01f - 0.5f;
        float ys = *p.yspd * 0.01f - 0.5f;
        float as = *p.aspd * 0.01f - 0.5f;

        // max angular speed is 2pi rad/s, convert to angular speed
        const float w = (as != 0.0f ?   std::copysign(1.0f, as)
                                      * std::pow(twoPi, std::abs(as) / 0.5f)
                                    : 0.0f);
        p.angPos += float(w * dt);
        
        if (p.angPos >  twoPi) { p.angPos -= twoPi; } else
        if (p.angPos < -twoPi) { p.angPos += twoPi; }
                
        const float xa = w != 0 ? r * std::cos(p.angPos) : 0.0f; // meters
        const float ya = w != 0 ? r * std::sin(p.angPos) : 0.0f; // meters

        const float vx = (xs != 0.0f ? std::copysign(1.0f, xs)
                                      * std::pow(maxSpd, std::abs(xs) / 0.5f)
                                    : 0.0f);
        const float vy = (ys != 0.0f ? std::copysign(1.0f, ys)
                                      * std::pow(maxSpd, std::abs(ys) / 0.5f)
                                    : 0.0f);
        p.sx += float(vx * dt) / m_maxdist;
        p.sy += float(vy * dt) / m_maxdist;
        
        float x = *p.xpos + 100.0f * p.sx; // convert to percentage
        float y = *p.ypos + 100.0f * p.sy; // convert to percentage
        
        if (x < 0.0f)
        {
            p.sx = (x - *p.xpos) * 0.01f + 1.0f;
            
        }
        else if (x > 100.0f)
        {
            p.sx = (x - *p.xpos) * 0.01f - 1.0f;
        }
                        
        if (y < 0.0f)
        {
            p.sy = (y - *p.ypos) * 0.01f + 1.0f;
        }
        else if (y > 100.0f)
        {
            p.sy = (y - *p.ypos) * 0.01f - 1.0f;
        }

        x = p.sx + xa / m_maxdist; //meters normalized
        y = p.sy + ya / m_maxdist; //meters normalized

        p.x = *p.xpos + 100.0f * x; // convert to percentage
        p.y = *p.ypos + 100.0f * y; // convert to percentage
        
        if (x <   0.0f) { x += 100.0f; } else
        if (x > 100.0f) { x -= 100.0f; }
                        
        if (y <   0.0f) { y += 100.0f; } else
        if (y > 100.0f) { y -= 100.0f; }

     }
}

void WFSRendererParameters::setRendererId(const int rid)
{
    m_valueTreeState.state.setProperty("RendererId", juce::var(rid), nullptr);
}

int WFSRendererParameters::rendererId()
{
    auto rid = m_valueTreeState.state.getProperty("RendererId",
                                                  juce::var(RENDERER_ID));
    return (int)rid;
}

void WFSRendererParameters::setLayoutFilePath(const juce::File& lfp)
{
    setLayoutFilePath(lfp.getFullPathName());
}

void WFSRendererParameters::setLayoutFilePath(const juce::String& lfp)
{
    m_valueTreeState.state.setProperty("LayoutFilePath", lfp, nullptr);
}

juce::String WFSRendererParameters::layoutFilePath()
{
    auto lfp = m_valueTreeState.state.getProperty("LayoutFilePath");
    return lfp;
}

void WFSRendererParameters::setLastDirectory(const juce::File& ld)
{
    setLastDirectory(ld.getFullPathName());
}

void WFSRendererParameters::setLastDirectory(const juce::String& ld)
{
    m_valueTreeState.state.setProperty("LastDirectory", ld, nullptr);
}

juce::String WFSRendererParameters::lastDirectory()
{
    auto defDir = juce::File::getSpecialLocation(File::userDocumentsDirectory);
    auto ld = m_valueTreeState.state.getProperty("LastDirectory",
                                                 juce::var(defDir.getFullPathName()));
    return ld;
}

void WFSRendererParameters::setOscAddress(const juce::String& addr)
{
    m_valueTreeState.state.setProperty("OscAddress", addr, nullptr);
}

juce::String WFSRendererParameters::oscAddress()
{
    auto addr = m_valueTreeState.state.getProperty("OscAddress",
                                                   juce::var(DEFAULT_ADDRESS));
    return addr.toString();
}

void WFSRendererParameters::setOscPort(const int port)
{
    setOscPort(juce::String(port));
}

void WFSRendererParameters::setOscPort(const juce::String& port)
{
    m_valueTreeState.state.setProperty("OscPort", port, nullptr);
}

int WFSRendererParameters::oscPort()
{
    auto s = m_valueTreeState.state.getProperty("OscPort",
                                                juce::var(DEFAULT_PORT_STRING)).toString();
    return s.getIntValue();
}

void WFSRendererParameters::setLayoutRotated(const bool isRotated)
{
    m_valueTreeState.state.setProperty("LayoutRotated", juce::var(isRotated), nullptr);
}

bool WFSRendererParameters::isLayoutRotated()
{
    auto val = m_valueTreeState.state.getProperty("LayoutRotated", juce::var(false));
    return (bool)val;
}


bool WFSRendererParameters::save(const juce::File& file)
{
    std::unique_ptr<XmlElement> xml(m_valueTreeState.copyState().createXml());
    return xml->writeTo(file);
}

bool WFSRendererParameters::load(const juce::File& file)
{
    juce::XmlDocument doc(file);
    std::unique_ptr<XmlElement> xml(doc.getDocumentElement());
    
    if (! xml)
    {
        return false;
    }
    m_valueTreeState.replaceState(juce::ValueTree::fromXml(*xml));
    return true;
}
