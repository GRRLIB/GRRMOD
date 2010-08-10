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

#include "grrmod.h"
#include "GRRMOD_internals.h"
#include <string.h>

static void GRRMOD_Callback();

static u8  SoundBuffer[2][AUDIOBUFFER]  ATTRIBUTE_ALIGN(32);
static u8  tempbuffer[AUDIOBUFFER];
static u32 whichab = 0;
static bool playing = false;
static bool paused = false;
static GRRLIB_FuntionsList RegFunc;


/**
 * Initialize GRRMOD. Call this once at the beginning your code.
 * @return A number representating a code:
 *         -     0 : The operation completed successfully.
 *         -    -1 : Failed to initialize the MOD engine.
 * @see GRRMOD_End
 */
s8 GRRMOD_Init() {
    GRRMOD_MOD_Register(&RegFunc);
    //GRRMOD_MP3_Register(&RegFunc);

    AUDIO_Init(NULL);
    GRRMOD_SetFrequency(48000);

    RegFunc.Init();

    return 0;
}

/**
 * Call this before exiting your application.
 * Ensure this function is only ever called once.
 */
void GRRMOD_End() {
    GRRMOD_Unload();

    RegFunc.End();
}

/**
 * Load a MOD file from memory.
 */
void GRRMOD_SetMOD(const void *mem, u64 size) {
    RegFunc.SetMOD(mem, size);
}

/**
 * Unload a MOD file.
 */
void GRRMOD_Unload() {
    GRRMOD_Stop();
    RegFunc.Unload();
}

/**
 * This function starts the specified module playback.
 */
void GRRMOD_Start() {
    if(playing) return;

    RegFunc.Start();

    memset(&SoundBuffer[0], 0, AUDIOBUFFER);
    memset(&SoundBuffer[1], 0, AUDIOBUFFER);

    DCFlushRange(&SoundBuffer[0], AUDIOBUFFER);
    DCFlushRange(&SoundBuffer[1], AUDIOBUFFER);

    whichab = 0;
    playing = true;
    paused = false;

    AUDIO_RegisterDMACallback( GRRMOD_Callback );
    AUDIO_InitDMA((u32)SoundBuffer[whichab], AUDIOBUFFER);
    AUDIO_StartDMA();
}

/**
 * This function stops the currently playing module.
 */
void GRRMOD_Stop() {
    if(!playing) return;

    AUDIO_StopDMA();
    AUDIO_RegisterDMACallback(NULL);

    playing = false;

    RegFunc.Stop();
}

/**
 * This function toggles the playing/paused status of the module.
 */
void GRRMOD_Pause() {
    if(!playing) return;

    RegFunc.Pause();
    paused = !paused;
}

/**
 * Get the song title.
 * @return Pointer to the song title.
 */
char *GRRMOD_GetSongTitle() {
    return RegFunc.GetSongTitle();
}

/**
 * Get the MOD type.
 * @return Pointer to the MOD type.
 */
char *GRRMOD_GetModType() {
    return RegFunc.GetModType();
}

/**
 * Set the frequency. Only 32000Hz and 48000Hz are available.
 * @param freq Frequency to set in kHz.
 */
void GRRMOD_SetFrequency(u32 freq)
{
    if(freq==32000)
        AUDIO_SetDSPSampleRate(AI_SAMPLERATE_32KHZ);
    else
        AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
    RegFunc.SetFrequency(freq);
}

/**
 * Set the volume levels for the MOD music (call it after MODPlay_SetMOD()).
 * @param musicvolume The music volume, 0 to 64.
 */
void GRRMOD_SetVolume(s8 musicvolume)
{
    RegFunc.SetVolume(musicvolume);
}

/**
 * This function returns the frequency of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get frequency.
 * @return The current frequency of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_GetVoiceFrequency(u8 voice) {
    return RegFunc.GetVoiceFrequency(voice);
}

/**
 * This function returns the volume of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get volume.
 * @return The current volume of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_GetVoiceVolume(u8 voice) {
    return RegFunc.GetVoiceVolume(voice);
}

/**
 * This function returns the actual playing volume of the specified voice.
 * @param voice The number of the voice to analyze (starting from zero).
 * @return The real volume of the voice when the function was called, in the range 0-65535.
 */
u32 GRRMOD_GetRealVoiceVolume(u8 voice) {
    return RegFunc.GetRealVoiceVolume(voice);
}

/**
 * Callback function for DMA.
 */
static void GRRMOD_Callback() {
    if (playing) {
        AUDIO_StopDMA();
        AUDIO_InitDMA((u32)SoundBuffer[whichab], AUDIOBUFFER);
        DCFlushRange(&SoundBuffer[whichab], AUDIOBUFFER);
        AUDIO_StartDMA();

        whichab ^= 1;
        memset(&SoundBuffer[whichab], 0, AUDIOBUFFER);

        RegFunc.Update(tempbuffer);

        if(paused) {
            memset(tempbuffer, 0, AUDIOBUFFER);
        }
        else {
            memcpy(SoundBuffer[whichab], tempbuffer, AUDIOBUFFER);
        }
    }
    else {
        memset(tempbuffer, 0, AUDIOBUFFER);
    }
}
