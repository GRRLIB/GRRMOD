/*------------------------------------------------------------------------------
Copyright (c) 2010-2024 The GRRLIB Team

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
#define GRRMOD_VER_STRING "0.0.1"

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
void GRRMOD_End(void);
void GRRMOD_SetMOD(const void *mem, u64 size);
void GRRMOD_Unload(void);
void GRRMOD_SetFrequency(u32 freq);
void GRRMOD_SetVolume(s16 volume_l, s16 volume_r);
u32 GRRMOD_GetVoiceFrequency(u8 voice);
u32 GRRMOD_GetVoiceVolume(u8 voice);
u32 GRRMOD_GetRealVoiceVolume(u8 voice);
void GRRMOD_Start(void);
void GRRMOD_Stop(void);
void GRRMOD_Pause(void);
char *GRRMOD_GetSongTitle(void);
char *GRRMOD_GetModType(void);

#ifdef _GRRMOD_DEBUG
u32 GRRMOD_MixingTime(void);
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
 * - 669 (Composer 669)
 * - AMF (DSMI Advanced Module Format)
 * - AMF (ASYLUM Music Format V1.0)
 * - APUN (APlayer)
 * - DSM (DSIK internal format)
 * - FAR (Farandole Composer)
 * - GDM (General DigiMusic)
 * - IMF (Imago Orpheus)
 * - IT (Impulse Tracker)
 * - MED (OctaMED)
 * - MOD (15 and 31 instruments)
 * - MTM (MultiTracker Module editor)
 * - OKT (Amiga Oktalyzer)
 * - S3M (Scream Tracker 3)
 * - STM (Scream Tracker)
 * - STX (Scream Tracker Music Interface Kit)
 * - ULT (UltraTracker)
 * - UNI (MikMod)
 * - XM (FastTracker 2)
 *
 * @section Links
 * Forum: http://grrlib.santo.fr/forum\n
 * Code: https://github.com/GRRLIB/GRRMOD\n
 * Chat: <a href="irc://irc.efnet.net/grrlib">\#GRRLIB</a> on EFnet
 *
 * @section Credits
 * Main Coders : NoNameNo, Crayon\n
 * Documentation : Crayon, NoNameNo\n
 *
 * @section Licence
 * See the [LICENCE](LICENCE.md) file for licence rights and limitations (MIT).
 */
