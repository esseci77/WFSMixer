/*
    Copyright (C) 2007-2009 Fons Adriaensen <fons@kokkinizita.net>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <math.h>
#include "wfslayout.h"


WFSlayout::WFSlayout (const juce::File& configFile)
{
    parse(configFile);
    calc_normal();
    initialise ();
}

WFSlayout::~WFSlayout (void)
{
   // delete [] _chandata;
}

void WFSlayout::calc_normal (void)
{
    int    i, j, k;
    float  dx, dy, m;

    for (i = 0; i < (int) _nchannel; i++)
    {
        j = i + 1;
        k = i - 1;

        if (k < 0) k += _nchannel;
        if (j == (int) _nchannel) j = 0;

        dx = _chandata [j]._xpos - _chandata [k]._xpos;
        dy = _chandata [j]._ypos - _chandata [k]._ypos;
        m = hypotf (dx, dy);
        _chandata [i]._xnorm =  dy / m;
        _chandata [i]._ynorm = -dx / m;
        _chandata [i]._gcorr = 1;
    }
}

void WFSlayout::parse(const juce::File& configFile)
{
    XmlDocument doc (File::getCurrentWorkingDirectory().getChildFile (configFile.getFullPathName()));
    auto mainElement = doc.getDocumentElement();
    
    if (! mainElement)
    {
        _error = doc.getLastParseError();
        return;
    }
    printf("Loading configuration\n");
    
    if (mainElement->hasTagName ("WFSLayout"))
    {
        _dsecsrce = mainElement->getStringAttribute("speakerDistance").getFloatValue();
        _maxdist  = mainElement->getStringAttribute("maxDistance").getFloatValue();
        _adddist  = mainElement->getStringAttribute("addDistance").getFloatValue();

        for (auto* e : mainElement->getChildIterator())
        {
            if (e->hasTagName ("chanlist"))
            {
                _nchannel = e->getIntAttribute("nchannel");
                _chandata.resize(_nchannel);
                int n = 0;
                
                for (auto* echild : e->getChildWithTagNameIterator("channel"))
                {
                    float xpos = echild->getStringAttribute ("xpos").getFloatValue();
                    float ypos = echild->getStringAttribute ("ypos").getFloatValue();
                    int   port = echild->getIntAttribute ("port");
#if MAX_OUTPUTS > 64
                    int   host = 1;
#else
                    int   host = echild->getIntAttribute ("host");
#endif
                    _chandata[n]._xpos = xpos;
                    _chandata[n]._ypos = ypos;
                    _chandata[n]._port = port;
                    _chandata[n]._host = host;
                    n++;
                }
            }
            else if (e->hasTagName ("segmlist"))
            {
                _nsegment = e->getIntAttribute("nsegment");
                _segmlist.resize(_nsegment);
                int n = 0;
                
                for (auto* echild : e->getChildWithTagNameIterator("segment"))
                {
                    int channelRef = echild->getIntAttribute("channelRef");
                    addsegment(n,channelRef);
                    n++;
                }
            }
            else if (e->hasTagName("room"))
            {
                auto nwalls = e->getIntAttribute("nwalls");
                _roomCorners.resize(nwalls);
                int n = 0;
                
                for (auto* echild : e->getChildWithTagNameIterator("wall"))
                {
                    _roomCorners[n].x = (float)echild->getDoubleAttribute("x");
                    _roomCorners[n].y = (float)echild->getDoubleAttribute("y");
                    n++;
                }
            }
        }
    }
    _configFilePath = configFile.getFullPathName();
}

void WFSlayout::initialise ()
{
    int    i1, i2;
    float  dx, dy, m;
    
    for (i1 = 0; i1 < (int)_nsegment; i1++)
    {
        i2 = i1 + 1;
        if (i2 == (int)_nsegment) i2 = 0;
        dx = _segmlist [i2].x - _segmlist [i1].x;   //[gc] leg1, x component of distance
        dy = _segmlist [i2].y - _segmlist [i1].y;   //[gc] leg2, y component of distance
        m = hypotf (dx, dy);                        //[gc] hypotenuse, joining line between 2 points
        _segmlist [i1].ux = dx / m;                 //[gc] cos(angle) between hypotenuse and leg1
        _segmlist [i1].uy = dy / m;                 //[gc] sin(angle) between hypotenuse and leg1
    }
}

void WFSlayout::addsegment (int i, int j)
{
    _segmlist [i].x = _chandata [j]._xpos;
    _segmlist [i].y = _chandata [j]._ypos;
}

float WFSlayout::distance (float x, float y)
{
    
    int    i, i1, i2 = 0;
    float  dx, dy, m, dx1 = 0.0f, dy1 = 0.0f, m1;
    float  s1, s2, c1, c2;
    
    m1 = 1e10f;
    for (i = 0; i < (int)_nsegment; i++) //finds the nearest segment to current position
    {
        dx = _segmlist [i].x - x;
        dy = _segmlist [i].y - y;
        m = hypotf (dx, dy);
        if (m  < m1)
        {
            dx1 = dx;
            dy1 = dy;
            m1 = m;
            i2 = i;
        }
    }
    i1 = i2 ? (i2 - 1) : _nsegment - 1; // if i2=0 then i1 is the last segment
    
    s1 = dx1 * _segmlist [i1].uy - dy1 * _segmlist [i1].ux;
    s2 = dx1 * _segmlist [i2].uy - dy1 * _segmlist [i2].ux;
    if ((s1 < 0) && (s2 < 0)) return (s1 > s2) ? s1 : s2;
    c1 = dx1 * _segmlist [i1].ux + dy1 * _segmlist [i1].uy;
    c2 = dx1 * _segmlist [i2].ux + dy1 * _segmlist [i2].uy;
    if (c2 < 0) return s2;
    if (c1 > 0) return s1;
    return m1;
}

float WFSlayout::centered (float x, float y)
{
    float d = 2 * hypotf (x, y);
    if (d > 1.0f) d = 1.0f;
    return 1.0f - d;
}

bool WFSlayout::isOk() const
{
    return _error.isEmpty();
}

juce::String WFSlayout::error()
{
    auto err = _error;
    _error.clear();
    return err;
}
