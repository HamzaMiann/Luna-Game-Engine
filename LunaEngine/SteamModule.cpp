#include "SteamModule.h"

#define _CRT_SECURE_NO_WARNINGS

#include <steam/steam_api.h>

//#define USE_STEAM

namespace steam_api
{
	bool isInitialized = false;
}

void ShowFriends()
{
	int nFriends = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	if (nFriends == -1)
	{
		printf("GetFriendCount returned -1, the current user is not logged in.\n");
		// We always recommend resetting to 0 just in case you were to do something like allocate
		// an array with this value, or loop over it in a way that doesn't take into the -1 into account.
		nFriends = 0;
	}

	for (int i = 0; i < nFriends && i < 10; ++i)
	{
		CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
		const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
		printf("Friend %d: %lld - %s\n", i, friendSteamID.ConvertToUint64(), friendName);
	}
}

bool InitSteam()
{
	printf("Initializing Steam...\n");
#ifdef USE_STEAM
	if (!SteamAPI_Init())
	{
		printf("Could not initialize Steam...\n");
		steam_api::isInitialized = false;
	}
	else
	{
		printf("Steam has been initialized...\n");
		steam_api::isInitialized = true;

		ShowFriends();
	}
#endif
	return steam_api::isInitialized;
}

void ReleaseSteam()
{
#ifdef USE_STEAM
	if (steam_api::isInitialized)
	{
		SteamAPI_Shutdown();
	}
#endif
}
