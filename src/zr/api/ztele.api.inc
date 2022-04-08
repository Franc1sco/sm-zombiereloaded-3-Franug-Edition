
/**
 * @section Global forward handles.
 */
Handle g_hAPIFwdOnClientTeleport = INVALID_HANDLE;
Handle g_hAPIFwdOnClientTeleported = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to respawn.
 */
void APITeleportInit()
{
	// Forwards
	g_hAPIFwdOnClientTeleport = CreateGlobalForward("ZR_OnClientTeleport", ET_Ignore, Param_Cell);
	g_hAPIFwdOnClientTeleported = CreateGlobalForward("ZR_OnClientTeleported", ET_Ignore, Param_Cell);
}

/**
 * Called right before ZR is about to teleport a player.
 * 
 * @param client			The client index.
 */
void APIOnClientTeleport(int client)
{
	// Start forward call.
	Call_StartForward(g_hAPIFwdOnClientTeleport);

	// Push the parameters.
	Call_PushCell(client);

	// Finish the call.
	Call_Finish();
}

/**
 * Called after a player was teleported.
 * 
 * @param client			The client index.
 */
void APIOnClientTeleported(int client)
{
	// Start forward call.
	Call_StartForward(g_hAPIFwdOnClientTeleported);
	
	// Push the parameters.
	Call_PushCell(client);
	
	// Finish the call.
	Call_Finish();
}
