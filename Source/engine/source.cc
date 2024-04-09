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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "source.h"
#include "../PluginProcessor.h"

#if defined DEBUG  && RENDERER_ID > 1
#   define TRACE 1
#endif
// -----------------------------------------------------------------------------


float      Source::_fsamp = 0;
int        Source::_fsize = 0;
int        Source::_delta = 0;
int        Source::_modul = 0;
int        Source::_noutp = 0;
WFSlayout* Source::_layout = 0;
std::vector<int> Source::_cindex;

WFSRendererAudioProcessor* processor = nullptr;
float temp;

// -----------------------------------------------------------------------------

Source::Source () :
 _parlist (PARLIST_SIZE),
 _pars0 (0),
 _pars1 (0),
 _count (0),
 _rmode (IDLE)
{ }

Source::Source (const int rendererId) :
 _parlist (PARLIST_SIZE),
 _pars0 (0),
 _pars1 (0),
 _count (0),
 _rmode (IDLE)
{
    init(rendererId);
}

void Source::init(const int rendererId)
{
    _filter.init (_fsamp, 1700.0f, 50.0f);
    int k = (int)((_layout->maxdist () + _layout->adddist ()) * _fsamp / 343.0f);
    _delind = 0;
    _dsize = _fsize;
    while (_dsize < k) _dsize *= 2;
    _delayA.resize(_dsize + 1);
    _delayB.resize(_dsize + 1);
    
    /////////////////////////////////////////ADDED///////////////////////////////////////////
    _cindex.resize(MAX_OUTPUTS);
    
    const int  nchan = _layout->nchannel ();
    const WFSchan* A = _layout->chandata ();
    
    int i = 0;
    int j = 0;
    
    for (; i < nchan; i++, j++, A++)
    {
        if (A->_host == rendererId)
        {
#if DEBUG
            std::cout << "cindex[" << j << "]: " << i
                      << " xpos: " << A->_xpos
                      << " ypos: " << A->_ypos << std::endl;
#endif
            _cindex[j] = i;
        }
        
        if (j == MAX_OUTPUTS)
        {
            break;
        }
    }
    _noutp = j;
    
    while (j < MAX_OUTPUTS)
    {
        _cindex[j++] = -1;
    }

#ifdef TRACE
    printf("Render id: %5d, noutp: %5d\n", rendererId, _noutp);
#endif
    /////////////////////////////////////////ADDED///////////////////////////////////////////
}


Source::~Source (void)
{
    /*
    delete[] _delayA;
    delete[] _delayB;
    
    if (_cindex)
    {
        delete[] _cindex;
        _cindex = nullptr;
    }*/
}


// -----------------------------------------------------------------------------

