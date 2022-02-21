#include "GrowGenome.h"

void GrowGenome::init(mt19937& rng)
{
	for (int i = 0; i < 4; i++)
	{
		nxt[i][0] = GG_minNextGG + rng() % (GG_maxNextGG - GG_minNextGG + 1);
		nxt[i][1] = GG_minNextCTG + rng() % (GG_maxNextCTG - GG_minNextCTG + 1);
	}
}

void GrowGenome::mutate(mt19937& rng)
{
	int p = rng() % 4;

	if (rng() % 2)
	{
		nxt[p][0] = GG_minNextGG + rng() % (GG_maxNextGG - GG_minNextGG + 1);
	}
	else
	{
		nxt[p][1] = GG_minNextCTG + rng() % (GG_maxNextCTG - GG_minNextCTG + 1);
	}
}