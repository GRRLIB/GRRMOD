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

#include "GRRMOD_internals.h"
#include "mikmod/include/mikmod.h"
#include <string.h>

// This is normally in the mikmod.h file of the MikMod project
MIKMODAPI extern struct MDRIVER drv_wii; /* Wii driver. */

typedef struct _GRRMOD_DATA {
    char *ModType;    /**< A string representing the MOD type. */
    char *SongTitle;  /**< A string representing the song title. */
} GRRMOD_DATA;

static GRRMOD_DATA MusicData = {};
static MODULE *module = NULL;   /**< Module structure. */

static u8 *pBuffer; /**< Pointer to the sound buffer. */
static u8 **ppBuffer = &pBuffer; /**< Pointer to the sound buffer pointer. */

/**
 * Register MOD function list.
 * @param RegFunc The function list to register.
 */
void GRRMOD_MOD_Register(GRRMOD_FuntionsList *RegFunc) {
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
 * @param stereo If set to true the music will play in stereo, otherwise it will mono.
 * @return A number representating a code:
 *         -     0 : The operation completed successfully.
 *         -    -1 : Failed to initialize the MOD engine.
 * @see GRRMOD_MOD_End
 */
s8 GRRMOD_MOD_Init(bool stereo) {
    MikMod_RegisterDriver(&drv_wii);
    MikMod_RegisterAllLoaders();
    md_device = 1; // Only one device is used

    md_mode = DMODE_16BITS |
              DMODE_HQMIXER |
              DMODE_SOFT_MUSIC |
              DMODE_SOFT_SNDFX;

    if(stereo == true) {
        md_mode |= DMODE_STEREO; //this causes some modules (s3m mostly) to play back incorrectly on Wii
    }

    char CommandLine[35] = {};
    sprintf(CommandLine, "buffer=%d,size=%d", (int)ppBuffer, SNDBUFFERSIZE);
    if(MikMod_Init(CommandLine) != 0) {
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
 * @param mem Memory to set.
 * @param size Size of the memory to set.
 */
void GRRMOD_MOD_SetMOD(const void *mem, u64 size) {
    if(module != NULL) {
        GRRMOD_MOD_Unload();
    }
    FILE *File = fmemopen((void *)mem, size, "rb");
    module = Player_LoadFP(File, 128, 0);
    if(module != NULL) {
        module->wrap = true; // The module will restart when it's finished
        MusicData.SongTitle = strdup(module->songname);
        MusicData.ModType = strdup(module->modtype);
    }
}

/**
 * Unload a MOD file.
 */
void GRRMOD_MOD_Unload() {
    if(module != NULL) {
        Player_Free(module);
        module = NULL;
    }
    if(MusicData.ModType != NULL) {
        free(MusicData.ModType);
        MusicData.ModType = NULL;
    }
    if(MusicData.SongTitle != NULL) {
        free(MusicData.SongTitle);
        MusicData.SongTitle = NULL;
    }
}

/**
 * This function starts the specified module playback.
 */
void GRRMOD_MOD_Start() {
    if(module == NULL) {
        return;
    }

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
    if(module != NULL) {
        pBuffer = buffer; // Point to the new sound buffer
        MikMod_Update();
    }
}
