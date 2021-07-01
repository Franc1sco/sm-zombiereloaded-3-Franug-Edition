/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:		  roundendsounds.inc
 *  Type:		  Core 
 *  Description:   Round End sound effects.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * @section Defines for min/max round wim human sound file index.
 */
#define SOUND_ROUNDEND_MIN 1
#define SOUND_ROUNDEND_MAX 4

/**
 * @endsection
 */

/**
 * Zombie sound types
 */
enum RoundEndSounds
{
	HumansWinCt, /** Human Wim */
	ZombisWimT, /** Zombie's Win */
}

void RoundEndSoundsStart(int winner)
{
	if (!g_hCvarsList.g_bCVAR_ROUNDEND_SOUNDS)
	{
		return;
	}
	
	// Get Volume Cvar
	float LevelVolume = g_hCvarsList.g_fCVAR_ROUNDEND_VOLUME;
	
	switch(winner)
	{
		// Show "zombies win" RoundSoundsWim
		case CS_TEAM_T:
		{	
			// Get random death sound.
			char soundre[SOUND_MAX_PATH];
			RoundEndSoundsGetRandomSound(soundre, ZombisWimT);

			// x = client index.
			for (int x = 1; x <= MaxClients; x++)
			{
				// If client isn't in-game, then stop.
				if (!IsClientInGame(x))
				{
					continue;
				}
				
				// If client is fake (or bot), then stop.
				if (IsFakeClient(x))
				{
					continue;
				}
				// From Cliente Round end sound Zombis Wim.
				RoundEndSoundsSoundFromClient(x, soundre, LevelVolume);
			}
		}
		// Show "humans win" RoundSoundsWim
		case CS_TEAM_CT:
		{
			// Get random death sound.
			char soundre[SOUND_MAX_PATH];
			RoundEndSoundsGetRandomSound(soundre, HumansWinCt);

			// x = client index.
			for (int x = 1; x <= MaxClients; x++)
			{
				// If client isn't in-game, then stop.
				if (!IsClientInGame(x))
				{
					continue;
				}
				
				// If client is fake (or bot), then stop.
				if (IsFakeClient(x))
				{
					continue;
				}
				// From Cliente Round end sound Humans Wim.
				RoundEndSoundsSoundFromClient(x, soundre, LevelVolume);
			}
		}
		// Show no overlay.
		default:
		{
			//strcopy(overlaypath, sizeof(overlaypath), "");
		}
	}
}

bool RoundEndSoundsGetRandomSound(char[] soundre, RoundEndSounds soundtype)
{
	int soundroundmin = SOUND_ROUNDEND_MIN;
	int soundroundmax = SOUND_ROUNDEND_MAX;

	// Pick a random integer between min and max sound file index.
	int randresound = GetRandomInt(soundroundmin, soundroundmax);
	
	switch(soundtype)
	{
		// Find HumansWin sound.
		case HumansWinCt:
		{
			// Format random index into sound path.
			Format(soundre, SOUND_MAX_PATH, "%shumans_win%d.mp3", g_hCvarsList.g_sCVAR_ROUNDEND_HUMANS_WIN, randresound);
		}
		// Find ZombisWim sound.
		case ZombisWimT:
		{
			// Format random index into sound path.
			Format(soundre, SOUND_MAX_PATH, "%szombie_win%d.mp3", g_hCvarsList.g_sCVAR_ROUNDEND_ZOMBIE_WIN, randresound);
		}
		// Invalid case given.
		default:
		{
			// No handled case was given.
			return false;
		}
	}
	// Found sound.
	return true;
}

void RoundEndSoundsSoundFromClient(int client, const char[] soundre, float level = 1.0)
{
	if (IsClientInGame(client) && !IsFakeClient(client))
	{
		// Emit sound from client.
		if (g_bZRoundEndCookie[client]) EmitSoundToClient(client, soundre, _, SNDCHAN_AUTO, _, _, level);
	}
}