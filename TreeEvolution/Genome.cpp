#include "Genome.h"

vector <Genome> Genome::defaultGenomes = vector <Genome>();

void Genome::init(mt19937& rng)
{
	if (useDefauldGenomes && defaultGenomes.size() > 0)
	{
		int p = rng() % defaultGenomes.size();
		//int p = 5 * 81;

		*this = defaultGenomes[p];
	}
	else
	{
		for (int i = 0; i < 16; i++)
		{
			grow[i].init(rng);
		}
		for (int i = 0; i < 8; i++)
		{
			cellType[i].init(rng);
		}
	}
}

void Genome::mutate(mt19937& rng)
{
	int p = rng() % 24;

	if (p < 16)
	{
		grow[p].mutate(rng);
	}
	else
	{
		p -= 16;
		cellType[p].mutate(rng);
	}
}