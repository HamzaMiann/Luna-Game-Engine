#include "SteamModule.h"

#define _CRT_SECURE_NO_WARNINGS

#include <steam/steam_api.h>

namespace steam_api
{
	bool isInitialized = false;
}

bool InitSteam()
{
	printf("Initializing Steam...\n");
	if (!SteamAPI_Init())
	{
		printf("Could not initialize Steam...\n");
		steam_api::isInitialized = false;
	}
	else
	{
		printf("Steam has been initialized...\n");
		steam_api::isInitialized = true;
	}
	return steam_api::isInitialized;
}

void ReleaseSteam()
{
	if (steam_api::isInitialized)
	{
		SteamAPI_Shutdown();
	}
}
