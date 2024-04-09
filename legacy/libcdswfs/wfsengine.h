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


#ifndef __WFSENGINE_H
#define __WFSENGINE_H


#ifndef __MESSAGES_H
#include "messages.h"
#endif

#ifndef __WFSLAYOUT_H
#include "wfslayout.h"
#endif


class WFSengine
{
public:

    WFSengine (void);
    virtual ~WFSengine (void);

    virtual void setup (int         fsamp,       // sample frequency
                        int         fsize,       // jack period size
                        int         delta,       // parameter period (frames)
                        int         modul,       // timer period (frames)
                        int         nsrce,       // number of sources
                        int         noutp,       // number of outputs
			            WFSlayout  *layout,      // layout object
                        int        *cindex) = 0; // channel index

    virtual void params (const Modeldata *mdata) = 0;

    virtual void render (float **psrce,
                         float **pchan,
                         int     size,
                         int     time,
                         int     solo) = 0;

private:

};


typedef WFSengine *WFSengine_cr (void);


#endif
