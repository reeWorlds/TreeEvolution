#pragma once

#include "includes.h"

#define CTG_minHumidity 0
#define CTG_maxHumidity 5

#define CTG_minCompostFactor 0
#define CTG_maxCompostFactor 10


class CellTypeGenome
{

public:

	int humidity;
	int compostFactor;

	void init(mt19937& rng);

	void mutate(mt19937& rng);
};