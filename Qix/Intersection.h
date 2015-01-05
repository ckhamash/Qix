#pragma once
#include "SFML\Graphics.hpp"

class Game;

class Intersection {
private:
	Game* pGame;
	sf::Vector2f pos;

	Intersection* pCW = nullptr;
	Intersection* pCCW = nullptr;
public:
	Intersection();
	Intersection(Game* pGame, sf::Vector2f position);
	~Intersection();

	sf::Vector2f getPos();
	void setPos(sf::Vector2f position);

	Intersection* getCW();
	void setCW(Intersection* pIntersection);
	Intersection* getCCW();
	void setCCW(Intersection* pIntersection);

	Intersection* getHorizontal();
	Intersection* getVertical();

	bool isRedundant();

	void bridge(); // causes the neighbors of this intersection to point to each other

	void draw(sf::Color color);
};

