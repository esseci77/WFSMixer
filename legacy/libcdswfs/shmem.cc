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


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "shmem.h"



Shmem::Shmem (const char *name, size_t size, bool create) :
    _data (0), _size (0)
{
    int fd;

    umask (2);
    if (name && *name && size)
    {
#ifdef __APPLE__
        if (create) fd = shm_open (name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG );
#else
        if (create) fd = shm_open (name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG );
#endif
        else        fd = shm_open (name, O_RDWR, 0);
        
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
}


Shmem::~Shmem (void)
{
    if (_data) munmap (_data, _size);
}
