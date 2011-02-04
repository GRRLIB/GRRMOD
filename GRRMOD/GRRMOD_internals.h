/*------------------------------------------------------------------------------
Copyright (c) 2011 The GRRLIB Team

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

#define SNDBUFFERSIZE   (5760)    /**< Audio maximum buffer size. */

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

/**
 * Structure to hold the list of functions to use.
 */
typedef struct GRRLIB_FuntionsList {
    s8 (*Init)(bool stereo);
    void (*End)(void);
    void (*SetMOD)(const void *mem, u64 size);
    void (*Unload)(void);
    void (*SetFrequency)(u32 freq);
    u32 (*GetVoiceFrequency)(u8 voice);
    u32 (*GetVoiceVolume)(u8 voice);
    u32 (*GetRealVoiceVolume)(u8 voice);
    void (*Start)(void);
    void (*Stop)(void);
    void (*Pause)(void);
    char *(*GetSongTitle)(void);
    char *(*GetModType)(void);
    void (*Update)(u8 *buffer);
} GRRLIB_FuntionsList;

// Module functions
void GRRMOD_MOD_Register(GRRLIB_FuntionsList *RegFunc);
s8 GRRMOD_MOD_Init(bool stereo);
void GRRMOD_MOD_End();
void GRRMOD_MOD_SetMOD(const void *mem, u64 size);
void GRRMOD_MOD_Unload();
void GRRMOD_MOD_SetFrequency(u32 freq);
u32 GRRMOD_MOD_GetVoiceFrequency(u8 voice);
u32 GRRMOD_MOD_GetVoiceVolume(u8 voice);
u32 GRRMOD_MOD_GetRealVoiceVolume(u8 voice);
void GRRMOD_MOD_Start();
void GRRMOD_MOD_Stop();
void GRRMOD_MOD_Pause();
char *GRRMOD_MOD_GetSongTitle();
char *GRRMOD_MOD_GetModType();
void GRRMOD_MOD_Update(u8 *buffer);

// MP3 functions
void GRRMOD_MP3_Register(GRRLIB_FuntionsList *RegFunc);
s8 GRRMOD_MP3_Init(bool stereo);
void GRRMOD_MP3_End();
void GRRMOD_MP3_SetMOD(const void *mem, u64 size);
void GRRMOD_MP3_Unload();
void GRRMOD_MP3_SetFrequency(u32 freq);
u32 GRRMOD_MP3_GetVoiceFrequency(u8 voice);
u32 GRRMOD_MP3_GetVoiceVolume(u8 voice);
u32 GRRMOD_MP3_GetRealVoiceVolume(u8 voice);
void GRRMOD_MP3_Start();
void GRRMOD_MP3_Stop();
void GRRMOD_MP3_Pause();
char *GRRMOD_MP3_GetSongTitle();
char *GRRMOD_MP3_GetModType();
void GRRMOD_MP3_Update(u8 *buffer);

//==============================================================================
// C++ footer
//==============================================================================
#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif // __GRRMOD_INTERNALS_H__
