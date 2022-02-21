#include "Simulator.h"

Simulator::Simulator()
{
	rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());
	//rng.seed(4);

	nullifyF();

	globalIteration = 0;

	seasonP = 0;
	seasonInnerCounter = 0;

	respawnIteration = 0;

	fileIteration = 0;

	if (useDefauldGenomes)
	{
		for (int iter = 0; iter <= defaultGenomesCount; iter++)
		{
			ifstream in("../Genomes/g" + to_string(iter) + ".txt");

			int itN;
			in >> itN;

			for (int it = 0; it < itN; it++)
			{
				Genome g;

				for (int i = 0; i < 16; i++)
				{
					for (int j = 0; j < 8; j++)
					{
						in >> g.grow[i].nxt[j / 2][j % 2];
					}
				}
				for (int i = 0; i < 8; i++)
				{
					in >> g.cellType[i].compostFactor >> g.cellType[i].humidity;
				}

				Genome::defaultGenomes.push_back(g);
			}
		}
	}
}

void Simulator::nullifyF()
{
	for (int i = 0; i < fHeight; i++)
	{
		for (int j = 0; j < fWidth; j++)
		{
			f[i][j] = -1;
		}
	}
}

void Simulator::updateTreesEnergy()
{
	const int sunMaxVal[4] = { 2, 5, 7, 4 };

	// update sun energy
#pragma omp parallel for num_threads(NUM_THREADS)
	for (int i = 0; i < fWidth; i++)
	{
		int seasonIndex = (i + fWidth - seasonP) % fWidth / (fWidth / 4);

		int val = sunMaxVal[seasonIndex] * 2;

		for (int j = fHeight - 1; j >= 4; j--)
		{
			val = f[j][i] == -1 ? val + 2 : val - 2;
			val = min(val, sunMaxVal[seasonIndex] * 2);
			lt[j][i] = val / 2 + j / 15;
		}
		for (int j = 3; j >= 0; j--)
		{
			lt[j][i] = 0;
		}
	}

#pragma omp parallel for num_threads(NUM_THREADS)
	for (int i = 0; i < trees.size(); i++)
	{
		Tree* tree = &trees[i];

		int sunEnergy = 0;

		for (auto& cell : tree->cells)
		{
			sunEnergy += lt[cell.y][cell.x];
		}

		tree->energy += sunEnergy;
	}

	// update compost
	for (auto& tree : trees)
	{
		int compostDecr = tree.g.cellType[tree.cells[0].typeP].compostFactor * 10;

		for (auto& cell : tree.cells)
		{
			compostDecr -= tree.g.cellType[cell.typeP].compostFactor;
		}

		tree.energy += compostDecr;
	}

	// update hummidity
	const int minHumiditySeason[4] = { 0, 1, 2, 0 };

	for (auto& tree : trees)
	{
		int seasonIndex = (tree.cells[0].x + fWidth - seasonP) % fWidth / (fWidth / 4);
		int minHum = minHumiditySeason[seasonIndex];

		int sumHumidityTr = 0, sumHumidity = 0;

		for (auto& cell : tree.cells)
		{
			sumHumidity += tree.g.cellType[cell.typeP].humidity;
			sumHumidityTr += tree.g.cellType[cell.typeP].humidity - minHum;
		}

		tree.energy += min(sumHumidityTr - sumHumidity / 2, (int)tree.cells.size() * 2);
	}

	// season update
	for (auto& tree : trees)
	{
		int seasonIndex = (tree.cells[0].x + fWidth - seasonP) % fWidth / (fWidth / 4);
		
		if (seasonIndex == 0)
		{
			// to do nothing
		}
		else if (seasonIndex == 1)
		{
			tree.energy -= tree.cells.size();
		}
		else if (seasonIndex == 2)
		{
			tree.energy -= tree.cells.size() * 3 / 2;
		}
		else if (seasonIndex == 3)
		{
			tree.energy -= tree.cells.size();
		}
	}
}

