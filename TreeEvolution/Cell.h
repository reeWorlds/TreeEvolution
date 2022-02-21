#pragma once

#include "includes.h"

#define cellSeedColorIndex 1000000001

class Cell
{

public:

	int x, y;
	int growP, typeP;
	int isSeed;

	Cell(int _x, int _y);
};