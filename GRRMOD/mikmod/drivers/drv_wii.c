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

#include <gctypes.h>
#include "mikmod_internals.h"

static int buffersize=0;
static int *audiobuffer=NULL;

static BOOL WII_IsThere(void)
{
	return TRUE;
}

static int WII_Init(void)
{
	if(audiobuffer==NULL)
	{
		return 1;
	}
	return VC_Init();
}

static void WII_CommandLine(const CHAR *cmdline)
{
	CHAR *ptr=MD_GetAtom("buffer",cmdline,FALSE);
	if (ptr) {
		audiobuffer = (void *)atoi(ptr);
		free(ptr);
	}
	ptr=MD_GetAtom("size",cmdline,FALSE);
	if (ptr) {
		buffersize = atoi(ptr);
		free(ptr);
	}
}

static void	WII_Update(void)
{
	SBYTE* buffer = (SBYTE*)(*audiobuffer);
	if(buffer!=NULL)
	{
		VC_WriteBytes(buffer,buffersize);
	}
}

MIKMODAPI MDRIVER drv_wii={
	NULL,
	"Wii",
	"Wii Driver v1.0",
	0,255,
	"wii",
	"buffer:r:0:Audio buffer pointer\n"
		"size:r:5760:Audio buffer size\n",
	WII_CommandLine,
	WII_IsThere,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	WII_Init,
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
