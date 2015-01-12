#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <random>
#include "Stix.h"
#include "Qix.h"
#include "Level.h"
#include "Input.h"

const double PI = 3.141592653589793238463;

class Game {
private:
	const int scale = 2;
	const sf::Vector2f baseResolution = sf::Vector2f(240, 284);

	sf::RenderWindow window;
	sf::Clock frameTimer;
	Input input;

	Stix stix;
	Qix qix;
	Level level;

	int score = 0, hiScore;

	std::random_device randomEngine;
	std::uniform_real_distribution<float> percentDistribution;

	sf::Font font;

	sf::Text scoreText, scoreCounter, percentText, percentCounter, hiScoreText, hiScoreCounter, gameOverText, restartText, levelText;
public:
	Game();
	~Game();

	void initText();

	sf::RenderWindow& getWindow();
	int getScale();
	float getRandomPercent();
	float restartFrameTimer();

	Input& getInput();
	Stix& getStix();
	Qix& getQix();
	Level& getLevel();

	void addScore(int value);
	void loadHiScore();
	void saveHiScore();

	void restart();

	void update();
	void display();
	void run();
};

