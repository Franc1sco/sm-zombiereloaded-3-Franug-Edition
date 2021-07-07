/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:		  infect.inc
 *  Type:		  Core
 *  Description:   Client infection functions.
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
 * Array for flagging client as zombie.
 */
bool bZombie[MAXPLAYERS + 1];

/**
 * Infects a client. Execute events, sets attributes and flags that indicate
 * that the client is a zombie.
 *
 * @param client			The client to infect.
 * @param attacker		  (Optional) The attacker who did the infect.
 * @param motherinfect	  (Optional) Indicates a mother zombie infect.
 * @param respawnoverride   (Optional) Set to true to override respawn cvar.
 * @param respawn		   (Optional) Value to override with.
 */
public Action ZR_OnClientInfect(int &client, int &attacker, bool &motherInfect, bool &respawnOverride, bool &respawn)
{
	// Mark player as zombie.
	bZombie[client] = true;
	// Emit scream sound if enabled.
	ZombieSoundsScream(client);
	SEffectsOnClientInfected(client);

	return Plugin_Continue;
}

/**
 * Turns a zombie back into a human. Execute events, sets attributes and flags that indicate
 * that the client is a human.
 *
 * @param client		The client to make human.
 * @param respawn	   Teleport client back to spawn if true.
 * @param protect	   Start spawn protection on new human.
 */
public Action ZR_OnClientHuman(int &client, bool &respawn, bool &protect)
{
	// Mark player as human.
	bZombie[client] = false;
	// Forward event to modules.
	SEffectsOnClientHuman(client);

	return Plugin_Continue;
}


/**
 * Client is joining a team.
 * 
 * @param client	The client index.
 * @param team	  The team index. 
 */
void InfectOnClientTeam(int client, int team)
{
	// If client isn't joining spec, then stop.
	if (team != CS_TEAM_SPECTATOR)
	{
		return;
	}
	
	// Disable zombie flag on client.
	bZombie[client] = false;
}


/**
 * Client is spawning into the game.
 * 
 * @param client	The client index.
 */
void InfectOnClientSpawn(int client)
{
	// Disable zombie flag on client.
	bZombie[client] = false;
}

/**
 * The round is ending.
 */
public Action InfectOnRoundEnd()
{
	// x = client index.
	for (int x = 1; x <=  MaxClients; x++)
	{
		// If client isn't in-game, then stop.
		if (!IsClientInGame(x))
		{
			continue;
		}
		
		// Disable zombie flag on client.
		bZombie[x] = false;
	}
}


/**
 * Returns if a client is infected.
 * 
 * @param client	The client index.
 * @return		  True if the client has been infected, false otherwise.
 */	
bool InfectIsClientInfected(int client)
{
	// If client is invalid, then stop.
	if (!ZRIsClientValid(client))
	{
		return false;
	}
	
	// Return client's zombie flag.
	return bZombie[client];
}
