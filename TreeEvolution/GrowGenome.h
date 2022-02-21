#pragma once

#include "includes.h"

#define GG_minNextGG 0
#define GG_maxNextGG 31

#define GG_minNextCTG 0
#define GG_maxNextCTG 7

class GrowGenome
{

public:

	int nxt[4][2];

	void init(mt19937& rng);

	void mutate(mt19937& rng);
};