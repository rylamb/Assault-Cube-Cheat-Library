#include "pch.h"
#include "aimbot.h"

float Vector3::vectorMagnitude()
{
	return sqrt(x * x + y * y + z * z);
}

vector<playerent*> fillEntityArray(uintptr_t* local_player_addr, vector<playerent*> playerList)
{
	uintptr_t* numPlayersAddr = (uintptr_t*)0x50F500;
	uintptr_t entityListPtr = (uintptr_t)local_player_addr + 0x4;
	uintptr_t startEntityList = *(uintptr_t*)entityListPtr + 0x4;
	int numPlayers = *numPlayersAddr;

	cout << "numPlayersAddr = " << std::hex << numPlayersAddr << endl;
	cout << "entityListPtr = " << std::hex << entityListPtr << endl;
	cout << "startEntityList = " << std::hex << startEntityList << endl;
	cout << "numPlayers = " << std::dec << numPlayers << endl;

	for (int i = 0; i < numPlayers - 1; i++)
	{
		uintptr_t currentEntity = startEntityList + 4*i;
		cout << "currentEntity = " << std::hex << currentEntity << endl;
		playerList.push_back((playerent*)currentEntity);
	}


	return playerList;
}