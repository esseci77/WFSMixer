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


#ifndef __PARAMS_H
#define __PARAMS_H


#include "global.h"
#include "JuceHeader.h"

class Outpars
{
public:

    int       _delay;
    float     _gainA;
    float     _gainB;
};


class Srcpars
{
public:

    int       _flags;
    int       _timer;
    float     _xpos;
    float     _ypos;
    float     _gain;
    Outpars   _outpars [MAX_OUTPUTS];
};


class Parlist
{
public:

    Parlist (int size);
    ~Parlist (void);

    void reset (void) { _nwr = _nrd = 0; }

    int       wr_avail (void) const { return _size - _nwr + _nrd; } 
    void      wr_commit (void) { _nwr++; }
    Srcpars  *wr_ptr (void) { return _data + (_nwr & _mask); }

    int       rd_avail (void) const { return _nwr - _nrd; } 
    void      rd_commit (void) { _nrd++; }
    Srcpars  *rd_ptr (void) { return _data + (_nrd & _mask); }

private:

    Srcpars * _data;
    int       _size;
    int       _mask;
    int       _nwr;
    int       _nrd;
};


#endif
