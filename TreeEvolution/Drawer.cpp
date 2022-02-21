#include "Drawer.h"

Drawer::Drawer()
{
	font.loadFromFile("font.ttf");

	seasonPanelCenter = windowWidth / 2;

	treeTimeScale = 1.0;
	//treeTimeScale = 0.1;
}

bool Drawer::switchEvent(sf::Event event, sf::RenderWindow& window)
{
	if (event.type == sf::Event::Closed)
	{
		return false;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		auto [x, y] = sf::Mouse::getPosition(window);

		if (y > seasonPanelY && y < seasonPanelY + seasonPanelHeight)
		{
			if(x * fWidth / windowWidth - (windowWidth / 8 / 2) < 0)
			{
				x = ((windowWidth / 8 / 2) * windowWidth + fWidth - 1) / fWidth;
			}

			if ((x * fWidth / windowWidth) + (windowWidth / 8 / 2) >= fWidth)
			{
				x = (fWidth - (windowWidth / 8 / 2) - 1) * windowWidth / fWidth;
			}

			seasonPanelCenter = x;
		}
	}

	if (event.type == sf::Event::MouseWheelMoved)
	{
		int d = event.mouseWheel.delta;

		if (d < 0)
		{
			treeTimeScale /= 1.1;
		}
		else
		{
			treeTimeScale *= 1.1;
		}
	}

	return true;
}

void Drawer::drawInfo(sf::RenderWindow& window)
{
	const int fontSize = 40;

	sf::Text text;

	text.setFont(font);
	text.setCharacterSize(fontSize);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(2);

	text.setPosition(10, 10);
	text.setString("Generation " + to_string(sm.globalIteration / 80));
	window.draw(text);

	int speedInt = (int)(treeTimeScale * 1000.0);
	int speed1 = speedInt / 1000;
	int speed2 = speedInt % 1000;
	string sSpeed2 = to_string(speed2);
	while (sSpeed2.size() < 3) { sSpeed2 = "0" + sSpeed2; }
	text.setPosition(350, 10);
	text.setString("Speed " + to_string(speed1) + "." + sSpeed2);
	window.draw(text);

	text.setPosition(700, 10);
	text.setString("Respawn Test " + to_string(sm.respawnIteration));
	window.draw(text);
}

void Drawer::drawSeasonPanel(sf::RenderWindow& window)
{
	int shift = fWidth / 4;
	sf::RectangleShape rect;

	for (int iter = -4; iter < 4; iter++)
	{
		int p = sm.seasonP + iter * shift;

		rect.setPosition(p * windowWidth / fWidth, seasonPanelY);
		rect.setSize(sf::Vector2f(windowWidth / 4, seasonPanelHeight));
		switch ((iter + 4) % 4)
		{
		case 0: rect.setFillColor(sf::Color(0, 206, 209)); break; // winter
		case 1: rect.setFillColor(sf::Color(107, 142, 35)); break; // spring
		case 2: rect.setFillColor(sf::Color(205, 205, 50)); break; // summer
		case 3: rect.setFillColor(sf::Color(178, 34, 34)); break; // autumn
		}

		window.draw(rect);
	}

	rect.setPosition(seasonPanelCenter - seasonWindowR, seasonPanelY);
	rect.setSize(sf::Vector2f(seasonWindowR * 2, seasonPanelHeight));
	rect.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(rect);
}

void Drawer::drawCells(sf::RenderWindow& window)
{
	sf::RectangleShape rect;

	rect.setSize(sf::Vector2f(7, 7));

	int startPos = seasonPanelCenter * fWidth / windowWidth - (windowWidth / 8 / 2);

	for (int i = 0; i < fHeight; i++)
	{
		for (int j = startPos; j < startPos + windowWidth / 8; j++)
		{
			if (sm.f[i][j] != -1)
			{
				rect.setPosition((j - startPos) * 8, windowHeight - 8 * (1 + i));
				
				if (sm.f[i][j] != cellSeedColorIndex)
				{
					int treeI = sm.f[i][j] & fMask;
					int treePos = sm.f[i][j] >> fShift;

					if (sm.trees[treeI].cells[treePos].isSeed == 1)
					{
						rect.setFillColor(sf::Color::White);
					}
					else
					{
						rect.setFillColor(sm.trees[treeI].color);
					}
				}
				else
				{
					rect.setFillColor(sf::Color::White);
				}

				window.draw(rect);
			}
		}
	}
	
}

void Drawer::draw(sf::RenderWindow& window)
{
	window.clear(sf::Color::Black);

	drawSeasonPanel(window);

	drawCells(window);

	drawInfo(window);
	
	window.display();
}

void Drawer::process(sf::RenderWindow& window)
{
	dTimer = 0;

	while (window.isOpen())
	{
		double dt = clock.getElapsedTime().asMicroseconds() / 1000.0;

		dTimer += dt;
		sTimer += dt;
		clock.restart();

		if (window.pollEvent(event))
		{
			if (!switchEvent(event, window))
			{
				window.close();
			}
		}

		if (sTimer > treeStepPeriod / treeTimeScale)
		{
			sm.processOneStep();

			sTimer = 0.0;
		}

		if (dTimer > drawPeriod)
		{
			draw(window);

			dTimer = 0.0;
		}
	}
}

void Drawer::show()
{
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Tree Evolution");

	window.setKeyRepeatEnabled(false);
	window.setPosition(sf::Vector2i(0, 0));

	process(window);
}