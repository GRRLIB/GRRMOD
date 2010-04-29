/*===========================================
        GRRMOD
        - Test Code -
============================================*/
#include <grrlib.h>
#include "mikmod_build.h"

#include <stdlib.h>
#include <wiiuse/wpad.h>

// Mod file
#include "music_mod.h"
#include "music_xm.h"
#include "music_s3m.h"
#include "music_it.h"

#define MAX_WIDTH 6.0f
#define MIN_WIDTH 0.2f
#define DECAY 0.5f;

typedef struct
{
    int freq;
    int vol;
    int realvol;
    float width;
} CH;

static CH channel1 = {0, 0, 0, MIN_WIDTH};
static CH channel2 = {0, 0, 0, MIN_WIDTH};
static CH channel3 = {0, 0, 0, MIN_WIDTH};
static CH channel4 = {0, 0, 0, MIN_WIDTH};


typedef struct _MOD_READER
{
    MREADER Core;
    u64     Offset;
    char    *BufferPtr;
    u64     Size;
} MOD_READER;

static MODULE *module;

static BOOL GRRMOD_Eof(MREADER * reader);
static BOOL GRRMOD_Read(MREADER * reader, void *ptr, size_t size);
static int GRRMOD_Get(MREADER * reader);
static BOOL GRRMOD_Seek(MREADER * reader, long offset, int whence);
static long GRRMOD_Tell(MREADER * reader);


#define AUDIOBUFFER 4096
u8  SoundBuffer[2][AUDIOBUFFER]  __attribute__((__aligned__(32)));
u8  tempbuffer[AUDIOBUFFER];
int whichab = 0;
static int playing = 0;

static void sound_callback()
{
    u32 *src;
    u32 *dst;
    int count;

    if (playing)
    {
        AUDIO_StopDMA();
        AUDIO_InitDMA((u32)SoundBuffer[whichab], AUDIOBUFFER);
        DCFlushRange(&SoundBuffer[whichab], AUDIOBUFFER);
        AUDIO_StartDMA();

        whichab ^= 1;
        memset(&SoundBuffer[whichab], 0, AUDIOBUFFER);

        if(module)
        {
            setBuffer((s16 *)&tempbuffer, AUDIOBUFFER>>3);
            MikMod_Update();
        }

        count = AUDIOBUFFER >> 3;
        src = (u32 *)&tempbuffer;
        dst = (u32 *)&SoundBuffer[whichab];

        while ( count )
        {
            *dst++ = *src;
            *dst++ = *src++;
            count--;
        }
    }
    else
    {
        memset(tempbuffer, 0, AUDIOBUFFER);
    }
}

float calc_size(SBYTE voice, CH* channel)
{
    int freq = Voice_GetFrequency(voice);
    int vol = Voice_GetVolume(voice);
    int realvol = Voice_RealVolume(voice);

    if (freq != channel->freq || vol != channel->vol || realvol > channel->realvol)
    {
        channel->width = MAX_WIDTH;
    }
    else
    {
        channel->width -= DECAY;
        if (channel->width < MIN_WIDTH)
            channel->width = MIN_WIDTH;
    }

    channel->vol = vol;
    channel->freq = freq;
    channel->realvol = realvol;

    return channel->width;
}


