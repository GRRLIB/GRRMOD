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


#define AUDIOBUFFER 2048
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
        //so.samples_mixed_so_far = so.play_position = 0;

//YmMusicCumpute c la lib que jai porter qui prepare le buffer, nous ca sera mikmod qui le fera.
//ymMusicCompute((void *)pMusic, (ymsample *)&tempbuffer,AUDIOBUFFER >> 2);

/* ?????????????????????????
        if(module)
        {
            setBuffer((s16 *)music_mod, music_mod_size);
            MikMod_Update();
            Player_Active();
        }
*/
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

int main(int argc, char **argv) {
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

	// Register all the drivers
	MikMod_RegisterAllDrivers();
	// Register all the module loaders
	MikMod_RegisterAllLoaders();

	// Initialize the library

	// Init mikmod
	md_mode = 0;
	md_mode |= DMODE_16BITS;
	md_mode |= DMODE_SOFT_MUSIC;
	md_mode |= DMODE_SOFT_SNDFX;
	//md_mode |= DMODE_STEREO;  this causes some modules (s3m mostly) to play back incorrectly on wii, i dont know why
	md_mode |= DMODE_HQMIXER;

	md_mixfreq = 48000;

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

    // Load module
    if(module)
    {
        // start module
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
    DCFlushRange((char *)&SoundBuffer[0], AUDIOBUFFER);

    playing=1;
    //on l'appel une fois pour demarer le bordel ;)
    sound_callback();



    // Initialise the Wiimotes
    WPAD_Init();

    // Loop forever
    while(1) {
        WPAD_ScanPads();  // Scan the Wiimotes

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
            break;
        }
/* ?????????????????????????
        if(module)
        {
            MikMod_Update();
        }
*/
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