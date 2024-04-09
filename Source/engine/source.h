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


#ifndef __SOURCE_H
#define __SOURCE_H

//#ifdef __APPLE__
//#include <libcdswfs/messages.h>
/*
#else
#include <cdswfs/messages.h>
*/
//#endif

#include "atimeref.h"
#include "params.h"
#include "filter.h"
#include "wfslayout.h"
#include <sstream>
#include <vector>

// --------------------------------------------------------[ from libcdswfs ]---
struct MD_header
{
    uint32_t     _mtype;
    uint32_t     _flags;
    uint32_t     _timer;
    uint32_t     _nsrce;
};



struct MD_source
{
    uint32_t     _mode;
    float        _xpos;
    float        _ypos;
    float        _gain;
};


struct Modeldata
{
    MD_header    _header;
    MD_source    _sources [64];
};

// -----------------------------------------------------------------------------

class State
{
public:

    State (void) : _delay (0), _gainA (0), _gainB (0) {}

    float  _delay;
    float  _gainA;
    float  _gainB;
};


class Source
{
public:

    Source (void);
    Source (const int rendererId);
    ~Source (void);

    void init(const int rendererId);
    
    int params (const MD_header *header, const MD_source *source);
    int render (const float   *psrce,
  	            float  *const *pchan,
 	            int      size,
	            int      time,
                int      solo);

    static float       _fsamp;
    static int         _fsize;
    static int         _delta;
    static int         _modul;
    static int         _noutp;
    static WFSlayout  *_layout;
    
    static std::vector<int> _cindex;
    
    void hold (float *const *pchan, int opind, int nfram, int solo);
    void fade (float *const *pchan, int opind, int nfram, int solo);
    void ipol (float *const *pchan, int opind, int nfram, int solo);
    
    int diff (int a, int b)
    {
        int d  = a - b;
        if (d >= _modul / 2) return d - _modul;
        if (-d > _modul / 2) return d + _modul;
        return d;
    }
    
    enum { IDLE, HOLD, FADE, IPOL };
    
    Parlist   _parlist;
    Filter    _filter;
    int       _dsize;     // delay line size
    int       _delind;    // delay line index
    State     _states [MAX_OUTPUTS];
    Srcpars  *_pars0;     // previous parameters
    Srcpars  *_pars1;     // current parameters
    int       _count;     // remaining samples for current mode
    int       _rmode;     // current render mode
    
    std::vector<float> _delayA; // delay lines
    std::vector<float> _delayB;
};


#endif