int main(int argc, char **argv) {
    float a = 0.0f;

    GRRLIB_Init();

    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();

    md_mode = 0;
    md_mode |= DMODE_16BITS;
    md_mode |= DMODE_SOFT_MUSIC;
    md_mode |= DMODE_SOFT_SNDFX;
    //md_mode |= DMODE_STEREO;  //this causes some modules (s3m mostly) to play back incorrectly on wii, i dont know why
    md_mode |= DMODE_HQMIXER;

    md_mixfreq = 24000;

    if(MikMod_Init(""))
    {
        exit(0);
    }

    MOD_READER *Reader = (MOD_READER *)malloc(sizeof (MOD_READER));
    if(Reader)
    {
        Reader->Offset = 0;
        Reader->BufferPtr = (char *)music_mod; // Buffer
        Reader->Size = music_mod_size;   // File size
        Reader->Core.Eof = &GRRMOD_Eof;
        Reader->Core.Read = &GRRMOD_Read;
        Reader->Core.Get = &GRRMOD_Get;
        Reader->Core.Seek = &GRRMOD_Seek;
        Reader->Core.Tell = &GRRMOD_Tell;
    }
    module = Player_LoadGeneric((MREADER *)Reader, 256, 0);

    if(module)
    {
        module->wrap = true;    // The module will restart when it's finished
        Player_Start(module);
    }
    else
    {
        exit(0);
    }

    AUDIO_Init(NULL);       /*** Start audio subsystem ***/
    AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ); /*** Set default samplerate to 48khz ***/
    AUDIO_RegisterDMACallback( (void *)sound_callback );/*** and the DMA Callback ***/

    memset(&SoundBuffer[0], 0, AUDIOBUFFER);
    DCFlushRange((char *)&SoundBuffer[0], AUDIOBUFFER);

    memset(&SoundBuffer[1], 0, AUDIOBUFFER);
    DCFlushRange((char *)&SoundBuffer[1], AUDIOBUFFER);

    playing=1;
    sound_callback();

    WPAD_Init();

    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);

    // Loop forever
    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();  // Scan the Wiimotes

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
            break;
        }

        GRRLIB_Camera3dSettings(0.0f, 0.0f,13.0f, 0,1,0, 0,0,0);

        GRRLIB_SetLightAmbient(0x333333FF);
        GRRLIB_SetLightDiff(0,(guVector){0.0f,0.0f,0.0f},20.0f,1.0f,0x00FFFFFF);
        GRRLIB_SetLightDiff(1,(guVector){0.0f,13.0f,3.0f},20.0f,1.0f,0xFF00FFFF);
        GRRLIB_SetLightDiff(2,(guVector){0.0f,-13.0f,3.0f},20.0f,1.0f,0xFFFF00FF);
        GRRLIB_SetLightDiff(3,(guVector){13.0f,0.0f,3.0f},20.0f,1.0f,0xFF0000FF);
        GRRLIB_SetLightDiff(4,(guVector){-13.0f,0.0f,3.0f},20.0f,1.0f,0x00FF00FF);

        GRRLIB_3dMode(0.1,1000,45,0,1);

	GRRLIB_ObjectViewInv(-3.0f, 0.0f, 0.0f, a,a*2,a*3, 1.0f,calc_size(0, &channel1),1.0f);
        GRRLIB_DrawCube(1.0,true,0xFFFFFFFF);

	GRRLIB_ObjectViewInv(-1.0f, 0.0f, 0.0f, a,a*2,a*3, 1.0f,calc_size(1, &channel2),1.0f);
        GRRLIB_DrawCube(1.0,true,0xFFFFFFFF);

	GRRLIB_ObjectViewInv(1.0f, 0.0f, 0.0f, a,a*2,a*3, 1.0f,calc_size(2, &channel3),1.0f);
        GRRLIB_DrawCube(1.0,true,0xFFFFFFFF);

	GRRLIB_ObjectViewInv(3.0f, 0.0f, 0.0f, a,a*2,a*3, 1.0f,calc_size(3, &channel4),1.0f);
        GRRLIB_DrawCube(1.0,true,0xFFFFFFFF);


        a+=0.5f;
        GRRLIB_2dMode();

        GRRLIB_Render();  // Render the frame buffer to the TV
    }

    // Kill module renderer if it exists
    if(module)
    {
        Player_Stop();
        Player_Free(module);
        MikMod_Exit();
    }
    AUDIO_StopDMA();
    AUDIO_RegisterDMACallback(NULL);

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}

static BOOL GRRMOD_Eof(MREADER * reader)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    return (pReader->Size == (pReader->Offset)) ? true : false;
}

static BOOL GRRMOD_Read(MREADER * reader, void *ptr, size_t size)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    memcpy(ptr, pReader->BufferPtr + pReader->Offset, size);
    pReader->Offset += size;

    return 1;
}

static int GRRMOD_Get(MREADER * reader)
{
    MOD_READER *pReader = (MOD_READER *) reader;
    char buf;

    buf = *(pReader->BufferPtr + pReader->Offset);
    pReader->Offset++;

    return((int)buf);
}

static BOOL GRRMOD_Seek(MREADER * reader, long offset, int whence)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    if(whence == SEEK_SET)
        pReader->Offset = offset;
    else
        pReader->Offset += offset;

    return 1;
}

static long GRRMOD_Tell(MREADER * reader)
{
    MOD_READER *pReader = (MOD_READER *) reader;

    return pReader->Offset;
}
