#include "Intersection.h"
#include "Game.h"

Intersection::Intersection() { }

Intersection::Intersection(Game* pGame, sf::Vector2f position) 
	: pGame(pGame), pos(position) {
}

Intersection::~Intersection() { }

sf::Vector2f Intersection::getPos() { return pos; }
void Intersection::setPos(sf::Vector2f position) { pos = position; }

Intersection* Intersection::getCW() { return pCW; }
void Intersection::setCW(Intersection* pIntersection) { pCW = pIntersection; }
Intersection* Intersection::getCCW() { return pCCW; }
void Intersection::setCCW(Intersection* pIntersection) { pCCW = pIntersection; }

Intersection* Intersection::getHorizontal() { 
	if (pCW->getPos().y == pos.y)
		return pCW;
	else if (pCCW->getPos().y == pos.y)
		return pCCW;
	return nullptr;
}

Intersection* Intersection::getVertical() {
	if (pCW->getPos().x == pos.x)
		return pCW;
	else if (pCCW->getPos().x == pos.x)
		return pCCW;
	return nullptr;
}

bool Intersection::isRedundant() {
	if (pCW->getPos().x == pCCW->getPos().x || pCW->getPos().y == pCCW->getPos().y)
		return true;
	return false;
}

bool Intersection::isInsideCorner() {
	// get horizontal neighbor direction: left = -1, right = 1
	int horizontalDirection = 0;
	if (pos.x > getHorizontal()->getPos().x)
		horizontalDirection = -1;
	else if(pos.x < getHorizontal()->getPos().x)
		horizontalDirection = 1;

	// get vertical neighbor direction: up = -1, down = 1
	int verticalDirection = 0;
	if (pos.y > getVertical()->getPos().y)
		verticalDirection = -1;
	else if (pos.y < getVertical()->getPos().y)
		verticalDirection = 1;

	if (pCW == getHorizontal()) {
		if (horizontalDirection == -1 && verticalDirection == 1)
			return true;
		if (horizontalDirection == 1 && verticalDirection == -1)
			return true;
	}
	if (pCW == getVertical()) {
		if (verticalDirection == 1 && horizontalDirection == 1)
			return true;
		if (verticalDirection == -1 && horizontalDirection == -1)
			return true;
	}
	return false;
}

void Intersection::bridge() {
	pCW->setCCW(pCCW);
	pCCW->setCW(pCW);
}

void Intersection::draw(sf::Color color) {
	sf::RectangleShape line;
	line.setPosition(sf::Vector2f(pos));
	line.setFillColor(color);
	if (pCW != nullptr) {
		if (pCW->getPos().x == pos.x) { // vertical
			float length = std::abs(pCW->getPos().y - pos.y);
			float thickness = pGame->getScale();
			line.setSize(sf::Vector2f(thickness, length + thickness));
			if (pCW->getPos().y > pos.y) {
				line.setOrigin(thickness / 2, thickness / 2);
				pGame->getWindow().draw(line);
			}
			else {
				line.setOrigin(thickness / 2, length + thickness / 2);
				pGame->getWindow().draw(line);
			}
		}
		else if (pCW->getPos().y == pos.y) { // horizontal
			float length = std::abs(pCW->getPos().x - pos.x);
			float thickness = pGame->getScale();
			line.setSize(sf::Vector2f(length + thickness, thickness));
			if (pCW->getPos().x > pos.x) {
				line.setOrigin(thickness / 2, thickness / 2);
				pGame->getWindow().draw(line);
			}
			else {
				line.setOrigin(length + thickness / 2, thickness / 2);
				pGame->getWindow().draw(line);
			}
		}
	}
	if (pCCW != nullptr) {
		if (pCCW->getPos().x == pos.x) { // vertical
			float length = std::abs(pCCW->getPos().y - pos.y);
			float thickness = pGame->getScale();
			line.setSize(sf::Vector2f(thickness, length + thickness));
			if (pCCW->getPos().y > pos.y) {
				line.setOrigin(thickness / 2, thickness / 2);
				pGame->getWindow().draw(line);
			}
			else {
				line.setOrigin(thickness / 2, length + thickness / 2);
				pGame->getWindow().draw(line);
			}
		}
		else if (pCCW->getPos().y == pos.y) { // horizontal
			float length = std::abs(pCCW->getPos().x - pos.x);
			float thickness = pGame->getScale();
			line.setSize(sf::Vector2f(length + thickness, thickness));
			if (pCCW->getPos().x > pos.x) {
				line.setOrigin(thickness / 2, thickness / 2);
				pGame->getWindow().draw(line);
			}
			else {
				line.setOrigin(length + thickness / 2, thickness / 2);
				pGame->getWindow().draw(line);
			}
		}
	}
}