int Source::params (const MD_header *header, const MD_source *source)
{
    int            i;
    float          dcn, dss, spm, add;
    float          xs1, ys1, xs2, ys2, xsu, ysu;
    float          dx, dy, d1, d2, mm, m1, m2;
    float          gs, gc, gg, gA1, gA2, gB1, gB2;
    float          r, u, v, v0, v1;
    bool           sint, sext;
    Srcpars        *S;
    Outpars        *P;
    const WFSchan  *A;
    
    //    if ((source->_mode & 1) == 0) return 0; //[gc] I guess that means: if it's not in solo mode
    if (_parlist.wr_avail () == 0)
    {
        return 1;
    }
    S = _parlist.wr_ptr ();
    S->_flags = 1; // source->_mode; [esseci: restore it...
    S->_timer = (header->_timer + _delta) & (_modul - 1); // = timertx + delta
    
    S->_xpos = xs1 = xs2 = source->_xpos;
    S->_ypos = ys1 = ys2 = source->_ypos;
    S->_gain = gs = 0.2f * source->_gain;
    
    d2 = d1 = hypotf (xs1, ys1) + 0.01f;
    gc = (d1 + 5.0f) / (d1 + 1.0f);
    xsu = xs1 / d1;
    ysu = ys1 / d1;
    
    add = _layout->adddist ();
    spm = _fsamp / 343.0f;
    
    dss = _layout->dsecsrce ();
    dcn = _layout->distance (xs1, ys1);
    sint = (dcn <  dss);
    sext = (dcn > -dss);
    
    u = r = 0;
    
    if (sint && sext)
    {
        u = dcn / dss;
        xs1 += (1 - u) * dss * xsu;
        ys1 += (1 - u) * dss * ysu;
        xs2 -= (1 + u) * dss * xsu;
        ys2 -= (1 + u) * dss * ysu;
        d2  -= (1 + u) * dss;
        r = (1 + u) / 2;
    }
    
    v0 = sint ? _layout->centered (xs2, ys2) : 0;
    v1 = 1 - v0;

    for (i = 0, P = S->_outpars; i < _noutp; i++, P++)
    {
        
        //    std::cout << "cindex[" << i << "]: " << _cindex[i] << std::endl;
        A = _layout->chandata () + _cindex [i];
        mm = hypotf (A->_xpos, A->_ypos);
        gg = gs * A->_gcorr;
        gA1 = gB1 = 0;
        
        if (sext)
        {
            dx = A->_xpos - xs1;
            dy = A->_ypos - ys1;
            d1 = hypotf (dx, dy);
            u = dx * A->_xnorm + dy * A->_ynorm;
            
            if (u > 0)
            {
                m2 = sqrtf (d1 * (d1 + mm));
                m1 = sqrtf (mm) * gc / m2;
                gA1 = u * m1 / d1;
                gB1 = gA1 / m2;
            }
            if (! sint)
            {
                P->_delay = (int)(spm * (add + d1) + 0.5f);
                P->_gainA = gg * gA1;
                P->_gainB = gg * gB1;
            }
        }
        gA2 = gB2 = 0;
        
        if (sint)
        {
            dx = A->_xpos - xs2;
            dy = A->_ypos - ys2;
            d2 = hypotf (dx, dy);
            u = dx * A->_xnorm + dy * A->_ynorm;
            
            if (u < 0)
            {
                v = v0 + v1 * (xsu * dx + ysu * dy) / d2;
                if (v > 0)
                {
                    m2 = sqrtf (d2 * (d2 + mm));
                    m1 = sqrtf (mm) * gc / m2;
                    gA2 = -u * sqrtf (v) * m1 / d2;
                    gB2 = gA2 / m2;
                }
            }
            if (! sext)
            {
                P->_delay = (int)(spm * (add - d2) + 0.5f);
                P->_gainA = gg * gA2;
                P->_gainB = gg * gB2;
            }
        }
        
        if (sint && sext)
        {
            P->_delay = (int)(spm * (add + r * d1  + (r - 1) * d2) + 0.5f);
            P->_gainA = gg * (r * gA1 + (1 - r) * gA2);
            P->_gainB = gg * (r * gB1 + (1 - r) * gB2);
        }
    }
    _parlist.wr_commit ();
    return 0;
}

// -----------------------------------------------------------------------------
static int64 __nsamples = 0;

