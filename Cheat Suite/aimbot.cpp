#include "pch.h"
#include "aimbot.h"

float Vector3::vectorMagnitude()
{
	return sqrt(x * x + y * y + z * z);
}

vector<playerent*> fillEntityArray(uintptr_t* local_player_addr, vector<playerent*> playerList)
{
	uintptr_t* numPlayersAddr = (uintptr_t*)0x50F500;					// Location of # of players in game
	uintptr_t entityListPtr = (uintptr_t)local_player_addr + 0x4;		// 10F4F8, location of where bots are stored
	uintptr_t startEntityList = *(uintptr_t*)entityListPtr + 0x4;		// Index 0 is empty, first bot stored at index 1
	int numPlayers = *numPlayersAddr;

	// For testing, can comment out latner
	cout << "numPlayersAddr = " << std::hex << numPlayersAddr << endl;
	cout << "entityListPtr = " << std::hex << entityListPtr << endl;
	cout << "startEntityList = " << std::hex << startEntityList << endl;
	cout << "numPlayers = " << std::dec << numPlayers << endl;

	for (int i = 0; i < numPlayers - 1; i++)
	{
		// Increment memory address, storing dynamically allocated bot memory locations into playerList
		uintptr_t currentEntityList = startEntityList + 4*i;
		uintptr_t entityMemoryLocation = *(uintptr_t*)currentEntityList;
		playerList.push_back((playerent*)entityMemoryLocation);

		// For testing
		cout << "currentEntityList = " << std::hex << currentEntityList << endl;
		cout << "entityMemoryLocation = " << std::hex << entityMemoryLocation << endl << endl;
	}

	return playerList;
}