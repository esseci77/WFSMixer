/*
    Copyright (C) 2008 Fons Adriaensen
    
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

#ifdef _MSC_VER
# include <JuceHeader.h>

#else
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <errno.h>

#endif

#include "shmem.h"


Shmem::Shmem (const char *name, size_t size, bool create) :
    _data (0), _size (0)
{
#ifdef _MSC_VER
    auto tmpDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory);
    _file = tmpDir.getFullPathName()
            + juce::File::getSeparatorString()
            + juce::String(name);

    if (create || ! _file.exists())
    {
        if (_file.exists())
        {
            _file.deleteFile();
        }
        _file.create();

        juce::MemoryBlock dataBlock(size, true);
        _file.appendData(dataBlock.getData(), size);        
    }
    _mmf.reset(new juce::MemoryMappedFile(_file, juce::MemoryMappedFile::AccessMode::readWrite));
#else
    // linux && macOS
    int fd;

    umask (2);
    if (name && *name && size)
    {
        strncpy(_name, name, 63);
        
        int options = O_RDWR | O_CREAT;
#ifndef __APPLE__
        options |= O_TRUNC;
#endif
        if (create) fd = shm_open (_name, options, S_IRWXU | S_IRWXG );
        else        fd = shm_open (_name, O_RDWR, 0);
        
        if (fd < 0)
        {
            perror ("shm_open");
            return;
        }
        
        if (create) ftruncate (fd, size);
        
        _size = size; 
        _data = mmap (0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        if (_data == MAP_FAILED)
        {
            perror ("mmap");
            _data = 0;
            _size = 0;
        }
        close (fd);
    }
#endif
}

Shmem::~Shmem (void)
{
#ifdef _MSC_VER
    _mmf = nullptr;    
    _file.deleteFile();
#else
    if (_data) munmap (_data, _size);
    shm_unlink(_name);
#endif
}

#ifdef __APPLE__

bool Shmem::lock()
{
    if (mlock(_data, _size) != 0)
    {
        fprintf(stderr, "Error locking shared memory (%s): %s.",
                _name, strerror(errno));
        return false;
    }
    return true;
}

#endif