int Source::render (const float   *psrce,
                    float  *const *pchan,
                    int      nfram,
                    int      timer,
                    int      solo)
{
    // Obtain pointers to data for performance
    float* pA = _delayA.data();
    float* pB = _delayB.data();
    
    // Apply filters and write to delay lines.
    _filter.process (nfram, psrce, pA + _delind, pB + _delind);
    
    if (_delind == 0)
    {
        _delayA [_dsize] = _delayA [0];
        _delayB [_dsize] = _delayB [0];
    }
    
#ifdef TRACE
    printf ("------- [%d %d]\n", RENDERER_ID, timer);
#endif
    int opind = 0;
    
    while (nfram)
    {
        if (_count)
        {
            //[gc] if it's last chunk, take all remaining samples (= _count)
            const int nf = (nfram < _count) ? nfram : _count;
            
            switch (_rmode)
            {
                case HOLD: hold (pchan, opind, nf, solo); break;
                case FADE: fade (pchan, opind, nf, solo); break;
                case IPOL: ipol (pchan, opind, nf, solo); break;
//[gc] IMPORTANT!
//                    Substituting the previous line with the following, we can prove that is ipol that is faulty:
//                    using ipol we hear a big zipping effect in playback when moving a source,
//                    while using hold (new values without interpolation) when moving a source,
//                    we can only hear very little clicks.
                    
//                case IPOL: hold (pchan, opind, nf, solo); break;
#ifdef TRACE
                default:
                    printf ("IDLE  count = %5d  nfram = %5d  opind = %5d\n",
                            _count, nfram, opind);
#endif
            }
            _count -= nf;
            
            if (_count == 0)
            {
                _pars0 = _pars1;
                _pars1 = nullptr;
                
                if (_pars0)
                {
                    _parlist.rd_commit ();
                }
            }
            nfram -= nf;
            timer += nf;
            opind += nf;
        }
        else if (_parlist.rd_avail () > 0)
        {
            _pars1 = _parlist.rd_ptr ();

            // [gc] it's just to be sure that we have at least 512 samples
            //      to process, nothing more
            int dt = diff (_pars1->_timer, timer);
            
#if MAX_OUTPUTS > 64  // then overwrite dt
            dt =  _delta/2;
//          dt =  nfram;    // [gc] should be the same as above
#endif
            
#if TRACE
            if (dt > _delta)
            {
                printf("dt > delta: %d = %d - %d\n", dt, _pars1->_timer, timer);
            }
#endif
            if (_pars0)
            {
                if (dt < _delta / 2)
                {
                    _parlist.rd_commit ();
                    _pars1 = nullptr;
                }
                else
                {
                    _count = dt;
                    _rmode = IPOL;
                    
                    if (std::hypotf ((_pars1->_xpos) - (_pars0->_xpos),
                                     (_pars1->_ypos) - (_pars0->_ypos)) < 0.001f)
                    {
                        //[gc] if position is considered unchanged
                        //[gc] then maybe just the gain is to interpolate (fade);
                        _rmode = FADE;
                        
                        if (    (0.99f * _pars1->_gain <= _pars0->_gain)
                             && (0.99f * _pars0->_gain <= _pars1->_gain))
                        {
                            //[gc] if even gain is considered unchanged
                            //[gc] then hold the samples with same position and gain
                            _rmode = HOLD;
                        }
#if DEBUG
                        else
                        {
                            printf("FADE chosen [%lld]: %f (%f)\n",
                                   __nsamples,
                                   _pars0->_gain, _pars1->_gain);
                        }
#endif
                    }
                }
            }
            else
            {
                if (dt > _delta)
                {
                    _count = dt - _delta;
                    _rmode = IDLE;
                    _pars1 = nullptr;
                }
                else if (dt < _delta / 2)
                {
                    _parlist.rd_commit ();
                    _pars1 = nullptr;
                }
                else
                {
                    _count = dt;
                    _rmode = FADE;
                }
            }
        }
        else if (_pars0)
        {
            //[gc] if there's nothing to read but previously something
            //     was happening -> fade out
            _count = _delta;
            _rmode = FADE;
        }
        else
        {
            _count = _delta;
            _rmode = IDLE;
        }
        
        __nsamples++;
        
    } // while
    
    // 	Update delay lines write index.
    _delind += _fsize;
    _delind &= (_dsize - 1);
    return 0;
}


// ----------------------------------------------
// Both delay and gain constant.
// ----------------------------------------------
//
void Source::hold (float *const *pchan, int opind, int nfram, int solo)
{
#ifdef TRACE
    printf ("HOLD  count = %5d  nfram = %5d  opind = %5d\n",
            _count, nfram, opind);
#endif
    
    int m  = _dsize - 1;
    float* pA = _delayA.data();
    float* pB = _delayB.data();
    
    // Loop over all outputs.
    State* S = _states;
    Outpars* P = _pars1->_outpars;
    
    for (int i=0; i < _noutp; i++)
    {
        // If output is enabled, calculate samples.
        if (pchan && ((solo < 0) || _cindex [i] == solo))
        {
            // Pointer into output buffer.
            float* wp  = pchan [i] + opind;

            // Fixed gain and delay values.
            const float gA = S->_gainA = P->_gainA;
            const float gB = S->_gainB = P->_gainB;
            const float dd = S->_delay = (float)P->_delay;
            int k = _delind + opind - (int)(dd);

            // Loop over all samples.
            for (int j = 0; j < nfram; j++, k++)
            {
                // Apply gains and add to output.
                k &= m;
                wp [j] += gA * pA [k] + gB * pB [k];
            }
        }
        S++;
        P++;
    }
}


