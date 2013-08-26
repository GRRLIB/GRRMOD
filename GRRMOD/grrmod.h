/*------------------------------------------------------------------------------
Copyright (c) 2013 The GRRLIB Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
------------------------------------------------------------------------------*/

/**
 * @file grrmod.h
 * GRRMOD user include file.
 */


#ifndef __GRRMOD_H__
#define __GRRMOD_H__

/**
 * Version information for GRRMOD.
 */
#define GRRMOD_VER_STRING "1.0.0 ALPHA"

//==============================================================================
// Includes
//==============================================================================
#include <gccore.h>
//==============================================================================

//==============================================================================
// C++ header
//==============================================================================
#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

s8 GRRMOD_Init(bool stereo);
void GRRMOD_End();
void GRRMOD_SetMOD(const void *mem, u64 size);
void GRRMOD_Unload();
void GRRMOD_SetFrequency(u32 freq);
void GRRMOD_SetVolume(s16 volume_l, s16 volume_r);
u32 GRRMOD_GetVoiceFrequency(u8 voice);
u32 GRRMOD_GetVoiceVolume(u8 voice);
u32 GRRMOD_GetRealVoiceVolume(u8 voice);
void GRRMOD_Start();
void GRRMOD_Stop();
void GRRMOD_Pause();
char *GRRMOD_GetSongTitle();
char *GRRMOD_GetModType();

#ifdef _GRRMOD_DEBUG
u32 GRRMOD_MixingTime();
#endif

//==============================================================================
// C++ footer
//==============================================================================
#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif // __GRRMOD_H__
/** @} */ // end of group
/**
 * @mainpage GRRMOD Documentation
 * @image html grrmod_logo.png
 * Welcome to the GRRMOD documentation.
 *
 * @section Introduction
 * GRRMOD is a MOD player library for Wii application developers.
 * It supports the following format:
 * - MOD (15 and 31 instruments)
 * - IT (Impulse Tracker)
 * - S3M (Scream Tracker 3)
 * - XM (FastTracker 2)
 *
 * @section Links
 * Forum: http://grrlib.santo.fr/forum\n
 * Code: http://code.google.com/p/grrlib\n
 * IRC: <a href="irc://irc.efnet.net/grrlib">#GRRLIB</a> on EFnet
 *
 * @section Credits
 * Main Coders : NoNameNo, Crayon\n
 * Documentation  : Crayon, NoNameNo\n
 *
 * @section Licence
 * Copyright (c) 2013 The GRRLIB Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
