#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>
#include <vector>
#include <array>
#include "Stix.h"
#include "Level.h"
#include "Input.h"

class Game {
private:
	const int scale = 2;
	const sf::Vector2f baseResolution = sf::Vector2f(240, 256);

	sf::RenderWindow window;
	sf::Clock frameTimer;
	Input input;

	Stix stix;
	Level level;

public:
	Game();
	~Game();

	sf::RenderWindow& getWindow();
	int getScale();

	Input& getInput();
	Stix& getStix();
	Level& getLevel();
	 
	void update();
	void display();
	void run();
};

