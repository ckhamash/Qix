#pragma once
#include "SFML\Graphics.hpp"
#include "Intersection.h"

class Game;

enum class FillType { Empty, Fast, Slow };

class Level {
private:
	Game* pGame;

	sf::FloatRect levelBounds;
	std::vector<std::vector<FillType>> grid; // 0 = empty, 1 = fastdraw, 2 = slowdraw
	std::vector<Intersection> intersections;
	std::vector<Intersection*> qixBounds;

	void initLevel();
	void connectIntersections(std::vector<Intersection> &inters);
	bool isBetweenHorizontal(sf::Vector2i target, sf::Vector2i a, sf::Vector2i b);
	bool isBetweenVertical(sf::Vector2i target, sf::Vector2i a, sf::Vector2i b);
public:
	Level();
	Level(Game* pGame);
	~Level();

	std::vector<Intersection>& getIntersections();

	sf::Vector2i toGrid(sf::Vector2f position);
	sf::Vector2f toWindow(sf::Vector2i gridPos);
	void fillBoundary(std::vector<Intersection*> boundary, FillType fill); // fill by scanline

	void draw();
};

