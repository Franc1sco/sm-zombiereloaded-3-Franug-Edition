/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:		  zrsounds.inc
 *  Type:		  Core
 *  Description:   Zr Sounds Control.
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
 * List of Handle used by the plugin.
 */

Handle g_CountDownCookie = INVALID_HANDLE;
Handle g_ZrSoundsCookie = INVALID_HANDLE;
Handle g_ZombiesECookie = INVALID_HANDLE;
Handle g_ZRoundEndCookie = INVALID_HANDLE;
float g_fClientVol[MAXPLAYERS + 1];

void ZsoundsOnCommandsCreate()
{
	// Register ZSound command.
	RegConsoleCmd(SAYHOOKS_KEYWORD_ZSOUNDS, ZSoundsMenuCommand, "Opens ZR's main menu.");
}

public Action ZSoundsMenuCommand(int client, int argc)
{
	// If client is console, then stop and tell them this feature is for players only.
	if (ZRIsConsole(client))
	{
		TranslationPrintToServer("Must be player");
		return Plugin_Handled;
	}
	
	// Send ZSounds menu.
	ZSoundsMenuMain(client);
	
	// This stops the "Unknown command" message in client's console.
	return Plugin_Handled;
}

void ZrSoundsOnCookiesCreate()
{
	// Create cookie handle if it doesn't exist.
	if (g_CountDownCookie == INVALID_HANDLE)
	{
		g_CountDownCookie = RegClientCookie("zr_voice_count", "Select Voz CountDown.", CookieAccess_Protected);
	}
	if (g_ZrSoundsCookie == INVALID_HANDLE)
	{
		g_ZrSoundsCookie = RegClientCookie("zr_sounds", "Select Vol ZrSounds.", CookieAccess_Protected);
	}
	if (g_ZombiesECookie == INVALID_HANDLE)
	{
		g_ZombiesECookie = RegClientCookie("zr_zombies_sounds_enable", "Zombies sounds enable.", CookieAccess_Protected);
	}
	if (g_ZRoundEndCookie == INVALID_HANDLE)
	{
		g_ZRoundEndCookie = RegClientCookie("zr_roundend_sounds_enable", "RoundEnd sounds enable.", CookieAccess_Protected);
	}
}

void ZrSoundsOnCookiesCached(int client)
{
	// Get VOICECOUNT enabled cookie value.
	char voiceselect[8];
	char zombisounde[8];
	char zroundende[8];
	char volumevalue[11];
	GetClientCookie(client, g_CountDownCookie, voiceselect, sizeof(voiceselect));
	GetClientCookie(client, g_ZombiesECookie, zombisounde, sizeof(zombisounde));
	GetClientCookie(client, g_ZRoundEndCookie, zroundende, sizeof(zroundende));
	GetClientCookie(client, g_ZrSoundsCookie, volumevalue, sizeof(volumevalue));

	// If the cookie is empty, then set the default value.
	if (!voiceselect[0])
	{
		// Set cookie to default value from cvar.
		CookiesSetInt(client, g_CountDownCookie, g_hCvarsList.g_iCVAR_VOICECOUNT_DEFAULT);
	}
	if(!zombisounde[0])
		{
		// Set cookie to default value from cvar.
		CookiesSetClientCookieBool(client, g_ZombiesECookie, g_hCvarsList.g_bCVAR_ZOMBIES_SOUNDS_DEFAULT);
	}
	if(!zroundende[0])
		{
		// Set cookie to default value from cvar.
		CookiesSetClientCookieBool(client, g_ZRoundEndCookie, g_hCvarsList.g_bCVAR_ROUNDEND_SOUNDS_DEFAULT);
	}
	if(!volumevalue[0])
	{
		// Set cookie to default value from cvar.
		CookiesSetFloat(client, g_ZrSoundsCookie, g_hCvarsList.g_fCVAR_VOLUME_DEFAULT);
	}
}

