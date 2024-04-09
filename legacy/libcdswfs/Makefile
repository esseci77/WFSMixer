# -----------------------------------------------------------------------------------
#
#    Copyright (C) 2007-2013 Fons Adriaensen <fons@linuxaudio.org>
#    
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------------


PREFIX = /usr/local
SUFFIX := $(shell uname -m | sed -e 's/^unknown/$//' -e 's/^i.86/$//' -e 's/^x86_64/$/64/')
LIBDIR = lib$(SUFFIX)
INCDIR = include/cdswfs
SHARED = share/cdswfs
CPPFLAGS += -O3 -Wall -MMD -MP 
CPPFLAGS += -march=native

MAJVERS = 2
MINVERS = 0.0
VERSION = $(MAJVERS).$(MINVERS)

all:	libcdswfs-system.so mk_meter


CDSWFSSYSTEM_SO = libcdswfs-system.so
CDSWFSSYSTEM_MAJ = $(CDSWFSSYSTEM_SO).$(MAJVERS)
CDSWFSSYSTEM_MIN = $(CDSWFSSYSTEM_MAJ).$(MINVERS)
CDSWFSSYSTEM_DEP = 
CDSWFSSYSTEM_O = wfsconfig.o wfslayout.o wfsengine.o atimeref.o shmem.o
CDSWFSSYSTEM_H = wfsconfig.h wfslayout.h wfsengine.h atimeref.h shmem.h messages.h 

libcdswfs-system.so:	CPPFLAGS += -fpic -D_REENTRANT
libcdswfs-system.so:	LDFLAGS += -shared
libcdswfs-system.so:	$(CDSWFSSYSTEM_O)
	g++ $(LDFLAGS) -Wl,-soname,$(CDSWFSSYSTEM_MAJ) -o $(CDSWFSSYSTEM_MIN) $(CDSWFSSYSTEM_O) $(CDSWFSSYSTEM_DEP)



MK_METER_O = mk_meter.o
mk_meter.o:	CPPFLAGS += -I/usr/include/cairo
mk_meter:	LDLIBS += -lcairo
mk_meter:	$(MK_METER_O)
	$(LINK.cc) -o $@ $(MK_METER_O) $(LDLIBS)


install:	$(CDSWFSSYSTEM_SO)
	install -d $(PREFIX)/$(LIBDIR)
	install -d $(PREFIX)/$(INCDIR)
	rm -f $(PREFIX)/$(INCDIR)/*
	install -m 644 $(CDSWFSSYSTEM_H) $(PREFIX)/$(INCDIR)
	install -m 644 $(CDSWFSSYSTEM_MIN) $(PREFIX)/$(LIBDIR)
	ldconfig
	ln -sf $(CDSWFSSYSTEM_MIN) $(PREFIX)/$(LIBDIR)/$(CDSWFSSYSTEM_SO)
	mk_meter
	install -d $(PREFIX)/$(SHARED)
	install -m 644 *.png $(PREFIX)/$(SHARED)

clean:
	/bin/rm -f *~ *.o *.a *.d *.so $(CDSWFSSYSTEM_MIN) mk_meter *.png
