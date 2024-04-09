// -----------------------------------------------------------------------------------
//
//    Copyright (C) 2007-2013 Fons Adriaensen <fons@linuxaudio.org>
//    
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// -----------------------------------------------------------------------------------


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wfsconfig.h"

#ifdef __APPLE__
#include "osxbundleutils.h"
#endif


WFSconfig::WFSconfig (void)
{
    reset ();
}


WFSconfig::~WFSconfig (void)
{
}


void WFSconfig::reset (void)
{
    int i;

    _start_command [0] = 0;
    _layout_plugin [0] = 0;
    _engine_plugin [0] = 0;
    _master_addr  [0] = 0;
    _master_iface [0] = 0;

    for (i = 0; i < MAXRENDER; i++)
    {
        _render_addr  [i][0] = 0;
        _render_iface [i][0] = 0;
    }

    _modelparam_mca_addr [0] = 0;
    _modelparam_mca_port = 0;
    _monitoring_mca_addr [0] = 0;
    _monitoring_mca_port = 0;
    _master_osc_udp_port = 0;

    _master_mode = 0;
    _sources_min = 0;
    _sources_max = 0;
    _timesig = 0;
    _testsig = 0;
    _period = 1024;
    _offset = 0;

    for (i = 0; i < MAXSOURCE; i++)
    {
	_sources [i]._mode = 0;
	_sources [i]._xpos = 0;
	_sources [i]._ypos = 0;
	_sources [i]._gain = 0;
    }
    
    _timesig_port [0] = 0;
}


int WFSconfig::load (const char *conf)
{
    FILE          *F;
    char          *p, *q;
    char          buff [1024];
    int           line, stat, n;
    int           i1, i2;
    float         f1, f2, f3;
    char          s1 [256];
    char          s2 [256];

    if (! (F = fopen (conf, "r"))) 
    {
	fprintf (stderr, "Can't read '%s'.\n", conf);
        return 1;
    } 

    printf("Loading configuration: %s\n", conf);
    
    stat = 0;
    line = 0;

    while (! stat && fgets (buff, 1024, F))
    {
        line++;
        p = buff; 
        while (isspace (*p)) p++;
        if (*p == '#' || *p < ' ') continue;
        q = p;
        while ((*q >= ' ') && !isspace (*q)) q++;
        *q++ = 0;   
        while ((*q >= ' ') && isspace (*q)) q++;

        stat = 0;
        if (!strcmp (p, "/cdswfs"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                if (i1 != 1) 
                {
                    fprintf (stderr, "Line %d: unknown version %d.\n", line, i1);
                    stat = ERROR;
                }
            }
        }
        else if (!strcmp (p, "/command/start"))
        {
            if (sscanf (q, "%s%n", _start_command, &n) != 1) stat = ARGS;
            else q += n;
        }
        else if (!strcmp (p, "/plugin/layout"))
        {
#ifdef __APPLE__
            int rv = sscanf (q, "%s%n", buff, &n);
            
            WfsMacBundle* bundle = WfsMacBundle::sharedInstance();
            const char* path = bundle->findPluginPath(buff);
            
            if (path)
            {
                strcpy(_layout_plugin, path);
                delete [] path;
            }
            
            if (rv != 1) stat = ARGS;
#else
            if (sscanf (q, "%s%n", _layout_plugin, &n) != 1) stat = ARGS;
#endif
            else q += n;
            
             printf("scanf rval: %d (%s)\n", rv, buff);
        }
        else if (!strcmp (p, "/plugin/engine"))
        {
#ifdef __APPLE__
            int rv = sscanf (q, "%s%n", buff, &n);
            
            WfsMacBundle* bundle = WfsMacBundle::sharedInstance();
            const char* path = bundle->findPluginPath(buff);
            
            if (path)
            {
                strcpy(_engine_plugin, path);
                delete [] path;
            }
            
            if (rv != 1) stat = ARGS;
#else
            if (sscanf (q, "%s%n", _engine_plugin, &n) != 1) stat = ARGS;
#endif
            else q += n;
        }
        else if (!strcmp (p, "/master/addr"))
        {
            
            if (sscanf (q, "%s%s%n", s1, s2, &n) != 2) stat = ARGS;
            else
            {
            q += n;
            s1 [63] = 0;
            s2 [15] = 0;
            strcpy (_master_addr,  s1);
            strcpy (_master_iface, s2);
            }
        }
        else if (!strcmp (p, "/render/addr"))
        {
            if (sscanf (q, "%d%s%s%n", &i1, s1, s2, &n) != 3) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1) || (i1 > MAXRENDER))
                {
                    fprintf (stderr, "Line %d: illegal renderer number %d.\n", line, i1);
                    stat = ERROR;
                }		    
                else
                {
                    s1 [63] = 0;
                    s2 [15] = 0;
                    strcpy (_render_addr  [i1 - 1], s1);
                    strcpy (_render_iface [i1 - 1], s2);
                }
            }
        }
        else if (!strcmp (p, "/modelparam/mca/addr"))
        {
            if (sscanf (q, "%s%n", s1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                s1 [63] = 0;
                strcpy (_modelparam_mca_addr, s1);
            }
        }
        else if (!strcmp (p, "/modelparam/mca/port"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1000) || (i1 > 65535))
                {
                    fprintf (stderr, "Line %d: illegal IP port %d.\n", line, i1);
                    stat = ERROR;
                }
                else _modelparam_mca_port = i1;
            }
        }
        else if (!strcmp (p, "/monitoring/mca/addr"))
        {
            if (sscanf (q, "%s%n", s1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                s1 [63] = 0;
                strcpy (_monitoring_mca_addr, s1);
            }
        }
        else if (!strcmp (p, "/monitoring/mca/port"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1000) || (i1 > 65535))
                {
                    fprintf (stderr, "Line %d: illegal IP port %d.\n", line, i1);
                    stat = ERROR;
                }
                else _monitoring_mca_port = i1;
            }
        }
        else if (!strcmp (p, "/master/osc/udp/port"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1000) || (i1 > 65535))
                {
                    fprintf (stderr, "Line %d: illegal IP port %d.\n", line, i1);
                    stat = ERROR;
                }
                else _master_osc_udp_port = i1;
            }
        }
        else if (!strcmp (p, "/master/mode/static"))
        {
            _master_mode = MM_STATIC;
        }
        else if (!strcmp (p, "/master/mode/shmem"))
        {
            _master_mode = MM_SHMEM;
        }
        else if (!strcmp (p, "/master/mode/osccom"))
        {
            _master_mode = MM_OSCCOM;
        }
        else if (!strcmp (p, "/audio/sources"))
        {
            if (sscanf (q, "%d%d%n", &i1, &i2, &n) != 2) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1) || (i1 > i2) || (i2 > MAXSOURCE))
                {
                    fprintf (stderr, "Line %d: illegal source channel(s) %d..%d.\n", line, i1, i2);
                    stat = ERROR;
                }
                else
                {
                    _sources_min = i1;
                    _sources_max = i2;
                }
            }
        }
        else if (!strcmp (p, "/audio/timesig"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1) || (i1 > 64))
                {
                    fprintf (stderr, "Line %d: illegal timesig channel %d.\n",
                             line, i1);
                    stat = ERROR;
                }
                else
                {
                    _timesig = i1;
                    sprintf(_timesig_port, "system:capture_%d", _timesig);
                }
            }
        }
        else if (!strcmp (p, "/audio/testsig"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1) || (i1 > 64))
                {
                    fprintf (stderr, "Line %d: illegal testsig channel %d.\n",
                             line, i1);
                    stat = ERROR;
                }
                else
                {
                    _testsig = i1;
                }
            }
        }
        else if (!strcmp (p, "/audio/timesig_port"))
        {
            if (sscanf (q, "%s%n", s1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                s1 [63] = 0;
                strcpy (_timesig_port, s1);
            }
        }
        else if (!strcmp (p, "/param/period"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                _period = i1;
            }
        }
        else if (!strcmp (p, "/param/offset"))
        {
            if (sscanf (q, "%d%n", &i1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                _offset = i1;
            }
        }
        else if (!strcmp (p, "/master/vsource"))
        {
            if (sscanf (q, "%d%d%f%f%f%n", &i1, &i2, &f1, &f2, &f3, &n) != 5) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1) || (i1 > MAXSOURCE))
                {
                    fprintf (stderr, "Line %d: illegal source channel(s) %d.\n", line, i1);
                    stat = ERROR;
                }
                else
                {
                    i1--;
                    _sources [i1]._mode = i2;
                    _sources [i1]._xpos = f1;
                    _sources [i1]._ypos = f2;
                            _sources [i1]._gain = (f3 < -80.0f) ? 0 : powf (10.0f, 0.05f * f3);
                }
            }
        }
