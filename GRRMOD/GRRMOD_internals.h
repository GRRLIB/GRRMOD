/*------------------------------------------------------------------------------
Copyright (c) 2010 The GRRLIB Team

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
 * @file GRRMOD_internals.h
 * GRRMOD Internal include file.
 */

#ifndef __GRRMOD_INTERNALS_H__
#define __GRRMOD_INTERNALS_H__

#define AUDIOBUFFER 4096    /**< Audio buffer size. */

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

// Module functions
s8 GRRMOD_MOD_Init();
void GRRMOD_MOD_End();
void GRRMOD_MOD_SetMOD(const void *mem, u64 size);
void GRRMOD_MOD_Unload();
void GRRMOD_MOD_SetFrequency(u32 freq);
void GRRMOD_MOD_SetVolume(s8 musicvolume);
u32 GRRMOD_MOD_GetVoiceFrequency(u8 voice);
u32 GRRMOD_MOD_GetVoiceVolume(u8 voice);
u32 GRRMOD_MOD_GetRealVoiceVolume(u8 voice);
void GRRMOD_MOD_Start();
void GRRMOD_MOD_Stop();
void GRRMOD_MOD_Pause();
char *GRRMOD_MOD_GetSongTitle();
char *GRRMOD_MOD_GetModType();
void GRRMOD_MOD_Update(u8 *buffer);

//==============================================================================
// C++ footer
//==============================================================================
#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif // __GRRMOD_INTERNALS_H__