void Simulator::processOneStep()
{
	globalIteration++;

	if (useDefauldGenomes == 0 && globalIteration % (750 * 80) == 0 && false)
	{
		if (trees.size() >= 20)
		{
			fileIteration++;

			ofstream out("../Genomes/g" + to_string(defaultGenomesCount + fileIteration) + ".txt");

			out << 5 << "\n";
			for (int t = 0; t < 5; t++)
			{
				int p = rng() % trees.size();

				Genome g = trees[p].g;

				for (int i = 0; i < 16; i++)
				{
					for (int j = 0; j < 8; j++)
					{
						out << g.grow[i].nxt[j / 2][j % 2] << " ";
					}
					out << "\n";
				}
				for (int i = 0; i < 8; i++)
				{
					out << g.cellType[i].compostFactor << " " << g.cellType[i].humidity << "\n";
				}
			}

			cout << "Finish file iteration " << fileIteration << "\n";

			trees.clear();
			freeSeeds.clear();
			nullifyF();
		}
	}

	if (trees.size() < initialTreesN / 20 || trees.size() == 0)
	{
		respawnIteration++;

		for (int iter = 0; iter < initialTreesN; iter++)
		{
			int p = rng() & fWidthMask;

			while (f[0][p] != -1) { p = rng() & fWidthMask; }

			trees.push_back(Tree());
			trees.back().init(rng);
			trees.back().energy = maxSeedEnergy;
			trees.back().cells.push_back(Cell(p, 0));
			trees.back().reviseForSeed(0);
			f[0][p] = iter + (0 << fShift);
		}
	}

	updateTreesEnergy();

	vector <Tree> newTrees;
	vector <tuple<Cell, Genome, int, sf::Color> > newFreeSeeds;

	for (auto& [cell, g, energy, color] : freeSeeds)
	{
		if (cell.y == 0)
		{
			newTrees.push_back(Tree());
			newTrees.back().age = 0;
			newTrees.back().maxAge = treeMinAge + rng() % (treeMaxAge - treeMinAge + 1);
			newTrees.back().energy = energy;
			newTrees.back().g = g;
			newTrees.back().cells.push_back(cell);
			newTrees.back().reviseForSeed(0);
			newTrees.back().color = color;
			f[cell.y][cell.x] = newTrees.size() - 1;
		}
		else
		{
			f[cell.y][cell.x] = -1;
			if (f[cell.y - 1][cell.x] == -1)
			{
				cell.y--;
				f[cell.y][cell.x] = cellSeedColorIndex;

				newFreeSeeds.push_back(tuple<Cell, Genome, int, sf::Color>(cell, g, energy, color));
			}
		}
	}
	for (auto& tree : trees)
	{
		if (tree.energy <= 0)
		{
			for (auto& cell : tree.cells)
			{
				f[cell.y][cell.x] = -1;
			}

			continue;
		}

		tree.age++;
		if (tree.age >= tree.maxAge)
		{
			int seedCells = 0;
			for (auto& cell : tree.cells)
			{
				if (cell.isSeed == 1)
				{
					seedCells++;
				}
			}

			for (auto& cell : tree.cells)
			{
				if (cell.isSeed == 1)
				{
					f[cell.y][cell.x] = cellSeedColorIndex;
					
					Genome newG = tree.g;
					sf::Color color = tree.color;
					int r = rng() % 100;
					if (r < 50)
					{
						for (int i = 0; i < r / 10; i++)
						{
							newG.mutate(rng);
							color = tree.newColor(rng);
						}
					}

					int energy = min(max(minSeedEnergy, tree.energy / seedCells + minSeedEnergy),
						maxSeedEnergy);

					newFreeSeeds.push_back(
						tuple<Cell, Genome, int, sf::Color>(cell, tree.g, energy, color));
				}
				else
				{
					f[cell.y][cell.x] = -1;
				}
			}
		}
		else
		{
			if (tree.age == minTreeSizeToLive_Time)
			{
				if (tree.cells.size() < minTreeSizeToLive && rng() % 100 < 50)
				{
					for (auto& cell : tree.cells)
					{
						f[cell.y][cell.x] = -1;
					}

					continue;
				}
			}

			vector <Cell> newCells;

			for (auto& cell : tree.cells)
			{
				if (cell.isSeed == 1)
				{
					int growed = 0;

					if (cell.y > 0 &&
						tree.g.grow[cell.growP].nxt[2][0] < 16 &&
						f[cell.y - 1][cell.x] == -1 &&
						tree.energy > growEnergyCost)
					{
						f[cell.y - 1][cell.x] = 0;
						newCells.push_back(Cell(cell.x, cell.y - 1));
						newCells.back().growP = tree.g.grow[cell.growP].nxt[2][0];
						newCells.back().typeP = tree.g.grow[cell.growP].nxt[2][1];
						tree.energy -= growEnergyCost;
						growed = 1;
					}
					if (cell.y < fHeight - 1 &&
						tree.g.grow[cell.growP].nxt[0][0] < 16 &&
						f[cell.y + 1][cell.x] == -1 &&
						tree.energy > growEnergyCost)
					{
						f[cell.y + 1][cell.x] = 0;
						newCells.push_back(Cell(cell.x, cell.y + 1));
						newCells.back().growP = tree.g.grow[cell.growP].nxt[0][0];
						newCells.back().typeP = tree.g.grow[cell.growP].nxt[0][1];
						tree.energy -= growEnergyCost;
						growed = 1;
					}
					if (f[cell.y][(cell.x - 1) & fWidthMask] == -1 &&
						tree.g.grow[cell.growP].nxt[3][0] < 16 &&
						tree.energy > growEnergyCost && cell.y > -1)
					{
						f[cell.y][(cell.x - 1) & fWidthMask] = 0;
						newCells.push_back(Cell((cell.x - 1) & fWidthMask, cell.y));
						newCells.back().growP = tree.g.grow[cell.growP].nxt[3][0];
						newCells.back().typeP = tree.g.grow[cell.growP].nxt[3][1];
						tree.energy -= growEnergyCost;
						growed = 1;
					}
					if (f[cell.y][(cell.x + 1) & fWidthMask] == -1 &&
						tree.g.grow[cell.growP].nxt[1][0] < 16 &&
						tree.energy > growEnergyCost && cell.y > -1)
					{
						f[cell.y][(cell.x + 1) & fWidthMask] = 0;
						newCells.push_back(Cell((cell.x + 1) & fWidthMask, cell.y));
						newCells.back().growP = tree.g.grow[cell.growP].nxt[1][0];
						newCells.back().typeP = tree.g.grow[cell.growP].nxt[1][1];
						tree.energy -= growEnergyCost;
						growed = 1;
					}

					cell.isSeed = (growed ^ 1); // __mbch;
				}
			}

			newTrees.push_back(tree);

			for (auto& cell : newCells)
			{
				newTrees.back().cells.push_back(cell);
				newTrees.back().reviseForSeed(newTrees.back().cells.size() - 1);
			}
		}
	}

	trees = newTrees;
	freeSeeds = newFreeSeeds;
	int treeI = 0;
	for (auto& tree : trees)
	{
		int cellI = 0;
		for (auto& cell : tree.cells)
		{
			f[cell.y][cell.x] = treeI + (cellI << fShift);

			cellI++;
		}

		treeI++;
	}
	
	seasonInnerCounter++;
	seasonInnerCounter &= 15;
	if (seasonInnerCounter == 0)
	{
		seasonP += seasonShiftByStep;
		seasonP &= fWidthMask;
	}
}