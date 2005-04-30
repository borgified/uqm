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

#include "intel.h"

#include "controls.h"
#include "globdata.h"
#include "setup.h"

#include <stdio.h>


SIZE cur_player;

BATTLE_INPUT_STATE
computer_intelligence (void)
{
	BATTLE_INPUT_STATE InputState;

	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
		return 0;

	if (CyborgDescPtr)
	{
		if (PlayerControl[cur_player] & CYBORG_CONTROL)
		{
			InputState = tactical_intelligence ();

			// allow a player to warp-escape in cyborg mode
			if (cur_player == 0)
				InputState |= (*(HumanInput[cur_player])) () & BATTLE_ESCAPE;
		}
		else
			InputState = (*(HumanInput[cur_player])) ();
	}
	else if (!(PlayerControl[cur_player] & PSYTRON_CONTROL))
		InputState = 0;
	else
	{
		switch (LOBYTE (GLOBAL (CurrentActivity)))
		{
			case SUPER_MELEE:
			{
				SleepThread (ONE_SECOND >> 1);
				InputState = BATTLE_WEAPON; /* pick a random ship */
				break;
			}
			default:
				// Should not happen. Satisfying compiler.
				fprintf (stderr, "Warning: Unexpected state in "
						"computer_intelligence().");
				InputState = 0;
				break;
		}
	}
	return InputState;
}