// -------------------------------------------------------------------- [esseci]
        else if (!strcmp (p, "/audio/portoffset"))
        {
            if (sscanf (q, "%d%d%n", &i1, &i2, &n) != 2) stat = ARGS;
            else
            {
                q += n;
                if ((i1 < 1) || (i1 > MAXRENDER))
                {
                    fprintf (stderr, "Line %d: illegal renderer number %d.\n", line, i1);
                    stat = ERROR;
                }
                else
                {
                    _audio_port_offset [i1 - 1] = i2;
                }
            }
        }
        else if (!strcmp (p, "/audio/sources/baseportname"))
        {
            if (sscanf (q, "%s%n", s1, &n) != 1) stat = ARGS;
            else
            {
                q += n;
                s1 [63] = 0;
                strcpy (_base_port_name, s1);
            }
        }
        
// --------------------------------------------------------------------- to here
        else stat = COMM;

        if (! stat)
        {
            while (isspace (*q)) q++;
            if (*q >= ' ') stat = EXTRA;
        }		     

        switch (stat)
        {
            case COMM:
            fprintf (stderr, "Line %d: unknown command '%s'\n", line, p);   
                break;
            case ARGS:
            fprintf (stderr, "Line %d: missing arguments in '%s' command\n", line, p);   
                break;
            case EXTRA:
            fprintf (stderr, "Line %d: extra arguments in '%s' command\n", line, p);   
                break;
        }
    }

    fclose (F);
    return stat;
}



#ifdef UNITTEST


int main (int ac, char *av [])
{
    int        r, i;
    WFSconfig  C;

    if (ac < 2) return 1;
    
    r = C.load (av [1]);
    printf ("load returned %d\n", r);

    printf ("%s %s\n", C.master_addr (), C.master_iface ());
    for (i = 1; i <= MAXRENDER; i++) printf ("%s %s\n", C.render_addr (i), C.render_iface (i));

    return 0;
}


#endif