// ----------------------------------------------
// Interpolate gain only, or fade up or down.
// ----------------------------------------------
//
void Source::fade (float *const *pchan, int opind, int nfram, int solo)
{
#ifdef TRACE
    printf ("FADE  count = %5d  nfram = %5d  opind = %5d\n",
            _count, nfram, opind);
#endif
    int     m = _dsize - 1;
    float   r = nfram / (float) _count;
    float dgA = 0.0;
    float dgB = 0.0;
    float* pA = _delayA.data();
    float* pB = _delayB.data();
    
    // Loop over all outputs.
    State*   S = _states;
    Outpars* P = _pars1 ? _pars1->_outpars : 0;
    
    for (int i = 0; i < _noutp; i++)
    {
        // Starting values for interpolation.
        float gA = S->_gainA;
        float gB = S->_gainB;
        
        // Differences for nfram samples.
        if (P)
        {
            dgA = r * (P->_gainA - gA);
            dgB = r * (P->_gainB - gB);
            S->_delay = (float)P->_delay;
        }
        else
        {
            dgA = -r * gA;
            dgB = -r * gB;
        }
        // Updata current state.
        S->_gainA = gA + dgA;
        S->_gainB = gB + dgB;
        
        // If output is enabled, calculate samples.
        if (pchan && ((solo < 0) || _cindex [i] == solo))
        {
            // Pointer into output buffer.
            float* wp  = pchan [i] + opind;
            
            // Per sample increments.
            dgA /= nfram;
            dgB /= nfram;
            
            // Find initial index into delay lines.
            int k = _delind + opind - (int)(S->_delay);
            
            // Loop over all samples.
            for (int j = 0; j < nfram; j++, k++)
            {
                // Apply gains and add to output.
                k &= m;
                wp [j] += gA * pA [k] + gB * pB [k];

                // Adjust paramters for next sample.
                gA += dgA;
                gB += dgB;
            }
        }
        S++;
        if (P) P++;
    }
}

