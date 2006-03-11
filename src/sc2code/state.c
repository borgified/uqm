//Copyright Paul Reiche, Fred Ford. 1992-2002

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "state.h"

#include "encount.h"
#include "libs/misc.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

// in-memory file i/o
struct GAME_STATE_FILE
{
	const char *symname;
	DWORD size_hint;
	int   open_count;
	BYTE *data;
	DWORD used;
	DWORD size;
	DWORD ptr;
};
#define STATE_FILE_ITRAILER  0, 0, 0, 0, 0

#define NUM_STATE_FILES 3

static GAME_STATE_FILE state_files[NUM_STATE_FILES] =
{
	{"STARINFO",    STAR_BUFSIZE, STATE_FILE_ITRAILER},
	{"RANDGRPINFO", RAND_BUFSIZE, STATE_FILE_ITRAILER},
	{"DEFGRPINFO",  DEF_BUFSIZE,  STATE_FILE_ITRAILER}
};


GAME_STATE_FILE *
OpenStateFile (int stateFile, const char *mode)
{
	GAME_STATE_FILE *fp;

	if (stateFile < 0 || stateFile >= NUM_STATE_FILES)
		return NULL_PTR;
	
	fp = &state_files[stateFile];
	fp->open_count++;
	if (fp->open_count > 1)
		fprintf (stderr, "WARNING: "
				"State file %s open count is %d after open()\n",
				fp->symname, fp->open_count);
	
	if (!fp->data)
	{
		fp->data = HMalloc (fp->size_hint);
		if (!fp->data)
			return NULL_PTR;
		fp->size = fp->size_hint;
	}

	// we allow reading and writing for any open mode
	// but the mode determines what happens to the file contents
	if (mode[0] == 'w')
	{	// blow the file away
		fp->used = 0;
#ifdef DEBUG
		// paint buffer for tracking writes
		memset (fp->data, 0xCC, fp->size);
#endif
	}
	else if (mode[0] == 'r')
	{	// nothing
	}
	else
	{
		fprintf (stderr, "WARNING: "
				"State file %s opened with unsupported mode '%s'\n",
				fp->symname, mode);
	}
	fp->ptr = 0;
	
	return fp;
}

void
CloseStateFile (GAME_STATE_FILE *fp)
{
	fp->ptr = 0;
	fp->open_count--;
	if (fp->open_count < 0)
		fprintf (stderr, "WARNING: "
				"State file %s open count is %d after close()\n",
				fp->symname, fp->open_count);
	// Erm, Ok, it's closed! Honest!
}

void
DeleteStateFile (int stateFile)
{
	GAME_STATE_FILE *fp;

	if (stateFile < 0 || stateFile >= NUM_STATE_FILES)
		return;

	fp = &state_files[stateFile];
	if (fp->open_count != 0)
		fprintf (stderr, "WARNING: "
				"State file %s open count is %d during delete()\n",
				fp->symname, fp->open_count);

	fp->used = 0;
	fp->ptr = 0;
}

DWORD
LengthStateFile (GAME_STATE_FILE *fp)
{
	return fp->used;
}

int
ReadStateFile (PVOID lpBuf, COUNT size, COUNT count, GAME_STATE_FILE *fp)
{
	DWORD bytes = size * count;

	if (fp->ptr >= fp->size)
	{	// EOF
		return 0;
	}
	else if (fp->ptr + bytes > fp->size)
	{	// dont have that much data
		bytes = fp->size - fp->ptr;
		bytes -= bytes % size;
	}
	
	if (bytes > 0)
	{
		memcpy (lpBuf, fp->data + fp->ptr, bytes);
		fp->ptr += bytes;
	}
	return (bytes / size);
}

int
WriteStateFile (PVOID lpBuf, COUNT size, COUNT count, GAME_STATE_FILE *fp)
{
	DWORD bytes = size * count;
	
	if (fp->ptr + bytes > fp->size)
	{	// dont have that much space available
		DWORD newsize = fp->ptr + bytes;
		// grab more space in advance
		if (newsize < fp->size * 3 / 2)
			newsize = fp->size * 3 / 2;

		fp->data = HRealloc (fp->data, newsize);
		if (!fp->data)
			return 0;
		
		fp->size = newsize;
		if (newsize > fp->size_hint)
			fp->size_hint = newsize;
	}
	
	if (bytes > 0)
	{
		memcpy (fp->data + fp->ptr, lpBuf, bytes);
		fp->ptr += bytes;
		if (fp->ptr > fp->used)
			fp->used = fp->ptr;
	}
	return (bytes / size);
}

int
PutStateFileChar (char ch, GAME_STATE_FILE *fp)
{
	if (1 == WriteStateFile (&ch, sizeof(ch), 1, fp))
		return ch;
	else
		return EOF;
}

