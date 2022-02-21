#include "Tree.h"

void Tree::init(mt19937& rng)
{
	age = 0;
	maxAge = treeMinAge + rng() % (treeMaxAge - treeMinAge + 1);

	energy = 0; // __mbch

	g.init(rng);

	color = newColor(rng);
}

sf::Color Tree::newColor(mt19937& rng)
{
	sf::Color col = sf::Color(50 + rng() % 75, 100 + rng() % 25, 150 + rng() % 75);
	if (rng() % 2) { swap(col.g, col.r); }
	if (rng() % 2) { swap(col.b, col.r); }
	else { swap(col.b, col.g); }

	return col;
}

void Tree::reviseForSeed(int p) // __mbch;
{
	if (cells[p].isSeed == 0)
	{
		return;
	}

	int ggi = cells[p].growP;

	if (g.grow[ggi].nxt[0][0] >= 16 && g.grow[ggi].nxt[1][0] >= 16
		&& g.grow[ggi].nxt[2][0] >= 16 && g.grow[ggi].nxt[3][0] >= 16)
	{
		cells[p].isSeed = 0;
	}

	if (cells[p].y == 0 && g.grow[ggi].nxt[0][0] >= 16 &&
		g.grow[ggi].nxt[1][0] >= 16 && g.grow[ggi].nxt[3][0] >= 16)
	{
		cells[p].isSeed = 0;
	}
}