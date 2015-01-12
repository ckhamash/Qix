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

	float deathTime, respawnTime;
	float deathTimer = 0, respawnTimer = 0;

	float speed, radius;
	sf::Vector2f position;
	int lives = 3;
public:
	Stix();
	Stix(Game* pGame);
	~Stix();

	sf::Vector2f getPos();
	int getLives();

	void setCW(Intersection* pCW);
	void setCCW(Intersection* pCCW);

	void initializeStix(sf::Vector2f position, Intersection* pCW, Intersection* pCCW);
	bool isIntersecting(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d);
	bool isDying();
	bool isRespawning();

	void finalizePaths();
	void createCWPathIntersection(sf::Vector2f pos);
	void createCCWPathIntersection(sf::Vector2f pos);
	void checkPathCross();

	void restart();
	void kill();
	void respawn();
	void reattach(Intersection* pNewCW, Intersection* pNewCCW);
	void detach(FillType type);
	void checkDetach();

	void moveOnWall(float secondsSinceLastFrame);
	void moveOffWall(float secondsSinceLastFrame);

	void update(float secondsSinceLastFrame);

	void draw();
};