void ZSoundsMenuMain(int client)
{
	float Vol = CookiesGetFloat(client, g_ZrSoundsCookie);
	g_fClientVol[client] = Vol;
	
	// Create menu handle.
	Handle zsounds_menu_main = CreateMenu(ZrSoundsMenuHandler);
	SendConVarValue(client, g_hMp_Maxmoney, "0");
	SendConVarValue(client, g_hSv_Disable_Radar, "1");
	
	// Make client global translations target.
	SetGlobalTransTarget(client);
	
	char title[MENU_LINE_TITLE_LENGTH];
	char zombisounde[MENU_LINE_SMALL_LENGTH];
	char zroundende[MENU_LINE_SMALL_LENGTH];
	char soundzombi[MENU_LINE_REG_LENGTH];
	char soundrend[MENU_LINE_REG_LENGTH];
	char coundsound[MENU_LINE_REG_LENGTH];
	char zrvolume[MENU_LINE_HUGE_LENGTH];
	
	// Get the current toggle state of the cookies.
	ConfigBoolToSetting(CookiesGetClientCookieBool(client, g_ZombiesECookie), zombisounde, sizeof(zombisounde), false, client);
	ConfigBoolToSetting(CookiesGetClientCookieBool(client, g_ZRoundEndCookie), zroundende, sizeof(zroundende), false, client);
	
	// Translate each line into client's language.
	Format(title, sizeof(title), "%t\n ", "zr sounds");
	Format(soundzombi, sizeof(soundzombi), "%t", "Zombies_Sounds", zombisounde);
	Format(soundrend, sizeof(soundrend), "%t", "RoundEnd_Sounds", zroundende);
	Format(zrvolume, sizeof(zrvolume), "%t %i%", "zr_volume", RoundFloat(g_fClientVol[client] * 100));
	
	// Get conditional values for each option.
	int cookievalue = CookiesGetInt(client, g_CountDownCookie);
	
	//Menu Variavel
	if(cookievalue == 1)
	{
		Format(coundsound, sizeof(coundsound), "%t %t", "Select Countdown", "voz_1"); //mem
	}
	if(cookievalue == 2)
	{
		Format(coundsound, sizeof(coundsound), "%t %t", "Select Countdown", "voz_2"); //woman
	}
	if(cookievalue == 0)
	{
		Format(coundsound, sizeof(coundsound), "%t %t", "Select Countdown", "voz_3"); //disabled
	}
	
	// Add items to menu.
	SetMenuTitle(zsounds_menu_main, title);
	AddMenuItem(zsounds_menu_main, "soundzombi", soundzombi, MenuGetItemDraw(g_hCvarsList.g_bCVAR_ZOMBIES_SOUNDS));
	AddMenuItem(zsounds_menu_main, "soundrend", soundrend, MenuGetItemDraw(g_hCvarsList.g_bCVAR_ROUNDEND_SOUNDS));
	AddMenuItem(zsounds_menu_main, "zrvolume", zrvolume);
	AddMenuItem(zsounds_menu_main, "coundsound", coundsound, MenuGetItemDraw(g_hCvarsList.g_bCVAR_COUNTDOWN_SOUNDS));

	// Create a "Back" button to the main menu.
	SetMenuExitBackButton(zsounds_menu_main, true);
	
	// Display menu to client.
	DisplayMenu(zsounds_menu_main, client, MENU_TIME_FOREVER);	
}

public int ZrSoundsMenuHandler(Handle menu, MenuAction action, int client, int slot)
{
	// Client selected an option.
	if (action == MenuAction_Select)
	{
		bool resend = true;
		
		switch(slot)
		{
			// Toggled 
			case 0:
			{
				// Toggle 
				ZSEToggle(client);
			}
			// Toggled
			case 1:
			{
				ZREToggle(client);
			}
			// Toggled
			case 2:
			{
				ZSoundsVolumeMenuMain(client);
				resend = false;
			}
			// Opened
			case 3:
			{
				CounToggle(client);
				//resend = false;
			}
		}
		
		if (resend)
		{
			// Re-send menu.
			ZSoundsMenuMain(client);
		}
	}
	if (action == MenuAction_Cancel && slot != MenuCancel_ExitBack && IsValidClient(client))
	{
		SendConVarValue(client, g_hMp_Maxmoney, g_sValue_Mp_Maxmoney);
		SendConVarValue(client, g_hSv_Disable_Radar, g_sValue_Sv_Disable_Radar);
	}
	// Client closed the menu.
	if (action == MenuAction_Cancel)
	{
		// Client hit "Back" button.
		if (slot == MenuCancel_ExitBack)
		{
			// Send Zmenu menu.
			FakeClientCommand(client, "say !zmenu");
		}
	}
	// Client exited menu.
	if (action == MenuAction_End)
	{
		CloseHandle(menu);
	}
}

