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

#include "GRRMOD_internals.h"
#include "mikmod/include/mikmod_build.h"
#include <string.h>

static BOOL GRRMOD_Eof(MREADER * reader);
static BOOL GRRMOD_Read(MREADER * reader, void *ptr, size_t size);
static int GRRMOD_Get(MREADER * reader);
static BOOL GRRMOD_Seek(MREADER * reader, long offset, int whence);
static long GRRMOD_Tell(MREADER * reader);

/**
 * Structure to hold the music information.
 */
typedef struct _MOD_READER {
    MREADER Core;       /**< Structure with a bunch of pointers to functions. */
    u64     Offset;     /**< Current file position. */
    char    *BufferPtr; /**< Pointer to the music data. */
    u64     Size;       /**< Size of the music data. */
} MOD_READER;

typedef struct _GRRMOD_DATA {
    char *ModType;    /**< A string representing the MOD type. */
    char *SongTitle;  /**< A string representing the song title. */
} GRRMOD_DATA;

static GRRMOD_DATA MusicData = {};
static MODULE *module = NULL;   /**< Module structure. */

void GRRMOD_MOD_Register(GRRLIB_FuntionsList *RegFunc) {
    RegFunc->Init = GRRMOD_MOD_Init;
    RegFunc->End = GRRMOD_MOD_End;
    RegFunc->SetMOD = GRRMOD_MOD_SetMOD;
    RegFunc->Unload = GRRMOD_MOD_Unload;
    RegFunc->SetFrequency = GRRMOD_MOD_SetFrequency;
    RegFunc->GetVoiceFrequency = GRRMOD_MOD_GetVoiceFrequency;
    RegFunc->GetVoiceVolume = GRRMOD_MOD_GetVoiceVolume;
    RegFunc->GetRealVoiceVolume = GRRMOD_MOD_GetRealVoiceVolume;
    RegFunc->Start = GRRMOD_MOD_Start;
    RegFunc->Stop = GRRMOD_MOD_Stop;
    RegFunc->Pause = GRRMOD_MOD_Pause;
    RegFunc->GetSongTitle = GRRMOD_MOD_GetSongTitle;
    RegFunc->GetModType = GRRMOD_MOD_GetModType;
    RegFunc->Update = GRRMOD_MOD_Update;
}

/**
 * Initialize MOD library.
 * @return A number representating a code:
 *         -     0 : The operation completed successfully.
 *         -    -1 : Failed to initialize the MOD engine.
 * @see GRRMOD_MOD_End
 */
s8 GRRMOD_MOD_Init(bool stereo) {
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    md_device = 1; // Only one device is used

    md_mode = DMODE_16BITS |
              DMODE_HQMIXER |
              DMODE_SOFT_MUSIC |
              DMODE_SOFT_SNDFX;

    if(stereo) {
        md_mode |= DMODE_STEREO; //this causes some modules (s3m mostly) to play back incorrectly on Wii
    }

    char CommandLine[15] = {};
    sprintf(CommandLine, "buffer=%d", SNDBUFFERSIZE);
    if(MikMod_Init(CommandLine)) {
        return -1;
    }
    return 0;
}

/**
 * Call this before exiting your application.
 * Ensure this function is only ever called once.
 */
void GRRMOD_MOD_End() {
    MikMod_Exit();
}

/**
 * Load a MOD file from memory.
 */
void GRRMOD_MOD_SetMOD(const void *mem, u64 size) {
    MOD_READER Reader;

    Reader.Offset = 0;
    Reader.BufferPtr = (char *)mem;
    Reader.Size = size;
    Reader.Core.Eof = &GRRMOD_Eof;
    Reader.Core.Read = &GRRMOD_Read;
    Reader.Core.Get = &GRRMOD_Get;
    Reader.Core.Seek = &GRRMOD_Seek;
    Reader.Core.Tell = &GRRMOD_Tell;

    if(module) {
        GRRMOD_MOD_Unload();
    }
    module = Player_LoadGeneric((MREADER *)&Reader, 128, 0);
    if(module) {
        module->wrap = true;    // The module will restart when it's finished
        MusicData.SongTitle = strdup(module->songname);
        MusicData.ModType = strdup(module->modtype);
    }
}