int
SeekStateFile (GAME_STATE_FILE *fp, long offset, int whence)
{
	if (whence == SEEK_CUR)
		offset += fp->ptr;
	else if (whence == SEEK_END)
		offset += fp->used;

	if (offset < 0)
	{
		fp->ptr = 0;
		return 0;
	}
	fp->ptr = offset;
	return 1;
}


void
InitPlanetInfo (void)
{
	GAME_STATE_FILE *fp;

	fp = OpenStateFile (STARINFO_FILE, "wb");
	if (fp)
	{
		DWORD offset;
		STAR_DESCPTR pSD;

		offset = 0;
		pSD = &star_array[0];
		do
		{
			WriteStateFile (&offset, sizeof (offset), 1, fp);
			++pSD;
		} while (pSD->star_pt.x <= MAX_X_UNIVERSE
				&& pSD->star_pt.y <= MAX_Y_UNIVERSE);

		CloseStateFile (fp);
	}
}

void
UninitPlanetInfo (void)
{
	DeleteStateFile (STARINFO_FILE);
}

void
GetPlanetInfo (void)
{
	PVOID fp;

	pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN] = 0;
	pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[MINERAL_SCAN] = 0;
	pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] = 0;

	fp = OpenStateFile (STARINFO_FILE, "rb");
	if (fp)
	{
		COUNT star_index, planet_index, moon_index;
		DWORD offset;

		star_index = (COUNT)(CurStarDescPtr - star_array);
		planet_index = (COUNT)(pSolarSysState->pBaseDesc->pPrevDesc
				- pSolarSysState->PlanetDesc);
		if (pSolarSysState->pOrbitalDesc->pPrevDesc == pSolarSysState->SunDesc)
			moon_index = 0;
		else
			moon_index = (COUNT)(pSolarSysState->pOrbitalDesc
					- pSolarSysState->MoonDesc + 1);

		SeekStateFile (fp, star_index * sizeof (offset), SEEK_SET);
		ReadStateFile (&offset, sizeof (offset), 1, fp);

		if (offset)
		{
			COUNT i;

			for (i = 0; i < planet_index; ++i)
				offset += sizeof (
						pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
						) * (pSolarSysState->PlanetDesc[i].NumPlanets + 1);
				
			offset += sizeof (
					pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
					) * moon_index;

			SeekStateFile (fp, offset, SEEK_SET);
			ReadStateFile (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask,
						sizeof (
						pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
						), 1, fp);
		}

		CloseStateFile (fp);
	}
}

void
PutPlanetInfo (void)
{
	PVOID fp;

	fp = OpenStateFile (STARINFO_FILE, "r+b");
	if (fp)
	{
		COUNT i;
		COUNT star_index, planet_index, moon_index;
		DWORD offset;

		star_index = (COUNT)(CurStarDescPtr - star_array);
		planet_index = (COUNT)(pSolarSysState->pBaseDesc->pPrevDesc
				- pSolarSysState->PlanetDesc);
		if (pSolarSysState->pOrbitalDesc->pPrevDesc == pSolarSysState->SunDesc)
			moon_index = 0;
		else
			moon_index = (COUNT)(pSolarSysState->pOrbitalDesc
					- pSolarSysState->MoonDesc + 1);

		SeekStateFile (fp, star_index * sizeof (offset), SEEK_SET);
		ReadStateFile (&offset, sizeof (offset), 1, fp);

		if (offset == 0)
		{
			DWORD ScanRetrieveMask[NUM_SCAN_TYPES] =
			{
				0, 0, 0,
			};

			offset = LengthStateFile (fp);

			SeekStateFile (fp, star_index * sizeof (offset), SEEK_SET);
			WriteStateFile (&offset, sizeof (offset), 1, fp);

			SeekStateFile (fp, offset, SEEK_SET);
			for (i = 0; i < pSolarSysState->SunDesc[0].NumPlanets; ++i)
			{
				COUNT j;

				WriteStateFile (ScanRetrieveMask, sizeof (ScanRetrieveMask), 1, fp);
				for (j = 0; j < pSolarSysState->PlanetDesc[i].NumPlanets; ++j)
					WriteStateFile (ScanRetrieveMask, sizeof (ScanRetrieveMask), 1, fp);
			}
		}

		for (i = 0; i < planet_index; ++i)
			offset += sizeof (
					pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
					) * (pSolarSysState->PlanetDesc[i].NumPlanets + 1);
				
		offset += sizeof (
				pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
				) * moon_index;

		SeekStateFile (fp, offset, SEEK_SET);
		WriteStateFile (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask,
				sizeof (
				pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
				), 1, fp);

		CloseStateFile (fp);
	}
}