void ZSoundsVolumeMenuMain(int client)
{
	// Get Client Vol.
	float Vol = CookiesGetFloat(client, g_ZrSoundsCookie);
	g_fClientVol[client] = Vol;
	
	// Create menu handle.
	Handle Volume_Menu_Main = CreateMenu(VolumeMenuHandler);
	SendConVarValue(client, g_hMp_Maxmoney, "0");
	SendConVarValue(client, g_hSv_Disable_Radar, "1");
	
	// Make client global translations target.
	SetGlobalTransTarget(client);
	
	char volumemenutitle[MENU_LINE_TITLE_LENGTH];
	char v100[MENU_LINE_SMALL_LENGTH];
	char v75[MENU_LINE_SMALL_LENGTH];
	char v50[MENU_LINE_SMALL_LENGTH];
	char v25[MENU_LINE_SMALL_LENGTH];
	char v10[MENU_LINE_SMALL_LENGTH];
	char v05[MENU_LINE_SMALL_LENGTH];
	
		
	// Translate each line into client's language.
	Format(volumemenutitle, sizeof(volumemenutitle), "%t\n%t\n ", "Volume Menu Title", "Volume Current", RoundFloat(g_fClientVol[client] * 100));
	Format(v100, sizeof(v100), "%t", "v100");
	Format(v75, sizeof(v75), "%t", "v75");
	Format(v50, sizeof(v50), "%t", "v50");
	Format(v25, sizeof(v25), "%t", "v25");
	Format(v10, sizeof(v10), "%t", "v10");
	Format(v05, sizeof(v05), "%t", "v05");
	
	// Add items to menu.
	SetMenuTitle(Volume_Menu_Main, volumemenutitle);
	AddMenuItem(Volume_Menu_Main, "100", v100);
	AddMenuItem(Volume_Menu_Main, "75", v75);
	AddMenuItem(Volume_Menu_Main, "50", v50);
	AddMenuItem(Volume_Menu_Main, "25", v25);
	AddMenuItem(Volume_Menu_Main, "10", v10);
	AddMenuItem(Volume_Menu_Main, "5", v05);

	// Create a "Back" button to the main menu.
	SetMenuExitBackButton(Volume_Menu_Main, true);

	// Display menu to client.
	DisplayMenu(Volume_Menu_Main, client, MENU_TIME_FOREVER);
}

public int VolumeMenuHandler(Handle menu, MenuAction action, int client, int slot)
{
	if (action == MenuAction_Select)
	{
		bool resend = true;
		switch(slot)
		{
			case 0:
			{
				g_fClientVol[client] = 1.0;
				CookiesSetFloat(client, g_ZrSoundsCookie, g_fClientVol[client]);
				TranslationPrintCenterText(client, "Volume set to", RoundFloat(g_fClientVol[client] * 100));
				TranslationPrintToChat(client, "Volume updated");
			} 
			case 1:
			{
				g_fClientVol[client] = 0.75;
				CookiesSetFloat(client, g_ZrSoundsCookie, g_fClientVol[client]);
				TranslationPrintCenterText(client, "Volume set to", RoundFloat(g_fClientVol[client] * 100));
				TranslationPrintToChat(client, "Volume updated");
			}
			case 2:
			{
				g_fClientVol[client] = 0.5;
				CookiesSetFloat(client, g_ZrSoundsCookie, g_fClientVol[client]);
				TranslationPrintCenterText(client, "Volume set to", RoundFloat(g_fClientVol[client] * 100));
				TranslationPrintToChat(client, "Volume updated");
			}
			case 3:
			{
				g_fClientVol[client] = 0.25;
				CookiesSetFloat(client, g_ZrSoundsCookie, g_fClientVol[client]);
				TranslationPrintCenterText(client, "Volume set to", RoundFloat(g_fClientVol[client] * 100));
				TranslationPrintToChat(client, "Volume updated");
			}
			case 4:
			{
				g_fClientVol[client] = 0.1;
				CookiesSetFloat(client, g_ZrSoundsCookie, g_fClientVol[client]);
				TranslationPrintCenterText(client, "Volume set to", RoundFloat(g_fClientVol[client] * 100));
				TranslationPrintToChat(client, "Volume updated");
			}
			case 5:
			{
				g_fClientVol[client] = 0.05;
				CookiesSetFloat(client, g_ZrSoundsCookie, g_fClientVol[client]);
				TranslationPrintCenterText(client, "Volume set to", RoundFloat(g_fClientVol[client] * 100));
				TranslationPrintToChat(client, "Volume updated");
			}
		}
		if (resend)
		{
			// Re-send menu.
			ZSoundsVolumeMenuMain(client);
		}
	}
	// Client closed the menu.
	if (action == MenuAction_Cancel && slot != MenuCancel_ExitBack && IsValidClient(client))
	{
		SendConVarValue(client, g_hMp_Maxmoney, g_sValue_Mp_Maxmoney);
		SendConVarValue(client, g_hSv_Disable_Radar, g_sValue_Sv_Disable_Radar);
	}
	// Client closed the menu.
	if (action == MenuAction_Cancel)
	{
		// Client hit "Back" button.
		if (slot == MenuCancel_ExitBack)
		{
			// Re-open main menu.
			ZSoundsMenuMain(client);
		}
	}
	// Client exited menu.
	if (action == MenuAction_End)
	{
		CloseHandle(menu);
	}
}

