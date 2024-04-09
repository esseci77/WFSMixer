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


#ifndef __WFSCONFIG_H
#define __WFSCONFIG_H



class WFSsource
{
public:

    int     _mode;
    float   _xpos;
    float   _ypos;
    float   _gain;
};


class WFSconfig
{
public:

    enum { MAXRENDER = 16, MAXSOURCE = 48 };
    enum { MM_STATIC, MM_SHMEM, MM_OSCCOM };

    WFSconfig (void);
    ~WFSconfig (void);

    void reset (void);
    int load (const char *conf);

    const char *start_command (void) const { return _start_command; }
    const char *layout_plugin (void) const { return _layout_plugin; }
    const char *engine_plugin (void) const { return _engine_plugin; }

    const char *master_addr  (void) const { return _master_addr; }
    const char *master_iface (void) const { return _master_iface; }
    const char *render_addr  (int k) const { return _render_addr  [k - 1]; }
    const char *render_iface (int k) const { return _render_iface [k - 1]; }

    const char *modelparam_mca_addr (void) const { return _modelparam_mca_addr; }
    int         modelparam_mca_port (void) const { return _modelparam_mca_port; }

    const char *monitoring_mca_addr (void) const { return _monitoring_mca_addr; }
    int         monitoring_mca_port (void) const { return _monitoring_mca_port; }

    int         master_osc_udp_port (void) const { return _master_osc_udp_port; }

    int master_mode (void) const { return _master_mode; }
    int audio_sources_min (void) const { return _sources_min; }
    int audio_sources_max (void) const { return _sources_max; }
    int audio_timesig (void) const { return _timesig; }
    int audio_testsig (void) const { return _testsig; }
    int param_period (void) const { return _period; }
    int param_offset (void) const { return _offset; }

    const char* audio_timesig_port (void) const { return _timesig_port; }
    
    const WFSsource *sources (void) { return _sources; }

    // [esseci]
    int audio_port_offset(const int k) const { return _audio_port_offset [k-1]; }
    const char * audio_sources_base_port_name() const { return _base_port_name; }
    
private:

    enum { NOERR, ERROR, COMM, ARGS, EXTRA };

    char        _start_command [256];
    char        _layout_plugin [256];
    char        _engine_plugin [256];
    char        _master_addr   [64];
    char        _master_iface  [16];
    char        _render_addr   [MAXRENDER][64];
    char        _render_iface  [MAXRENDER][16];

    char        _modelparam_mca_addr [64];
    int         _modelparam_mca_port;
    char        _monitoring_mca_addr [64];
    int         _monitoring_mca_port;
    int         _master_osc_udp_port;

    int         _master_mode;
    int         _sources_min;
    int         _sources_max;
    int         _timesig;
    int         _testsig;
    int         _period;
    int         _offset;

    WFSsource   _sources [MAXSOURCE];
    
    int         _audio_port_offset [MAXRENDER]; // [esseci]
    char        _base_port_name    [64];
    char        _timesig_port      [64];
};



#endif
