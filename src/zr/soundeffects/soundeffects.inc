/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:		  soundeffects.inc
 *  Type:		  Core 
 *  Description:   Basic sound-management API.
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
 * Maximum sound path length.
 */
#define SOUND_MAX_PATH 128

/**
 * Ambient sound channel.
 */
#define SOUND_AMBIENT_CHANNEL 8

#include "zr/soundeffects/voice"
#include "zr/soundeffects/ambientsounds"
#include "zr/soundeffects/zombiesounds"

/**
 * Load sound effects data.
 */
SEffectsLoad()
{
	// Load ambient sound cvars.
	AmbientSoundsLoad();
}

/**
 * Map is starting.
 */ 
SEffectsOnMapStart()
{
	// Forward event to sub-modules.
	AmbientSoundsOnMapStart();
}

/**
 * Client is joining the server.
 * 
 * @param client	The client index.
 */
SEffectsClientInit(client)
{
	// Forward event to sub-modules.
	AmbientSoundsClientInit(client);
	ZombieSoundsClientInit(client);
}

/**
 * The round is starting.
 */
SEffectsOnRoundStart()
{
	// Forward event to sub-modules.
	VoiceOnRoundStart();
	AmbientSoundsOnRoundStart();
}

/**
 * The round is ending.
 */
SEffectsOnRoundEnd()
{
	// Forward event to sub-modules.
	VoiceOnRoundEnd();
	AmbientSoundsOnRoundEnd();
	ZombieSoundsOnRoundEnd();
}

/**
 * Client is spawning into the game.
 * 
 * @param client	The client index.
 */
SEffectsOnClientSpawn(client)
{
	// Forward event to sub-modules.
	VoiceOnClientSpawn(client);
	ZombieSoundsOnClientSpawn(client);
}

/**
 * Client is spawning into the game. *Post
 * 
 * @param client	The client index.
 */
SEffectsOnClientSpawnPost(client)
{
	// Forward event to sub-modules.
	AmbientSoundsOnClientSpawnPost(client);
}

/**
 * Client has been killed.
 * 
 * @param client	The client index.
 */
SEffectsOnClientDeath(client)
{
	// Forward event to sub-modules.
	ZombieSoundsOnClientDeath(client);
}

/**
 * Client has been hurt.
 * 
 * @param client	The client index.
 */
SEffectsOnClientHurt(client)
{
	// Forward event to sub-modules.
	ZombieSoundsOnClientHurt(client);
}

/**
 * Client has been infected.
 * 
 * @param client	The client index.
 */
SEffectsOnClientInfected(client)
{
	// Forward event to sub-modules.
	VoiceOnClientInfected(client);
	ZombieSoundsOnClientInfected(client);
}

/**
 * Client has been turned back human.
 * 
 * @param client	The client index.
 */
SEffectsOnClientHuman(client)
{
	// Forward event to sub-modules.
	VoiceOnClientHuman(client);
}

/**
 * Emits an ambient sound
 * 
 * @param sound		 The path to the sound file (relative to sounds/)
 * @param soundvolume   The volume of the sound (0.0 - 1.0)
 * @param client		(Optional) Client index to play sound to.
 */
SEffectsEmitAmbientSound(const String:sound[], client = -1)
{
	// Precache sound before playing.
	PrecacheSound(sound);
	
	
	if (ZRIsClientValid(client))
	{
		// Emit ambient sound.
		if(zfAmbientVolume[client] > 0)
		{
			EmitSoundToClient(client, sound, SOUND_FROM_PLAYER, SOUND_AMBIENT_CHANNEL, _, _, zfAmbientVolume[client]);
		
			// Flag client that sound is playing.
			bAmbientSoundsIsPlaying[client] = true;
		}
		else
		{
			bAmbientSoundsIsPlaying[client] = false;
		}
	}
	else
	{
		for (new x = 1; x <= MaxClients; x++)
		{
			// If client isn't in-game, then stop.
			if (!IsClientInGame(x))
			{
				continue;
			}
			
			// Emit ambient sound.
			if(zfAmbientVolume[x] > 0)
				EmitSoundToClient(x, sound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, _, _, zfAmbientVolume[x]);
		}
	}
}

/**
 * Stop an ambient sound
 *  
 * @param sound	 The path to the sound file (relative to sounds/) 
 */
SEffectsStopAmbientSound(const String:sound[])
{
	// x = client index.
	for (new x = 1; x <= MaxClients; x++)
	{
		// If client isn't in-game, then stop.
		if (!IsClientInGame(x))
		{
			continue;
		}
		
		// Stop ambient sound.
		if(zfAmbientVolume[x] > 0)
			StopSound(x, SOUND_AMBIENT_CHANNEL, sound);
	}
}

/**
 * Replay an ambient sound
 * 
 * @param sound	 The path to the sound file (relative to sounds/)
 */ 

/**
 * Emits a sound from a client.
 * 
 * @param client	The client index.
 * @param sound	 The sound file relative to the sound/ directory.
 * @param level	 The attenuation of the sound.
 */
SEffectsEmitSoundFromClient(client, const String:sound[], level = SNDLEVEL_NORMAL)
{
	// Precache sound before playing.// Emit sound from client.
	PrecacheSound(sound);
	// Emit sound from client.
	for(new i = 1; i <= MaxClients; i++)
	{
		if(IsClientInGame(i))
		{
			if(zfZSoundVolume[i] > 0) 
				EmitSoundToClient(i, sound, client, _, level, _, zfZSoundVolume[i]);
		}
	}
}
