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


// --------------------------------------------------------------------------------


#include <math.h>
#include "atimeref.h"


bool  Atimetab::_init = true;
int   Atimetab::_ff [6] = { FR0, FR1, FR2, FR3, FR4, FR5 };
float Atimetab::_st [STLEN];


Atimetab::Atimetab (void)
{
    int   i;
    float s;

    if (_init)
    {
	_init = false;
	for (i = 0; i < STLEN / 2; i++)
	{
	    s = sinf (6.2831853f * i / STLEN);
	    _st [i] = s;
	    _st [i + STLEN / 2] = -s;
	}
    }
}



Atimetx::Atimetx (void) :
    _ampl (0.01f)
{
    reset ();
}


void Atimetx::reset (void)
{
    int  i;

    _count = 0;
    for (i = 0; i < 6; i++) _pp [i] = 0;
}


void Atimetx::process (int nframes, float *out)
{
    int           i, j;
    float         v;

    _count += nframes;
    while (nframes--)
    {
        v = 0.0f;
	for (i = 0; i < 6; i++)
	{
	    j = _pp [i] >> SHIFT;
	    _pp [i] += _ff [i];
	    v += sint (j);
	} 
	*out++ = _ampl * v;
    }
}



Atimerx::Atimerx (void)
{
    reset ();
}


void Atimerx::reset (void)
{
    int         i;
    Atimefreq   *F;

    for (i = 0, F = _freq; i < 6; i++, F++)
    {
	F->_pp = 0;
	F->_xa = F->_ya = 0;
	F->_xf = F->_yf = 0;
    }
    _state = 0;
    _count = 0;
    _phase = 0;
    _cnt1  = 0;
    _cnt2  = 0;
}


void Atimerx::process (int nframes, float *inp)
{
    int         i, j;
    float       v;
    Atimefreq   *F;

    _count += nframes;
    while (nframes--)
    {
	v = *inp++;
	for (i = 0, F = _freq; i < 6; i++, F++)
	{
	    j = F->_pp >> SHIFT;
	    F->_pp += _ff [i];
	    F->_xa += sint (j) * v;
	    F->_ya += cost (j) * v;
	} 
	if (++_cnt1 == 64)
	{
	    for (i = 0, F = _freq; i < 6; i++, F++)
	    {
		F->_xf += 1e-2f * (F->_xa - F->_xf) + 1e-20f;
		F->_yf += 1e-2f * (F->_ya - F->_yf) + 1e-20f;
		F->_xa = F->_ya = 0;
	    }
            _cnt1 = 0;
	    if (++_cnt2 == 16)
	    {
	        j = resolve ();
                if (j < 0) _state--;
		else 
		{
		    if (_state < 1)
		    {
                        _phase = j;
			_state = 1;
		    }
		    else
		    {
			if (j == _phase) _state += 1;
			else             _state -= 3;
		    }
		}
		if (_state < 0) _state = 0;
		if (_state > 9) _state = 9;
		_cnt2 = 0;
	    }
	}
    }
}


int Atimerx::resolve (void)
{
    int         i, j, m;
    float       a, p;
    Atimefreq   *F = _freq;

    if (hypotf (F->_xf, F->_yf) < 1e-3f) return -1;
    p = atan2f (F->_yf, F->_xf) / (float)(2 * M_PI);
    if (p > 0.5f) p -= 1;
    m = 1;
    for (i = 1; i < 6; i++)
    {
        F++;
        if (hypotf (F->_xf, F->_yf) < 1e-3f) return -2;
	a = atan2f (F->_yf, F->_xf) / (float)(2 * M_PI) - p * _ff [i] / FR0;
	a = DIV1 * (a - floorf (a));
	j = (int)(floorf (a + 0.5f));
	if (fabsf (a - j) > 0.3f) return -3;
	p += m * (j & (DIV1 - 1));
	m *= DIV1;
    }  
    if (p < 0) p += FR0;
    return (int)(DIV0 * p + 0.5f) & MMASK;
}