/**
 * Unload a MOD file.
 */
void GRRMOD_MOD_Unload() {
    if(module) {
        Player_Free(module);
        module = NULL;
    }
    if(MusicData.ModType) {
        free(MusicData.ModType);
        MusicData.ModType = NULL;
    }
    if(MusicData.SongTitle) {
        free(MusicData.SongTitle);
        MusicData.SongTitle = NULL;
    }
}

/**
 * This function starts the specified module playback.
 */
void GRRMOD_MOD_Start() {
    if(module == NULL)   return;

    Player_Start(module);
}

/**
 * This function stops the currently playing module.
 */
void GRRMOD_MOD_Stop() {
    Player_SetPosition(0);
    Player_Stop();
}

/**
 * This function toggles the playing/paused status of the module.
 */
void GRRMOD_MOD_Pause() {
    Player_TogglePause();
}

/**
 * Get the song title.
 * @return Pointer to the song title.
 */
char *GRRMOD_MOD_GetSongTitle() {
    return MusicData.SongTitle;
}

/**
 * Get the MOD type.
 * @return Pointer to the MOD type.
 */
char *GRRMOD_MOD_GetModType() {
    return MusicData.ModType;
}

/**
 * Set the frequency. Values are harcoded at 48000Hz.
 * @param freq Frequency to set in kHz.
 */
void GRRMOD_MOD_SetFrequency(u32 freq) {
    md_mixfreq = freq;
}

/**
 * This function returns the frequency of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get frequency.
 * @return The current frequency of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_MOD_GetVoiceFrequency(u8 voice) {
    return Voice_GetFrequency(voice);
}

/**
 * This function returns the volume of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get volume.
 * @return The current volume of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_MOD_GetVoiceVolume(u8 voice) {
    return Voice_GetVolume(voice);
}

/**
 * This function returns the actual playing volume of the specified voice.
 * @param voice The number of the voice to analyze (starting from zero).
 * @return The real volume of the voice when the function was called, in the range 0-65535.
 */
u32 GRRMOD_MOD_GetRealVoiceVolume(u8 voice) {
    return Voice_RealVolume(voice);
}

/**
 * Set a buffer to update. This routine should be called on a regular basis to update the sound.
 * @param buffer The buffer to update.
 */
void GRRMOD_MOD_Update(u8 *buffer) {
    if(module) {
        setBuffer((s16 *)buffer, SNDBUFFERSIZE);
        MikMod_Update();
    }
}

/**
 * This function has the same behaviour as feof.
 */
static BOOL GRRMOD_Eof(MREADER * reader) {
    MOD_READER *pReader = (MOD_READER *) reader;

    return (pReader->Size == (pReader->Offset)) ? true : false;
}

/**
 * This function copies length bytes of data into dest, and return zero if an error occured, and any nonzero value otherwise. Note that an end-of-file condition will not be considered as an error in this case.
 */
static BOOL GRRMOD_Read(MREADER * reader, void *ptr, size_t size) {
    MOD_READER *pReader = (MOD_READER *) reader;

    memcpy(ptr, pReader->BufferPtr + pReader->Offset, size);
    pReader->Offset += size;

    return 1;
}

/**
 * This function has the same behaviour as fgetc.
 */
static int GRRMOD_Get(MREADER * reader) {
    MOD_READER *pReader = (MOD_READER *) reader;
    char buf;

    buf = *(pReader->BufferPtr + pReader->Offset);
    pReader->Offset++;

    return((int)buf);
}

/**
 * This function has the same behaviour as fseek, with offset 0 meaning the start of the object (module, sample) being loaded.
 */
static BOOL GRRMOD_Seek(MREADER * reader, long offset, int whence) {
    MOD_READER *pReader = (MOD_READER *) reader;

    if(whence == SEEK_SET)
        pReader->Offset = offset;
    else
        pReader->Offset += offset;

    return 1;
}

/**
 * This function has the same behaviour as ftell, with offset 0 meaning the start of the object being loaded.
 */
static long GRRMOD_Tell(MREADER * reader) {
    MOD_READER *pReader = (MOD_READER *) reader;

    return pReader->Offset;
}
