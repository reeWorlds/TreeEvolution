#pragma once

#include "includes.h"
#include "Simulator.h"

#define windowWidth 1800
#define windowHeight 900

#define drawPeriod 25.0
#define treeStepPeriod 50.0

#define seasonPanelHeight 60
#define seasonPanelY 70
#define seasonWindowR (windowWidth * (windowWidth / 8 / 2) / fWidth)

class Drawer
{
	sf::Clock clock;
	double dTimer;
	double sTimer;
	sf::Event event;

	sf::Font font;

	Simulator sm;

	double treeTimeScale;

	int seasonPanelCenter;

	bool switchEvent(sf::Event event, sf::RenderWindow& window);

	void drawInfo(sf::RenderWindow& window);
	void drawSeasonPanel(sf::RenderWindow& window);
	void drawCells(sf::RenderWindow& window);

	void draw(sf::RenderWindow& window);


	void process(sf::RenderWindow& window);

public:

	Drawer();

	void show();

};