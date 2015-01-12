#pragma once
#include "SFML\Graphics.hpp"

class Game;

class Qix {
private:
	Game* pGame;

	std::vector<sf::Vector2f> positions;
	std::vector<sf::Vector2f> velocities;
	float speed;
public:
	Qix();
	Qix(Game* pGame);
	~Qix();

	void initQix(std::vector<sf::Vector2f> initPositions);

	std::vector<sf::Vector2f> getPositions();

	bool isIntersecting();
	bool isIntersecting(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d);

	void randomizeVelocity(int index);
	void move(int index, float secondsSinceLastFrame);
	void update(float secondsSinceLastFrame);
	void draw();
};

