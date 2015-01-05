#pragma once
#include "SFML\Graphics.hpp"
#include "Intersection.h"
#include "Level.h"

class Game;

class Stix {
private:
	Game* pGame;

	Intersection* pCW = nullptr;
	Intersection* pCCW = nullptr;

	std::vector<Intersection> cwPath, ccwPath;

	bool onWall = true;
	FillType fillType = FillType::Fast;

	float speed, radius;
	sf::Vector2f position;
public:
	Stix();
	Stix(Game* pGame);
	~Stix();

	void initializeStix(sf::Vector2f position, Intersection* pCW, Intersection* pCCW);
	bool isIntersecting(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d);

	void createNewPathIntersection();

	void reattach(Intersection* pNewCW, Intersection* pNewCCW);
	void detach(FillType type);
	void checkDetach();

	void moveOnWall(float secondsSinceLastFrame);
	void moveOffWall(float secondsSinceLastFrame);

	void update(float secondsSinceLastFrame);

	void draw();
};

