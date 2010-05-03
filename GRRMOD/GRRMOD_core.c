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
#include <string.h>
#include "mikmod/include/mikmod_build.h"

static void GRRMOD_Callback();
static BOOL GRRMOD_Eof(MREADER * reader);
static BOOL GRRMOD_Read(MREADER * reader, void *ptr, size_t size);
static int GRRMOD_Get(MREADER * reader);
static BOOL GRRMOD_Seek(MREADER * reader, long offset, int whence);
static long GRRMOD_Tell(MREADER * reader);

/**
 * Structure to hold the music information.
 */
typedef struct _MOD_READER
{
    MREADER Core;		/**<  */
    u64     Offset;		/**<  */
    char    *BufferPtr;	/**< Pointer to the music data. */
    u64     Size;		/**< Size of the music data. */
} MOD_READER;

static MODULE *module;	/**<  */

#define AUDIOBUFFER 4096	/**< Audio buffer size. */
static u8  SoundBuffer[2][AUDIOBUFFER]  __attribute__((__aligned__(32)));
static u8  tempbuffer[AUDIOBUFFER];
static u32 whichab = 0;
static bool playing = false;


/**
 * Initialize GRRMOD. Call this once at the beginning your code.
 * @return A number representating a code:
 *         -     0 : The operation completed successfully.
 *         -    -1 : Failed to initialize the MOD engine.
 * @see GRRMOD_End
 */
s8 GRRMOD_Init() {
    AUDIO_Init(NULL);
    GRRMOD_SetFrequency(48000);

    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();

    md_mode = 0;
    md_mode |= DMODE_16BITS;
    md_mode |= DMODE_SOFT_MUSIC;
    md_mode |= DMODE_SOFT_SNDFX;
    //md_mode |= DMODE_STEREO;  //this causes some modules (s3m mostly) to play back incorrectly on wii, i dont know why
    md_mode |= DMODE_HQMIXER;

    if(MikMod_Init("")) {
        return -1;
    }
    return 0;
}

/**
 * Call this before exiting your application.
 * Ensure this function is only ever called once.
 */
void GRRMOD_End() {
    GRRMOD_Unload();
    MikMod_Exit();
}

/**
 * Load a MOD file from memory.
 */
void GRRMOD_SetMOD(const void *mem, u64 size) {
    MOD_READER *Reader = (MOD_READER *)malloc(sizeof (MOD_READER));
    if(Reader) {
        Reader->Offset = 0;
        Reader->BufferPtr = (char *)mem;
        Reader->Size = size;
        Reader->Core.Eof = &GRRMOD_Eof;
        Reader->Core.Read = &GRRMOD_Read;
        Reader->Core.Get = &GRRMOD_Get;
        Reader->Core.Seek = &GRRMOD_Seek;
        Reader->Core.Tell = &GRRMOD_Tell;

        module = Player_LoadGeneric((MREADER *)Reader, 128, 0);
        if(module) {
            module->wrap = true;    // The module will restart when it's finished
        }
    }
}

/**
 * Unload a MOD file.
 */
void GRRMOD_Unload() {
    GRRMOD_Stop();
    if(module) {
        Player_Free(module);
    }
}

/**
 * This function starts the specified module playback.
 */
void GRRMOD_Start() {
    if(playing || module == NULL)   return;

    Player_Start(module);

    memset(&SoundBuffer[0], 0, AUDIOBUFFER);
    memset(&SoundBuffer[1], 0, AUDIOBUFFER);

    DCFlushRange((char *)&SoundBuffer[0], AUDIOBUFFER);
    DCFlushRange((char *)&SoundBuffer[1], AUDIOBUFFER);

    whichab = 0;
    playing = true;

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

    Player_SetPosition(0);
    Player_Stop();
}

/**
 * This function toggles the playing/paused status of the module.
 */
void GRRMOD_Pause() {
    Player_TogglePause();
}

/**
 * Get the song title.
 */
