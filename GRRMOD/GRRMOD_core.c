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

#include "grrmod.h"
#include "GRRMOD_internals.h"
#include <string.h>
#include <aesndlib.h>
#include <ogc/lwp_watchdog.h>

#define STACKSIZE       8192

static bool thr_running = false;
static bool sndPlaying = false;
static bool paused = false;

static vu32 curr_audio = 0;
static u8 audioBuf[2][SNDBUFFERSIZE] ATTRIBUTE_ALIGN(32);

static lwpq_t player_queue;
static lwp_t hplayer;
static u8 player_stack[STACKSIZE] ATTRIBUTE_ALIGN(8);
static void* player(void *);

static s32 mod_freq = 48000;
static AESNDPB  *modvoice = NULL;

#ifdef _GRRMOD_DEBUG
static u64 mixtime = 0;
#endif

static GRRLIB_FuntionsList RegFunc;

// Static Functions
static void* player(void *arg);
static void __aesndvoicecallback(AESNDPB *pb,u32 state);

/**
 * Initialize GRRMOD. Call this once at the beginning your code.
 * @return A number representating a code:
 *         -     0 : The operation completed successfully.
 *         -    -1 : Failed to initialize engine.
 * @see GRRMOD_End
 */
s8 GRRMOD_Init(bool stereo) {
    GRRMOD_MOD_Register(&RegFunc);
    //GRRMOD_MP3_Register(&RegFunc);

    s8 errorCode = RegFunc.Init(stereo);
    if(errorCode != 0) {
        return errorCode;
    }

    AESND_Init();

    modvoice = AESND_AllocateVoice(__aesndvoicecallback);
    if(modvoice == NULL) {
        return -1;
    }

    AESND_SetVoiceFormat(modvoice, stereo ? VOICE_STEREO16 : VOICE_MONO16);
    AESND_SetVoiceFrequency(modvoice, mod_freq);
    AESND_SetVoiceVolume(modvoice, 255, 255);
    AESND_SetVoiceStream(modvoice, true);

    GRRMOD_SetFrequency(48000);

    LWP_InitQueue(&player_queue);

    sndPlaying = false;
    thr_running = false;
    paused = false;

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
    if(sndPlaying) return;

    RegFunc.Start();

    memset(audioBuf[0], 0, SNDBUFFERSIZE);
    memset(audioBuf[1], 0, SNDBUFFERSIZE);

    DCFlushRange(audioBuf[0], SNDBUFFERSIZE);
    DCFlushRange(audioBuf[1], SNDBUFFERSIZE);

    while(thr_running);

    curr_audio = 0;
    paused = false;
    sndPlaying = true;
    if(LWP_CreateThread(&hplayer, player, NULL, player_stack, STACKSIZE, 80)!=-1) {
        AESND_SetVoiceStop(modvoice, false);
        return;
    }
    sndPlaying = false;
}

/**
 * This function stops the currently playing module.
 */
void GRRMOD_Stop() {
    if(!sndPlaying) return;
    AESND_SetVoiceStop(modvoice, true);

    curr_audio = 0;
    sndPlaying = false;
    LWP_ThreadSignal(player_queue);
    LWP_JoinThread(hplayer, NULL);

    RegFunc.Stop();
}

/**
 * This function toggles the playing/paused status of the module.
 */
void GRRMOD_Pause() {
    if(!sndPlaying) return;

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
void GRRMOD_SetFrequency(u32 freq) {
    if(freq==32000 || freq==48000) {
        mod_freq = 48000;
        RegFunc.SetFrequency(freq);
    }
}

/**
 * Set the volume levels for the music (call it after MODPlay_SetMOD()).
 * @param volume_l The music volume (left), 0 to 255.
 * @param volume_r The music volume (right), 0 to 255.
 */
void GRRMOD_SetVolume(s16 volume_l, s16 volume_r) {
    AESND_SetVoiceVolume(modvoice,
        (volume_l<0) ? 0 : (volume_l>255) ? 255 : volume_l,
        (volume_r<0) ? 0 : (volume_r>255) ? 255 : volume_r);
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
 * Set a buffer to update. This routine is called inside a thread.
 */
static void* player(void *arg) {
#ifdef _GRRMOD_DEBUG
u64 start;
#endif

    u32 i;
    thr_running = true;
    while(sndPlaying) {
        LWP_ThreadSleep(player_queue);
        if(sndPlaying) {
            if(paused) {
                for(i=0; i<(SNDBUFFERSIZE>>1); i++)
                    ((u16*)((u8*)audioBuf[curr_audio]))[i] = 0;
            }
            else {
#ifdef _GRRMOD_DEBUG
                start = gettime();
#endif
                RegFunc.Update(((u8*)audioBuf[curr_audio]));
#ifdef _GRRMOD_DEBUG
                mixtime = gettime() - start;
#endif
            }
        }
    }
    thr_running = false;

    return 0;
}

/**
 * Callback function for AESND_AllocateVoice.
 */
static void __aesndvoicecallback(AESNDPB *pb, u32 state) {
    switch(state) {
        case VOICE_STATE_STOPPED:
        case VOICE_STATE_RUNNING:
            break;
        case VOICE_STATE_STREAM:
            AESND_SetVoiceBuffer(pb, (void*)audioBuf[curr_audio], SNDBUFFERSIZE);
            LWP_ThreadSignal(player_queue);
            curr_audio ^= 1;
            break;
    }
}

#ifdef _GRRMOD_DEBUG
u32 GRRMOD_MixingTime() {
    return ticks_to_microsecs(mixtime);
}
#endif