// ----------------------------------------------
// Interpolate delay and gain, moving source.
// ----------------------------------------------
//
void Source::ipol (float *const *pchan, int opind, int nfram, int solo)
{
    int      j, k;
    float    gA, dgA, gB, dgB, t1, t2, dt, a, b;
    
#ifdef TRACE
    printf ("IPOL  count = %5d  nfram = %5d  opind = %5d\n", _count, nfram, opind);
#endif
    
    int    m  = _dsize - 1;
    float  r  = nfram / (float) _count;
    float* pA = _delayA.data();
    float* pB = _delayB.data();
    
    // Loop over all outputs.
    State*   S = _states;
    Outpars* P = _pars1->_outpars;
    
    for (int i = 0; i < _noutp; i++)
    {
        // Starting values for interpolation.
        t1 = S->_delay;
        gA = S->_gainA;
        gB = S->_gainB;
                
        // Differences for nfram samples.
        dt  = r * (P->_delay - t1);
        dgA = r * (P->_gainA - gA);
        dgB = r * (P->_gainB - gB);
                
        // Updata current state.
        S->_delay = t1 + dt;
        S->_gainA = gA + dgA;
        S->_gainB = gB + dgB;
        
        // If output is enabled, calculate samples.
        if (pchan && ((solo < 0) || _cindex [i] == solo))
        {
            // Pointer into output buffer.
            float* wp  = pchan [i] + opind;
            
            // Per sample increments.
            dt  /= nfram;
            dgA /= nfram;
            dgB /= nfram;
            
            // Split initial delay in fractional and
            // integer parts.
            k = (int)(floorf (t1));
            t1 -= k;
            t2 = 1 - t1;
            
            // Find initial index into delay lines.
            k = _delind + opind - k - 1;
            
            // Loop over all samples.
            for (j = 0; j < nfram; j++, k++)
            {
                // Interpolate in both delay lines,
                // apply gains and add to output.
                k &= m;
                a = t1 * pA [k] + t2 * pA [k + 1];
                b = t1 * pB [k] + t2 * pB [k + 1];
                wp [j] += gA * a + gB * b;
                
                // Adjust paramters for next sample.
                gA += dgA;
                gB += dgB;
                
                // Note: at least on Intel processors, updating
                // boht t1 and t2 and checking for overflow is
                // considerably faster than updating a single
                // float value and then finding the inter part
                // again for each sample, even when just using
                // a cast instead of floorf().
                // This method assumes that fabsf (dt) < 1.
                t1 += dt;
                t2 -= dt;
                if (t1 < 0)
                {
                    // Integer part of delay decrements.
                    t1 += 1;
                    t2 -= 1;
                    k++; // Index increments!
                }
                if (t2 < 0)
                {
                    // Integer part of delay increments.
                    t1 -= 1;
                    t2 += 1;
                    k--; // Index decrements!
                }
            }
        }
        S++;
        P++;
    }
    
}
//// ----------------------------------------------
//// Interpolate delay and gain, moving source.
//// ----------------------------------------------
////
//void Source::ipol (float **pchan, int opind, int nfram, int solo)
//{
//    int       j, k, m;
//    float    gA, dgA, gB, dgB, t1, t2, dt, a, b, r;
//    float    *wp, *pA, *pB;
//    State    *S;
//    Outpars  *P;
//    
//#ifdef TRACE
//    printf ("IPOL  count = %5d  nfram = %5d  opind = %5d\n", _count, nfram, opind);
//#endif
//    
//    m  = _dsize - 1;
//    r  = nfram / (float) _count;
//    pA = _delayA;
//    pB = _delayB;
//    
//    // Loop over all outputs.
//    S = _states;
//    P = _pars1->_outpars;
//    int i = 0;
//    for (; i < _noutp; i++)
//    {
//        // Starting values for interpolation.
//        t1 = S->_delay;
//        gA = S->_gainA;
//        gB = S->_gainB;
//        
//        // Differences for nfram samples.
//        dt  = r * (P->_delay - t1);
//        dgA = r * (P->_gainA - gA);
//        dgB = r * (P->_gainB - gB);
//        
//        // Updata current state.
//        S->_delay = t1 + dt;
//        S->_gainA = gA + dgA;
//        S->_gainB = gB + dgB;
//        
//        // If output is enabled, calculate samples.
//        if (pchan && ((solo < 0) || _cindex [i] == solo))
//        {
//            // Pointer into output buffer.
//            wp  = pchan [i] + opind;
//            
//            // Per sample increments.
//            dt  /= nfram;
//            dgA /= nfram;
//            dgB /= nfram;
//            
//            // Split initial delay in fractional and
//            // integer parts.
//            
//            k = (int)(floorf (t1));
//            t1 -= k;
//            t2 = 1 - t1;
//            
//            // Find initial index into delay lines.
//            k = _delind + opind - k - 1;
//            
//            // Loop over all samples.
//            for (j = 0; j < nfram; j++, k++)
//            {
//                // Interpolate in both delay lines,
//                // apply gains and add to output.
//                k &= m;
//                a = t1 * pA [k] + t2 * pA [k + 1];
//                b = t1 * pB [k] + t2 * pB [k + 1];
//                wp [j] += gA * a + gB * b;
//                
//                // Adjust paramters for next sample.
//                gA += dgA;
//                gB += dgB;
//                
//                // Note: at least on Intel processors, updating
//                // boht t1 and t2 and checking for overflow is
//                // considerably faster than updating a single
//                // float value and then finding the inter part
//                // again for each sample, even when just using
//                // a cast instead of floorf().
//                // This method assumes that fabsf (dt) < 1.
//                t1 += dt;
//                t2 -= dt;
//                if (t1 < 0 || t2<0)
//                {
//                    t1 -= floorf (t1);
//                    t2 = 1-t1;
//                }
//            }
//        }
//        S++;
//        P++;
//    }
//}
//



