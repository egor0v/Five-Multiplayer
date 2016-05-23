#include "stdafx.h"

//Function to determine if a Player object is on your Rockstar Social Club friends list.
BOOL IsPlayerFriend(Player player)
{
	int handle[76]; //var num3 = sub_34009(A_0, (A_1) + 264, (A_1) + 272);
	NETWORK::NETWORK_HANDLE_FROM_PLAYER(player, &handle[0], 13);
	if (NETWORK::NETWORK_IS_HANDLE_VALID(&handle[0], 13))
	{
		return NETWORK::NETWORK_IS_FRIEND(&handle[0]);
	}
	return FALSE;
}

int GetNetworkHandle(Player player)
{
	int handle[76]; //var num3 = sub_34009(A_0, (A_1) + 264, (A_1) + 272);
	NETWORK::NETWORK_HANDLE_FROM_PLAYER(player, &handle[0], 13);
	if (NETWORK::NETWORK_IS_HANDLE_VALID(&handle[0], 13))
	{
		return handle[0];
	}
	return NULL;
}