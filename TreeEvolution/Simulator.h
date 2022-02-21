#pragma once

#include "includes.h"
#include "Tree.h"

#define fWidth 8192
//#define fWidth 1024
#define fHeight 96

#define fWidthMask (fWidth - 1)
#define fHeightMask (fHeight - 1)

#define fShift 15
#define fMask ((1 << 15) - 1)

#define seasonShiftByStep 1

#define initialTreesN 500

#define minTreeSizeToLive 10
#define minTreeSizeToLive_Time 35

#define minSeedEnergy 10
#define growEnergyCost 10
#define maxSeedEnergy 200

class Simulator
{
	void nullifyF();

	void updateTreesEnergy();

	int seasonInnerCounter;

public:

	int f[fHeight][fWidth];
	int lt[fHeight][fWidth];

	mt19937 rng;

	vector <Tree> trees;
	vector <tuple<Cell, Genome, int, sf::Color> > freeSeeds;

	int seasonP;
	int globalIteration;

	int fileIteration;

	int respawnIteration;

	Simulator();

	void processOneStep();

};