//EXPLAINATION IPOL
/*if (pchan && ((solo < 0) || _cindex [i] == solo))
 {
 // Pointer into output buffer.
 wp  = pchan [i] + opind;
 
 // Per sample increments.
 dt  /= nfram;
 dgA /= nfram;
 dgB /= nfram;
 
 // Split initial delay in fractional and
 // integer parts.
 
 k = (int)(floorf (t1));
 //            t1 -= k;
 
 
 // Find initial index into delay lines.
 k = _delind + opind - k - 1;
 
 // Loop over all samples.
 for (j = 0; j < nfram; j++, k++)
 {
 // Interpolate in both delay lines,
 // apply gains and add to output.
 k &= m;
 n = (int)(floorf (t1));
 t2 = t1-n;
 a = t2 * ( pA [k + 1] - pA [k]) + pA [k] ;
 b = t2 * ( pB [k + 1] - pB [k]) + pB [k] ;
 wp [j] += gA * a + gB * b;
 
 // Adjust paramters for next sample.
 gA += dgA;
 gB += dgB;
 
 // Note: at least on Intel processors, updating
 // boht t1 and t2 and checking for overflow is
 // considerably faster than updating a single
 // float value and then finding the inter part
 // again for each sample, even when just using
 // a cast instead of floorf().
 // This method assumes that fabsf (dt) < 1.
 t1 += dt;
 }
 }*/








