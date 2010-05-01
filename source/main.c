/*===========================================
        GRRMOD
        - Test Code -
============================================*/
#include <grrlib.h>
#include "grrmod.h"

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
static float calc_size(u8 voice, CH* channel);


int main(int argc, char **argv) {
    float a = 0.0f;
    u8 Volume = 64;

    GRRLIB_Init();

    GRRMOD_Init();

    GRRMOD_SetMOD(music_mod, music_mod_size);

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
        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS) {
            GRRMOD_SetVolume(++Volume);
        }
        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS) {
            GRRMOD_SetVolume(--Volume);
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

    GRRMOD_End();
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}

static float calc_size(u8 voice, CH* channel) {
    int freq = GRRMOD_GetVoiceFrequency(voice);
    int vol = GRRMOD_GetVoiceVolume(voice);
    int realvol = GRRMOD_GetRealVoiceVolume(voice);

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
