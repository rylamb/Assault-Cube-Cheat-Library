#pragma once
#include <cmath>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

struct Vector3
{
	float x, y, z;

	// Default constructor
	Vector3()
	{
		x = y = z = 0;
	}

	// Constructor with 3 arguments passed in X Y Z format
	Vector3(float xCoord, float yCoord, float zCoord)
	{
		x = xCoord;
		y = yCoord;
		z = zCoord;
	}

	float vectorMagnitude();
};

class playerent
{
public:
	char pad_0000[12]; //0x0000
	float headPos; //0x000C
	char pad_0010[36]; //0x0010
	Vector3 bodyPos; //0x0034
	Vector3 yawPitchRoll; //0x0040
	char pad_004C[172]; //0x004C
	int32_t playerHealth; //0x00F8
	int32_t playerArmor; //0x00FC
	char pad_0100[556]; //0x0100
	int32_t team; //0x032C
	char pad_0330[1844]; //0x0330
}; //Size: 0x0


vector<playerent*> fillEntityArray(uintptr_t* local_player_addr, vector<playerent*> playerList);