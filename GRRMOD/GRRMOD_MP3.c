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
#include <stdio.h>//to delete
#include "GRRMOD_internals.h"
#include "mpg123.h"
#include <string.h>

#define MP3_READ_SIZE 1024

static u64     Offset;     /**< Current file position. */
static char    *BufferPtr; /**< Pointer to the music data. */
static u64     Size;       /**< Size of the music data. */
static bool    IsStereo;   /**< Set to true is the music is stereo. */

typedef struct _GRRMOD_DATA {
    char *ModType;    /**< A string representing the MOD type. */
    char *SongTitle;  /**< A string representing the song title. */
} GRRMOD_DATA;

static mpg123_handle *mh;
static GRRMOD_DATA MusicData = {};

static long frequency;
static int channels;
static off_t samples;

void GRRMOD_MP3_Register(GRRLIB_FuntionsList *RegFunc) {
    RegFunc->Init = GRRMOD_MP3_Init;
    RegFunc->End = GRRMOD_MP3_End;
    RegFunc->SetMOD = GRRMOD_MP3_SetMOD;
    RegFunc->Unload = GRRMOD_MP3_Unload;
    RegFunc->SetFrequency = GRRMOD_MP3_SetFrequency;
    RegFunc->GetVoiceFrequency = GRRMOD_MP3_GetVoiceFrequency;
    RegFunc->GetVoiceVolume = GRRMOD_MP3_GetVoiceVolume;
    RegFunc->GetRealVoiceVolume = GRRMOD_MP3_GetRealVoiceVolume;
    RegFunc->Start = GRRMOD_MP3_Start;
    RegFunc->Stop = GRRMOD_MP3_Stop;
    RegFunc->Pause = GRRMOD_MP3_Pause;
    RegFunc->GetSongTitle = GRRMOD_MP3_GetSongTitle;
    RegFunc->GetModType = GRRMOD_MP3_GetModType;
    RegFunc->Update = GRRMOD_MP3_Update;
}

/**
 * Initialize MP3 library.
 * @return A number representating a code:
 *         -     0 : The operation completed successfully.
 *         -    -1 : Failed to initialize the MP3 engine.
 * @see GRRMOD_MP3_End
 */
s8 GRRMOD_MP3_Init(bool stereo) {
    IsStereo = stereo;
    if(mpg123_init() == MPG123_OK)
        return 0;
    return -1;
}

/**
 * Call this before exiting your application.
 * Ensure this function is only ever called once.
 */
void GRRMOD_MP3_End() {
    mpg123_exit();
}

/**
 * Load a MP3 file from memory.
 */
void GRRMOD_MP3_SetMOD(const void *mem, u64 size) {
    int result, i;
    int encoding; // Unneeded value encoding
    size_t fakegot;
    size_t num_rates;

    // Set global value
    Offset = 0;
    BufferPtr = (char *)mem;
    Size = size;

    // Get bitrates
    mpg123_rates(NULL, &num_rates);

    // Get new mpg123 handle
    mh = mpg123_new(NULL, &result);

    if(mh == NULL)  return;

    // Streaming mode
    mpg123_open_feed(mh);

    // Ensure that this output format will not change (it could, when we allow it).
    mpg123_format_none(mh);

    // Set all bitrates as ok
    u8 channelcount = IsStereo ? MPG123_STEREO : MPG123_MONO;
    for(i = 0; i < (int)num_rates; ++i) {
        mpg123_format(mh, frequency, channelcount, MPG123_ENC_SIGNED_16);
    }

    result = mpg123_decode(mh, (u8 *)BufferPtr, Size, NULL, 0, &fakegot);

    if(result != MPG123_NEW_FORMAT) {
        // Failed to get data
        mpg123_close(mh);
        mh = NULL;

        // Exit out of here, no recovery
        return;
    }

    // Grab frequency to play back as well as number of channels
    result = mpg123_getformat(mh, &frequency, &channels, &encoding);

    // Set file length
    mpg123_set_filesize(mh, size);

    // Grab length
    samples = mpg123_length(mh);

    char Temp[1024];
/*
    mpg123_id3v1 *v1;
    mpg123_id3v2 *v2;
    //mpg123_scan(mh);
    if(mpg123_meta_check(mh) & MPG123_ID3 && mpg123_id3(mh, &v1, &v2) == MPG123_OK) {
        //memcpy(Temp, v1->title, sizeof(v1->title));
        //Temp[sizeof(v1->title)] = 0;
        //sprintf(Temp, "ID3: %s", v1->title);
        MusicData.SongTitle = strdup(v1->title);
    }
*/
    sprintf(Temp, "MP3: %li Hz, %i channels, encoding value %i\n", frequency, channels, encoding);
    MusicData.ModType = strdup(Temp);
}