// ------------------------------------------------------------------------------------
/*
 // ----------------------------------------------
 // Both delay and gain constant.
 // ----------------------------------------------
 //
 void Source::hold (float **pchan, int opind, int nfram, int solo)
 {
 int       j, k, m;
 float    gA, gB, dd;
 float    *wp, *pA, *pB;
 State    *S;
 Outpars  *P;
 
 #ifdef TRACE
 printf ("HOLD  count = %5d  nfram = %5d  opind = %5d\n", _count, nfram, opind);
 #endif
 
 m  = _dsize - 1;
 pA = _delayA;
 pB = _delayB;
 
 // Loop over all outputs.
 S = _states;
 P = _pars1->_outpars;
 int i=0;
 for (; i < _noutp; i++)
 {
	// If output is enabled, calculate samples.
	if (pchan && ((solo < 0) || _cindex [i] == solo))
	{
 // Pointer into output buffer.
 wp  = pchan [i] + opind;
 //#ifdef TRACE
 //        printf ("HOLD  ch=%3d count = %5d  nfram = %5d  opind = %5d\n",i, _count, nfram, opind);
 //#endif
 #ifdef TRACE
 //        printf ("chcoun %3d\n", i);
 #endif
 // Fixed gain and delay values.
 gA = S->_gainA = P->_gainA;
 gB = S->_gainB = P->_gainB;
 dd = S->_delay = P->_delay;
 k = _delind + opind - (int)(dd);
 
 // Loop over all samples.
 for (j = 0; j < nfram; j++, k++)
 {
 // Apply gains and add to output.
 k &= m;
 wp [j] += gA * pA [k] + gB * pB [k];
 }
	}
	S++;
 P++;
 }
 
 
 }
 
 
 // ----------------------------------------------
 // Interpolate gain only, or fade up or down.
 // ----------------------------------------------
 //
 void Source::fade (float **pchan, int opind, int nfram, int solo)
 {
 int       j, k, m;
 float    gA, dgA, gB, dgB, r;
 float    *wp, *pA, *pB;
 State    *S;
 Outpars  *P;
 
 #ifdef TRACE
 printf ("FADE  count = %5d  nfram = %5d  opind = %5d\n", _count, nfram, opind);
 #endif
 
 m  = _dsize - 1;
 r  = nfram / (float) _count;
 pA = _delayA;
 pB = _delayB;
 
 // Loop over all outputs.
 S = _states;
 P = _pars1 ? _pars1->_outpars : 0;
 int i = 0;
 for (; i < _noutp; i++)
 {
	// Starting values for interpolation.
 gA = S->_gainA;
 gB = S->_gainB;
 
 // Differences for nfram samples.
	if (P)
	{
 dgA = r * (P->_gainA - gA);
 dgB = r * (P->_gainB - gB);
 S->_delay = P->_delay;
	}
	else
	{
 dgA = -r * gA;
 dgB = -r * gB;
	}
	// Updata current state.
 S->_gainA = gA + dgA;
 S->_gainB = gB + dgB;
 
	// If output is enabled, calculate samples.
	if (pchan && ((solo < 0) || _cindex [i] == solo))
	{
 // Pointer into output buffer.
 wp  = pchan [i] + opind;
 
 // Per sample increments.
 dgA /= nfram;
 dgB /= nfram;
 
 // Find initial index into delay lines.
 k = _delind + opind - (int)(S->_delay);
 
 // Loop over all samples.
 for (j = 0; j < nfram; j++, k++)
 {
 // Apply gains and add to output.
 k &= m;
 #ifdef TRACE
 //            printf ("chcoun: %3d samples:%3d\n", i,j);
 #endif
 
 wp [j] += gA * pA [k] + gB * pB [k];
 
 // Adjust paramters for next sample.
 gA += dgA;
 gB += dgB;
 }
	}
	S++;
	if (P) P++;
 }
 }
 
 
 // ----------------------------------------------
 // Interpolate delay and gain, moving source.
 // ----------------------------------------------
 //
 void Source::ipol (float **pchan, int opind, int nfram, int solo)
 {
 int      i, j, k, m;
 float    gA, dgA, gB, dgB, t1, t2, dt, a, b, r;
 float    *wp, *pA, *pB;
 State    *S;
 Outpars  *P;
 
 #ifdef TRACE
 printf ("IPOL  count = %5d  nfram = %5d  opind = %5d\n", _count, nfram, opind);
 #endif
 
 m  = _dsize - 1;
 r  = nfram / (float) _count;
 pA = _delayA;
 pB = _delayB;
 
 // Loop over all outputs.
 S = _states;
 P = _pars1->_outpars;
 for (i = 0; i < _noutp; i++)
 {
	// Starting values for interpolation.
 t1 = S->_delay;
 gA = S->_gainA;
 gB = S->_gainB;
 
	// Differences for nfram samples.
	dt  = r * (P->_delay - t1);
	dgA = r * (P->_gainA - gA);
	dgB = r * (P->_gainB - gB);
 
	// Updata current state.
	S->_delay = t1 + dt;
 S->_gainA = gA + dgA;
 S->_gainB = gB + dgB;
 
	// If output is enabled, calculate samples.
	if (pchan && ((solo < 0) || _cindex [i] == solo))
	{
 // Pointer into output buffer.
 wp  = pchan [i] + opind;
 
 // Per sample increments.
 dt  /= nfram;
 dgA /= nfram;
 dgB /= nfram;
 
 // Split initial delay in fractional and
 // integer parts.
 k = (int)(floorf (t1));
 t1 -= k;
 t2 = 1 - t1;
 
 // Find initial index into delay lines.
 k = _delind + opind - k - 1;
 
 // Loop over all samples.
 for (j = 0; j < nfram; j++, k++)
 {
 // Interpolate in both delay lines,
 // apply gains and add to output.
 k &= m;
 a = t1 * pA [k] + t2 * pA [k + 1];
 b = t1 * pB [k] + t2 * pB [k + 1];
 wp [j] += gA * a + gB * b;
 
 // Adjust paramters for next sample.
 gA += dgA;
 gB += dgB;
 
 // Note: at least on Intel processors, updating
 // boht t1 and t2 and checking for overflow is
 // considerably faster than updating a single
 // float value and then finding the inter part
 // again for each sample, even when just using
 // a cast instead of floorf().
 // This method assumes that fabsf (dt) < 1.
 t1 += dt;
 t2 -= dt;
 if (t1 < 0)
 {
 // Integer part of delay decrements.
 t1 += 1;
 t2 -= 1;
 k++; // Index increments!
 }
 if (t2 < 0)
 {
 // Integer part of delay increments.
 t1 -= 1;
 t2 += 1;
 k--; // Index decrements!
 }
 }
	}
	S++;
	P++;
 }
 }
 
 
 
 // ------------------------------------------------------------------------------------
 */
