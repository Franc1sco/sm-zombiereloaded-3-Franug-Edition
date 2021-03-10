/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:		  volumecontrol.inc
 *  Type:		  Core 
 *  Description:   Basic volume control for player to change.
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
 * Define cookies for client.
 */

#define COUNTDOWN_COOKIE_VOLUME "zr_countdownvolume"
#define ZOMBIE_COOKIE_VOLUME "zr_zombievolume"
#define AMBIENT_COOKIE_VOLUME "zr_ambientvolume"

#define SOUND_MAX_PATH 128

/**
 * Menu Cookies Handler.
 */
Handle zhCountdownVolume = INVALID_HANDLE;
Handle zhZSoundVolume = INVALID_HANDLE;
Handle zhZAmbientVolume = INVALID_HANDLE;


/**
 * Client Volume.
 */

float zfZSoundVolume[MAXPLAYERS+1];
float zfCountdownVolume[MAXPLAYERS+1];
float zfAmbientVolume[MAXPLAYERS+1];

void VolumeOnCommandCreate()
{
	RegConsoleCmd("sm_zvol", Command_ZVolume, "Brings up the zvolume menu");
	RegConsoleCmd("sm_zvolume", Command_ZVolume, "Brings up the zvolume menu");
	RegConsoleCmd("sm_zcount", Command_Countdown, "Brings up the countdown menu");
	RegConsoleCmd("sm_countdown_volume", Command_Countdown, "Brings up the countdown menu");
	RegConsoleCmd("sm_zsound", Command_ZSound, "Brings up the zsound menu");
	RegConsoleCmd("sm_zombie_volume", Command_ZSound, "Brings up the zsound menu");
	RegConsoleCmd("sm_zambient", Command_ZAmbient, "Brings up the ambient menu");
	RegConsoleCmd("sm_ambient_volume", Command_ZAmbient, "Brings up the ambient menu");
}

void VolumeOnCookiesCreate()
{
	if (zhCountdownVolume == INVALID_HANDLE) 
		zhCountdownVolume = RegClientCookie(COUNTDOWN_COOKIE_VOLUME, "Control Countdown Sound.", CookieAccess_Protected);

	if (zhZSoundVolume == INVALID_HANDLE) 
		zhZSoundVolume = RegClientCookie(ZOMBIE_COOKIE_VOLUME, "Control Zombie Sound.", CookieAccess_Protected);

	if (zhZAmbientVolume == INVALID_HANDLE) 
		zhZAmbientVolume = RegClientCookie(AMBIENT_COOKIE_VOLUME, "Control Ambient Sound.", CookieAccess_Protected);
}

void VolumeOnCookiesCached(int client)
{
	char sValue[8];

	sValue[0] = '\0';
	GetClientCookie(client, zhZSoundVolume, sValue, sizeof(sValue));
	if (sValue[0] == '\0')
	{
		SetClientCookie(client, zhZSoundVolume, "100");
		strcopy(sValue, sizeof(sValue), "100");
	}
	SetZombieVoiceVolume(client, StringToInt(sValue));
	
	sValue[0] = '\0';
	GetClientCookie(client, zhCountdownVolume, sValue, sizeof(sValue));
	if (sValue[0] == '\0') 
	{
		SetClientCookie(client, zhCountdownVolume, "100");
		strcopy(sValue, sizeof(sValue), "100");
	}
	SetCountDownVolume(client, StringToInt(sValue));
	
	sValue[0] = '\0';
	GetClientCookie(client, zhZAmbientVolume, sValue, sizeof(sValue));
	if (sValue[0] == '\0') 
	{
		SetClientCookie(client, zhZAmbientVolume, "100");
		strcopy(sValue, sizeof(sValue), "100");
	}
	SetAmbientVolume(client, StringToInt(sValue));
}

public Action Command_ZVolume(int client, int args)
{
	if(IsClientInGame(client))
	{
		VolumeControlMenu(client);
		return Plugin_Handled;
	}
	return Plugin_Handled;
}

public void VolumeControlMenu(int client)
{
	Menu menu = CreateMenu(VolumeControlMenuHandler, MENU_ACTIONS_DEFAULT);
	
	char sMenuTitle[128];
	Format(sMenuTitle, sizeof(sMenuTitle), "%t", "Volume_Menu_Title");
	menu.SetTitle(sMenuTitle);	
	
	char sItem[64];
	
	Format(sItem, sizeof(sItem), "%t", "Zombie Voice");
	menu.AddItem("1" ,sItem);
	
	Format(sItem, sizeof(sItem), "%t", "Ambient");
	menu.AddItem("2" ,sItem);
	
	Format(sItem, sizeof(sItem), "%t", "Infect Countdown");
	menu.AddItem("3" ,sItem);
	
	menu.ExitBackButton = false;
	menu.Display(client, MENU_TIME_FOREVER);
}

