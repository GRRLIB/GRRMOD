/*	MikMod sound library
	(c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is free software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================

  $Id$

  Driver for Nintendo Wii

==============================================================================*/

/*

	Written by GRRLIB Team

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <gctypes.h>
#include "mikmod_internals.h"

static int sLen=0;
static SBYTE *audiobuffer=NULL;

static BOOL WII_IsThere(void)
{
	return 1;
}

static void WII_CommandLine(const CHAR *cmdline)
{
	CHAR *ptr=MD_GetAtom("buffer",cmdline,0);

	if (ptr) {
		sLen = atoi(ptr);
		free(ptr);
	}
}

static void	WII_Update(void)
{
	VC_WriteBytes(audiobuffer,sLen);
}

void setBuffer(int16_t *buffer, int renderSamples)
{
	audiobuffer = (SBYTE *)buffer;
}

MIKMODAPI MDRIVER drv_nos={
	NULL,
	"Wii",
	"Wii Driver v1.0",
	0,255,
	"wii",
	"buffer:r:5760:Audio buffer size\n",
	WII_CommandLine,
	WII_IsThere,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	VC_Init,
	VC_Exit,
	NULL,
	VC_SetNumVoices,
	VC_PlayStart,
	VC_PlayStop,
	WII_Update,
	NULL,
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};

/* ex:set ts=4: */
