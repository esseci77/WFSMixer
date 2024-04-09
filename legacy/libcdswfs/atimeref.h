/*
    Copyright (C) 2006-2009 Fons Adriaensen <fons@kokkinizita.net>
    
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


#ifndef __ATIMEREF_H
#define __ATIMEREF_H


class Atimetab
{
private:

    enum
    {
	DIV0 =    32,
	DIV1 =     8,
	FR0  = 32768, 
	FR1  =  4096,
	FR2  =  8704,
	FR3  = 12352,
	FR4  = 16392,
	FR5  = 20481,
	MODUL  = FR0 * DIV0,
	MSIGN  = MODUL / 2,
	MMASK  = MODUL - 1,
	STLEN  = 256,
	TMASK  = STLEN - 1,
	SHIFT  = 12   // LOG2 (MODUL / STLEN)  
    };

    friend class Atimetx;
    friend class Atimerx;

    Atimetab (void);

    float sint (int k) { return _st [k & TMASK]; }
    float cost (int k) { return _st [(k + STLEN / 4) & TMASK]; }

    static bool    _init;
    static int     _ff [6];
    static float   _st [STLEN];
};


class Atimefreq
{
private:

    friend class Atimerx;

    int    _pp;
    float  _xa;
    float  _ya;
    float  _xf;
    float  _yf;
};


class Atimetx : private Atimetab
{
public:

    Atimetx (void);

    void process (int nframes, float *out);
    void reset (void);
    int  modul (void) const { return MODUL; }
    int  timer (int offs = 0) const { return (_count + offs) & MMASK; }
    int  diff (int time) const
    {
	int d = time - _count;
	return (d & MSIGN) ? (d | ~MMASK) : (d & MMASK);
    }

private:

    int            _count;
    float          _ampl;
    int            _pp [6];
};


class Atimerx : private Atimetab
{
public:

    Atimerx (void);

    void process (int nframes, float *out);
    bool synced (void) const { return (_state >= 3); }
    void reset (void);
    int  modul (void) const { return MODUL; }
    int  timer (int offs = 0) const { return (_count + _phase + offs) & MMASK; }
    int  state (void) { return _state; }
    int  diff (int time) const
    {
	int d = time - _count - _phase;
	return (d & MSIGN) ? (d | ~MMASK) : (d & MMASK);
    }

private:

    int resolve (void);

    int         _state;
    int         _count;
    int         _phase;
    int         _cnt1;
    int         _cnt2;
    Atimefreq   _freq [6];
};





#endif