public int VolumeControlMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
	switch(action)
	{
		case MenuAction_Select:
		{
			char info[32];
			menu.GetItem(param2, info, sizeof(info));
			if(StrEqual(info, "1"))
			{
				ZombieVoiceVolumeMenu(param1);
			}
			else if(StrEqual(info, "2"))
			{
				AmbientVolumeMenu(param1);
			}
			else if(StrEqual(info, "3"))
			{
				CountDownVolumeMenu(param1);
			}
		}
		case MenuAction_Cancel:
		{
			if(param2 == MenuCancel_ExitBack)
				ZMenuMain(param1);
		}
		case MenuAction_End:
		{
			delete menu;
		}
	}
	return 0;
}

// Countdown Section
public Action Command_Countdown(int client, int args)
{
	if(args == 0)
	{
		CountDownVolumeMenu(client);
		return Plugin_Handled;
	}
	
	char sArg[32];
	GetCmdArg(1, sArg, sizeof(sArg));
	int iVolume = StringToInt(sArg);
	
	if(iVolume < 0 || iVolume > 100)
	{
		TranslationReplyToCommand(client, "Countdown_Usage");
		return Plugin_Handled;
	}
	else if (0 <= iVolume <= 100)
	{
		SetCountDownVolume(client, iVolume);
		return Plugin_Handled;
	}
	return Plugin_Handled;
}

public void CountDownVolumeMenu(int client)
{
	Menu menu = CreateMenu(CountDownVolumeMenuHandler, MENU_ACTIONS_DEFAULT);
	
	char sMenuTitle[128];
	Format(sMenuTitle, sizeof(sMenuTitle), "%t\n%t", "Countdown_Title_Menu", zfCountdownVolume[client], "Countdown_CommandUsage");
	menu.SetTitle(sMenuTitle);	
	
	menu.AddItem("1" ,"100%");
	menu.AddItem("2" ,"80%");
	menu.AddItem("3" ,"60%");
	menu.AddItem("4" ,"40%");
	menu.AddItem("5" ,"20%");
	
	menu.ExitBackButton = true;
	menu.Display(client, MENU_TIME_FOREVER);
}

public int CountDownVolumeMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
	switch(action)
	{
		case MenuAction_Select:
		{
			char info[32];
			menu.GetItem(param2, info, sizeof(info));
			if(StrEqual(info, "1"))
			{
				SetCountDownVolume(param1, 100);
				CountDownVolumeMenu(param1);
			}
			else if(StrEqual(info, "2"))
			{
				SetCountDownVolume(param1, 80);
				CountDownVolumeMenu(param1);
			}
			else if(StrEqual(info, "3"))
			{
				SetCountDownVolume(param1, 60);
				CountDownVolumeMenu(param1);
			}
			else if(StrEqual(info, "4"))
			{
				SetCountDownVolume(param1, 40);
				CountDownVolumeMenu(param1);
			}
			else if(StrEqual(info, "5"))
			{
				SetCountDownVolume(param1, 20);
				CountDownVolumeMenu(param1);
			}
		}
		case MenuAction_Cancel:
		{
			if(param2 == MenuCancel_ExitBack)
				VolumeControlMenu(param1);
		}
		case MenuAction_End:
		{
			delete menu;
		}
	}
	return 0;
}

public void SetCountDownVolume(int client, int volume)
{
	zfCountdownVolume[client] = volume * 0.01;
	
	char sValue[8];
	IntToString(volume, sValue, sizeof(sValue));
	SetClientCookie(client, zhCountdownVolume, sValue);
	
	if(IsClientInGame(client))
		TranslationPrintToChat(client, "Set_Countdown_Volume", volume);
}

// Zombie Voice Section
public Action Command_ZSound(int client, int args)
{
	if(args == 0)
	{
		ZombieVoiceVolumeMenu(client);
		return Plugin_Handled;
	}
	
	char sArg[32];
	GetCmdArg(1, sArg, sizeof(sArg));
	int iVolume = StringToInt(sArg);
	
	if(iVolume < 0 || iVolume > 100)
	{
		TranslationReplyToCommand(client, "ZombieVoice_Usage");
		return Plugin_Handled;
	}
	else if (0 <= iVolume <= 100)
	{
		SetZombieVoiceVolume(client, iVolume);
		return Plugin_Handled;
	}
	return Plugin_Handled;
}

public void ZombieVoiceVolumeMenu(int client)
{
	Menu menu = CreateMenu(ZombieVoiceVolumeMenuHandler, MENU_ACTIONS_DEFAULT);
	
	char sMenuTitle[128];
	Format(sMenuTitle, sizeof(sMenuTitle), "%t\n%t", "ZombieVoice_Title_Menu", zfZSoundVolume[client], "ZombieVoice_CommandUsage");
	menu.SetTitle(sMenuTitle);	
	
	menu.AddItem("1" ,"100%");
	menu.AddItem("2" ,"80%");
	menu.AddItem("3" ,"60%");
	menu.AddItem("4" ,"40%");
	menu.AddItem("5" ,"20%");
	
	menu.ExitBackButton = true;
	menu.Display(client, MENU_TIME_FOREVER);
}

public int ZombieVoiceVolumeMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
	switch(action)
	{
		case MenuAction_Select:
		{
			char info[32];
			menu.GetItem(param2, info, sizeof(info));
			if(StrEqual(info, "1"))
			{
				SetZombieVoiceVolume(param1, 100);
				ZombieVoiceVolumeMenu(param1);
			}
			else if(StrEqual(info, "2"))
			{
				SetZombieVoiceVolume(param1, 80);
				ZombieVoiceVolumeMenu(param1);
			}
			else if(StrEqual(info, "3"))
			{
				SetZombieVoiceVolume(param1, 60);
				ZombieVoiceVolumeMenu(param1);
			}
			else if(StrEqual(info, "4"))
			{
				SetZombieVoiceVolume(param1, 40);
				ZombieVoiceVolumeMenu(param1);
			}
			else if(StrEqual(info, "5"))
			{
				SetZombieVoiceVolume(param1, 20);
				ZombieVoiceVolumeMenu(param1);
			}
		}
		case MenuAction_Cancel:
		{
			if(param2 == MenuCancel_ExitBack)
				VolumeControlMenu(param1);
		}
		case MenuAction_End:
		{
			delete menu;
		}
	}
	return 0;
}

public void SetZombieVoiceVolume(int client, int volume)
{
	zfZSoundVolume[client] = volume * 0.01;
	
	char sValue[8];
	IntToString(volume, sValue, sizeof(sValue));
	SetClientCookie(client, zhZSoundVolume, sValue);
	
	if(IsClientInGame(client))
		TranslationPrintToChat(client, "Set_ZombieVoice_Volume", volume);
}

// Ambient Section
public Action Command_ZAmbient(int client, int args)
{
	if(args == 0)
	{
		AmbientVolumeMenu(client);
		return Plugin_Handled;
	}
	
	char sArg[32];
	GetCmdArg(1, sArg, sizeof(sArg));
	int iVolume = StringToInt(sArg);
	
	if(iVolume < 0 || iVolume > 100)
	{
		TranslationReplyToCommand(client, "Ambient_Usage");
		return Plugin_Handled;
	}
	else if (0 <= iVolume <= 100)
	{
		SetAmbientVolume(client, iVolume);
		return Plugin_Handled;
	}
	return Plugin_Handled;
}

public void AmbientVolumeMenu(int client)
{
	Menu menu = CreateMenu(AmbientVolumeMenuHandler, MENU_ACTIONS_DEFAULT);
	
	char sMenuTitle[128];
	Format(sMenuTitle, sizeof(sMenuTitle), "%t\n%t", "Ambient_Title_Menu", zfAmbientVolume[client], "Ambient_CommandUsage");
	menu.SetTitle(sMenuTitle);	
	
	menu.AddItem("1" ,"100%");
	menu.AddItem("2" ,"80%");
	menu.AddItem("3" ,"60%");
	menu.AddItem("4" ,"40%");
	menu.AddItem("5" ,"20%");
	
	menu.ExitBackButton = true;
	menu.Display(client, MENU_TIME_FOREVER);
}

public int AmbientVolumeMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
	switch(action)
	{
		case MenuAction_Select:
		{
			char info[32];
			menu.GetItem(param2, info, sizeof(info));
			if(StrEqual(info, "1"))
			{
				SetAmbientVolume(param1, 100);
				AmbientVolumeMenu(param1);
			}
			else if(StrEqual(info, "2"))
			{
				SetAmbientVolume(param1, 80);
				AmbientVolumeMenu(param1);
			}
			else if(StrEqual(info, "3"))
			{
				SetAmbientVolume(param1, 60);
				AmbientVolumeMenu(param1);
			}
			else if(StrEqual(info, "4"))
			{
				SetAmbientVolume(param1, 40);
				AmbientVolumeMenu(param1);
			}
			else if(StrEqual(info, "5"))
			{
				SetAmbientVolume(param1, 20);
				AmbientVolumeMenu(param1);
			}
		}
		case MenuAction_Cancel:
		{
			if(param2 == MenuCancel_ExitBack)
				VolumeControlMenu(param1);
		}
		case MenuAction_End:
		{
			delete menu;
		}
	}
	return 0;
}

public void SetAmbientVolume(int client, int volume)
{
	zfAmbientVolume[client] = volume * 0.01;
	
	char sValue[8];
	IntToString(volume, sValue, sizeof(sValue));
	SetClientCookie(client, zhZAmbientVolume, sValue);
	
	if(IsClientInGame(client))
		TranslationPrintToChat(client, "Set_Ambient_Volume", volume);
}