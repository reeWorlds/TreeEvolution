#pragma once

#include "includes.h"
#include "Genome.h"
#include "Cell.h"

#define treeMinAge 70
#define treeMaxAge 90

class Tree
{

public:

	int age, maxAge;
	int energy;

	Genome g;
	vector <Cell> cells;

	sf::Color color;

	void init(mt19937& rng);
	void reviseForSeed(int p);

	sf::Color newColor(mt19937& rng);
};