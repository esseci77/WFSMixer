
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


#include <assert.h>
#include "filter.h"


Filter::Filter (void) :
    _w1 (0),
    _w2 (0),
    _w3 (0),
    _w4 (0)
{
    reset ();
}


Filter::~Filter (void)
{
} 


void Filter::init (float fsam, float fmax, float flow)
{
    _w1 = 6.283f * fmax / fsam;
    _w2 = _w1 / 4;
    _w3 = _w2 / 4;
    _w4 = 6.283f * flow / fsam;
}


void Filter::reset (void)
{
    _z1 = _z2 = _z3 = _z4 = 0;
}


void Filter::process (int nfram,const float *ip, float *op1, float *op2)
{
    float x, z1, z2, z3, z4;

    z1 = _z1;
    z2 = _z2;
    z3 = _z3;
    z4 = _z4;
    while (nfram--)
    {
        x = *ip++ + 1e-8f;
        z1 += _w1 * (x - z1);
        z2 += _w2 * (x - z2);
        z3 += _w3 * (x - z3);
        x -= z1 / 2 + z2 / 4 + z3 / 8; 
        z4 += _w4 * (x - z4);
        *op1++ = x;
        *op2++ = z4;
    }
    _z1 = z1;
    _z2 = z2;
    _z3 = z3;
    _z4 = z4;
}


