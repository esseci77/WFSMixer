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


#ifndef __WFSARRAY_H
#define __WFSARRAY_H


#include <JuceHeader.h>
#include "AppConfig.h"

struct WFSchan
{
    float   _xpos;
    float   _ypos;
    int     _host;
    int     _port;
    float   _xnorm;
    float   _ynorm;
    float   _gcorr;
};

struct WFSsegm
{
    float x, y, ux, uy;
};

struct WFSRoomCorner
{
    float x = 0.0;
    float y = 0.0;
};

class WFSlayout
{
public:

    WFSlayout (const juce::File& configFile);
    virtual ~WFSlayout (void);

    unsigned int    nchannel (void) const { return _nchannel; }
    const WFSchan  *chandata (void) const { return _chandata.data(); }
    float           dsecsrce (void) const { return _dsecsrce; }
    float           maxdist  (void) const { return _maxdist;  }
    float           adddist  (void) const { return _adddist;  }
    float   distance (float x, float y);
    float   centered (float x, float y);
    void calc_normal (void);
    void parse(const juce::File& configFile);
    void initialise ();
    void addsegment (int i, int j);
    
    bool isOk() const;
    juce::String error();
    const juce::String& configFilePath() const { return _configFilePath; }
    
    const std::vector<WFSRoomCorner>& roomCorners() const { return _roomCorners; }
    
protected:
    unsigned int   _nchannel = 0;
    unsigned int   _nsegment = 0;
    float          _dsecsrce = 0.0;
    float          _maxdist  = 0.0;
    float          _adddist  = 0.0;
    
    std::vector<WFSchan>       _chandata;
    std::vector<WFSsegm>       _segmlist;
    std::vector<WFSRoomCorner> _roomCorners;
    
    juce::String _configFilePath;
    juce::String _error;
};

#endif
