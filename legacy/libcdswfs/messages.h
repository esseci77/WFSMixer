/*
    Copyright (C) 2008 Fons Adriaensen <fons@kokkinizita.net>
    
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


//#include <inttypes.h>
#include <cstdint>

#ifndef __MESSAGES_H
#define __MESSAGES_H


#define MODE_MUTE 0
#define MODE_TEST 1
#define MODE_REND 2


#define STAT_JACK 0x0001
#define STAT_SYNC 0x0002
#define STAT_PARS 0x0004
#define STAT_DMIN 0x0008
#define STAT_DMAX 0x0010


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



class Modeldata_list
{
public:


    Modeldata_list (unsigned int nitem) :
    _nitem (nitem)
    {
	_data = new Modeldata [nitem];
	reset ();
    }

    ~Modeldata_list (void)
    {
	delete[] _data;
    }

    void reset (void)
    {
        _count = _wr_ind = _rd_ind = 0;
    }  

    Modeldata *write_ptr (void) { return (_count == _nitem) ? 0 : _data + _wr_ind; }
    Modeldata *read_ptr (void)  { return (_count == 0)      ? 0 : _data + _rd_ind; }
    void write (void) { if (++_wr_ind == _nitem) _wr_ind = 0; _count++; }
    void read  (void) { if (++_rd_ind == _nitem) _rd_ind = 0; _count--; }

private:

    Modeldata    *_data;
    unsigned int  _nitem;
    unsigned int  _count;
    unsigned int  _wr_ind;
    unsigned int  _rd_ind;
};


#endif
