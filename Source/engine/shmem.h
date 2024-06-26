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


#ifndef __SHMEM_H
#define __SHMEM_H


class Shmem
{
public:

    Shmem (const char *name, size_t size, bool create);
    ~Shmem (void);

    void   *data (void) const { return _data; }
    size_t  size (void) const { return _size; }
  
#ifdef __APPLE__
    bool lock();
#endif
    
private:

    char           _name [64];
    void          *_data;
    size_t         _size;

#ifdef _MSC_VER
    juce::File _file;
    std::unique_ptr<juce::MemoryMappedFile> _mmf;
#endif
};


#endif