//Toggle CountDown Voices.
int CounToggle(int client)
{
	if (!g_hCvarsList.g_bCVAR_COUNTDOWN_SOUNDS)
	{
		//Tell client feature is disabled.
		TranslationPrintToChat(client, "Feature is disabled");

		return false;
	}

	// Get the cookie value.
	int Cdstate = CookiesGetInt(client, g_CountDownCookie);

	if (Cdstate == 1)
	{
	// Toggle the value.
		TranslationPrintCenterText(client, "CountDown select 2");
		TranslationPrintToChat(client, "CountDown select 2");
		CookiesSetInt(client, g_CountDownCookie, 2);
		return true;
	}	
	if (Cdstate == 2)
	{
	// Toggle the value.
		TranslationPrintCenterText(client, "CountDown select 0");
		TranslationPrintToChat(client, "CountDown select 0");
		CookiesSetInt(client, g_CountDownCookie, 0);
		return true;
	}	
	else
	{
	// Toggle the value.
		TranslationPrintCenterText(client, "CountDown select 1");
		TranslationPrintToChat(client, "CountDown select 1");
		CookiesSetInt(client, g_CountDownCookie, 1);
	}
	
	return true;
}

//Toggle Zombis sounds.
bool ZSEToggle(int client)
{
	if (!g_hCvarsList.g_bCVAR_ZOMBIES_SOUNDS)
	{
		//Tell client feature is disabled.
		TranslationPrintToChat(client, "Feature is disabled");

		return false;
	}
	
	// Get the cookie value.
	bool zsestate = CookiesGetClientCookieBool(client, g_ZombiesECookie);
	
	// Toggle the value.
	CookiesSetClientCookieBool(client, g_ZombiesECookie, !zsestate);
	
	// If ZHP was enabled, then tell client it has been disabled.
	if (zsestate)
	{
		TranslationPrintToChat(client, "Zombis sounds disable");
		TranslationPrintCenterText(client, "Zombis sounds disable");
	}
	// If ZHP was disabled, then tell client it has been enabled.
	else
	{
		TranslationPrintToChat(client, "Zombis sounds enable");
		TranslationPrintCenterText(client, "Zombis sounds enable");
	}
	
	return true;
}

//Toggle RoundEnd sounds.
bool ZREToggle(int client)
{
	if (!g_hCvarsList.g_bCVAR_ROUNDEND_SOUNDS)
	{
		// Tell client feature is disabled.
		TranslationPrintToChat(client, "Feature is disabled");
		
		return false;
	}
	
	// Get the cookie value.
	bool zrestate = CookiesGetClientCookieBool(client, g_ZRoundEndCookie);
	
	// Toggle the value.
	CookiesSetClientCookieBool(client, g_ZRoundEndCookie, !zrestate);
	
	// If RoundEnd sounds was enabled, then tell client it has been disabled.
	if (zrestate)
	{
		TranslationPrintToChat(client, "RoundEnd sounds disable");
		TranslationPrintCenterText(client, "RoundEnd sounds disable");
	}
	// If RoundEnd sounds was disabled, then tell client it has been enabled.
	else
	{
		TranslationPrintToChat(client, "RoundEnd sounds enable");
		TranslationPrintCenterText(client, "RoundEnd sounds enable");
	}
	
	return true;
}
