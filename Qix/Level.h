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
	std::vector<sf::RectangleShape> gridRects;
	std::vector<Intersection> intersections;
	std::vector<Intersection> fillIntersections;
	std::vector<Intersection*> qixBoundary;
	float percentFilled = 0;
	float levelUpPercent;
	int currentLevel = 0;

	int fastVal, slowVal, levelUpVal;

	float levelEndTime;
	float levelEndTimer;

	float fillTime;
	float fillTimer = 0;

	void connectIntersections(std::vector<Intersection> &inters);
	bool isBetweenHorizontal(sf::Vector2i target, sf::Vector2i a, sf::Vector2i b);
	bool isBetweenVertical(sf::Vector2i target, sf::Vector2i a, sf::Vector2i b);
public:
	Level();
	Level(Game* pGame);
	~Level();

	void initLevel();

	std::vector<Intersection>& getIntersections();
	std::vector<Intersection*>& getQixBoundary();
	int getFillPercent();
	int getLevelUpPercent();

	sf::Vector2i toGrid(sf::Vector2f position);
	sf::Vector2f toWindow(sf::Vector2i gridPos);

	void snapToGrid(std::vector<Intersection> &boundary);
	void replaceIntersections(std::vector<Intersection> &boundary);
	void newStixLine(std::vector<Intersection> cwBoundary, std::vector<Intersection> ccwBoundary, FillType fill);
	void fillBoundary(std::vector<Intersection> boundary, FillType fill); // fill by scanline
	float calculateFillPercent();
	void levelUp();
	void restart();

	void generateDrawRects();
	void draw();
};