/**
 * Unload a MP3 file.
 */
void GRRMOD_MP3_Unload() {
    if(mh) {
        mpg123_delete(mh);
        mh = NULL;
    }
}

/**
 * This function starts the specified module playback.
 */
void GRRMOD_MP3_Start() {
    if(mh == NULL)   return;


}

/**
 * This function stops the currently playing module.
 */
void GRRMOD_MP3_Stop() {
    Offset = 0;
}

/**
 * This function toggles the playing/paused status of the module.
 */
void GRRMOD_MP3_Pause() {

}

/**
 * Get the song title.
 * @return Pointer to the song title.
 */
char *GRRMOD_MP3_GetSongTitle() {
    return MusicData.SongTitle;
}

/**
 * Get the MP3 type.
 * @return Pointer to the MOD type.
 */
char *GRRMOD_MP3_GetModType() {
    return MusicData.ModType;
}

/**
 * Set the frequency. This should be called before GRRMOD_MP3_SetMOD.
 * @param freq Frequency to set in kHz.
 */
void GRRMOD_MP3_SetFrequency(u32 freq) {
    frequency = freq;
}

/**
 * This function returns the frequency of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get frequency.
 * @return The current frequency of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_MP3_GetVoiceFrequency(u8 voice) {
    return 0;
}

/**
 * This function returns the volume of the sample currently playing on the specified voice.
 * @param voice The number of the voice to get volume.
 * @return The current volume of the sample playing on the specified voice, or zero if no sample is currently playing on the voice.
 */
u32 GRRMOD_MP3_GetVoiceVolume(u8 voice) {
    return 0;
}

/**
 * This function returns the actual playing volume of the specified voice.
 * @param voice The number of the voice to analyze (starting from zero).
 * @return The real volume of the voice when the function was called, in the range 0-65535.
 */
u32 GRRMOD_MP3_GetRealVoiceVolume(u8 voice) {
    return 0;
}

/**
 * Set a buffer to update. This routine should be called on a regular basis to update the sound.
 * @param buffer The buffer to update.
 */
void GRRMOD_MP3_Update(u8 *outbuf) {
    if(mh == NULL || outbuf == NULL) {
        return;
    }
    // Clear data to ensure no garbage bytes
    memset(outbuf, 0, SNDBUFFERSIZE);//memset(outbuf, 0, renderSamples * 4);

    // Start with assumption that we have enough data
    int result = MPG123_OK;

    // Remember end of file
    int over = 0;

    // Bookkeeping
    int need = (SNDBUFFERSIZE / 4) * channels * 2;//int need = renderSamples * channels * 2;
    int have_read = 0;

    // Loop, grabbing enough data to get samples
    do {
        // Bookkeeping
        int dataIn = 0;
        size_t have_now = 0;

        // Figure out if we need more data
        if(result == MPG123_NEED_MORE) {
            if(over) {
                // Ensure we don't create garbage audio
                memset(outbuf + have_read, 0, need);
                Offset = 0;
                // More data next time
                return;
                //break;
            }

            // Read in
            if(Offset + MP3_READ_SIZE > Size) {
                dataIn = Size - Offset;
                //over = 1;
            }
            else {
                dataIn = MP3_READ_SIZE;
            }

            if(dataIn < MP3_READ_SIZE) {
                // End of file :<
                over = 1;
            }
        }

        // Grab data
        result = mpg123_decode(mh, (u8 *)(BufferPtr + Offset), dataIn, outbuf + have_read, need, &have_now);

        Offset += dataIn;

        // Ensure we keep track of newly gotten data
        need -= have_now;
        have_read += have_now;

        // if we finished, then exit with success
        if(need == 0) {
            // More data next time
            return;
        }
    } while(result == MPG123_NEED_MORE || result == MPG123_ERR);
}