void GRRMOD_GetSongTitle(char *Buffer, u32 Size) {
    if(module) {
        strncpy(Buffer, module->songname, Size);
    }
    else {
        Buffer = '\0';
    }
}

/**
 * Get the MOD type.
 */
void GRRMOD_GetModType(char *Buffer, u32 Size) {
    if(module) {
        strncpy(Buffer, module->modtype, Size);
    }
    else {
        Buffer = '\0';
    }
}

/**
 * Set the frequency. Values are harcoded at 48000kHz.
 * @param freq Frequency to set in kHz.
 */
void GRRMOD_SetFrequency(u32 freq)
{
    AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
    md_mixfreq = 24000;
}

/**
 * Set the volume levels for the MOD music (call it after MODPlay_SetMOD()).
 * @param musicvolume The music volume, 0 to 64.
 */
void GRRMOD_SetVolume(s8 musicvolume)
{
    if(musicvolume < 0)
        musicvolume = 0;
    else if(musicvolume > 64)
        musicvolume = 64;

    Player_SetVolume(musicvolume * 2);
}

/**
 * This function returns the frequency of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get frequency.
 * @return The current frequency of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_GetVoiceFrequency(u8 voice) {
    return Voice_GetFrequency(voice);
}

/**
 * This function returns the volume of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get volume.
 * @return The current volume of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_GetVoiceVolume(u8 voice) {
    return Voice_GetVolume(voice);
}

/**
 * This function returns the actual playing volume of the specified voice.
 * @param voice The number of the voice to analyze (starting from zero).
 * @return The real volume of the voice when the function was called, in the range 0-65535.
 */
u32 GRRMOD_GetRealVoiceVolume(u8 voice) {
    return Voice_RealVolume(voice);
}

/**
 * Callback function for DMA.
 */
static void GRRMOD_Callback()
{
    u32 *src;
    u32 *dst;
    int count;

    if (playing) {
        AUDIO_StopDMA();
        AUDIO_InitDMA((u32)SoundBuffer[whichab], AUDIOBUFFER);
        DCFlushRange(&SoundBuffer[whichab], AUDIOBUFFER);
        AUDIO_StartDMA();

        whichab ^= 1;
        memset(&SoundBuffer[whichab], 0, AUDIOBUFFER);

        if(module) {
            setBuffer((s16 *)&tempbuffer, AUDIOBUFFER>>3);
            MikMod_Update();
        }

        if(Player_Paused()) {
            memset(tempbuffer, 0, AUDIOBUFFER);
        }
        else {
            count = AUDIOBUFFER >> 3;
            src = (u32 *)&tempbuffer;
            dst = (u32 *)&SoundBuffer[whichab];

            while ( count ) {
                *dst++ = *src;
                *dst++ = *src++;
                count--;
            }
        }
    }
    else {
        memset(tempbuffer, 0, AUDIOBUFFER);
    }
}

/**
 * This function has the same behaviour as feof.
 */
static BOOL GRRMOD_Eof(MREADER * reader)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    return (pReader->Size == (pReader->Offset)) ? true : false;
}

/**
 * This function copies length bytes of data into dest, and return zero if an error occured, and any nonzero value otherwise. Note that an end-of-file condition will not be considered as an error in this case.
 */
static BOOL GRRMOD_Read(MREADER * reader, void *ptr, size_t size)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    memcpy(ptr, pReader->BufferPtr + pReader->Offset, size);
    pReader->Offset += size;

    return 1;
}

/**
 * This function has the same behaviour as fgetc.
 */
static int GRRMOD_Get(MREADER * reader)
{
    MOD_READER *pReader = (MOD_READER *) reader;
    char buf;

    buf = *(pReader->BufferPtr + pReader->Offset);
    pReader->Offset++;

    return((int)buf);
}

/**
 * This function has the same behaviour as fseek, with offset 0 meaning the start of the object (module, sample) being loaded.
 */
static BOOL GRRMOD_Seek(MREADER * reader, long offset, int whence)
{
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
static long GRRMOD_Tell(MREADER * reader)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    return pReader->Offset;
}
