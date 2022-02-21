#include "CellTypeGenome.h"

void CellTypeGenome::init(mt19937& rng)
{
	humidity = CTG_minHumidity + rng() % (CTG_maxHumidity - CTG_minHumidity + 1);
	compostFactor = CTG_minCompostFactor + rng() % (CTG_maxCompostFactor - CTG_minCompostFactor + 1);
}

void CellTypeGenome::mutate(mt19937& rng)
{
	if (rng() % 2)
	{
		if (rng() % 2)
		{
			humidity--;
			if (humidity < CTG_minHumidity) { humidity++; }
		}
		else
		{
			humidity++;
			if (humidity > CTG_maxHumidity) { humidity--; }
		}
	}
	else
	{
		if (rng() % 2)
		{
			compostFactor--;
			if (compostFactor < CTG_minCompostFactor) { compostFactor++; }
		}
		else
		{
			compostFactor++;
			if (compostFactor > CTG_maxCompostFactor) { compostFactor--; }
		}
	}
}