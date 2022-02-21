#pragma once

#include "includes.h"
#include "GrowGenome.h"
#include "CellTypeGenome.h"

#define useDefauldGenomes 0
#define defaultGenomesCount 83

class Genome
{

public:

	static vector <Genome> defaultGenomes;

	GrowGenome grow[16];
	CellTypeGenome cellType[8];

	void init(mt19937& rng);

	void mutate(mt19937& rng);
};