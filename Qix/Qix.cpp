#include "Qix.h"
#include "Game.h"

Qix::Qix() { }

Qix::Qix(Game* pGame) 
	: pGame(pGame)
	, speed(50 * pGame->getScale()) { }

Qix::~Qix() { }

void Qix::initQix(std::vector<sf::Vector2f> initPositions) {
	positions.clear();
	velocities.clear();
	for (int i = 0; i < initPositions.size(); i++) {
		positions.push_back(initPositions[i]);
		velocities.push_back(sf::Vector2f()); 
		randomizeVelocity(i);
	}
}

std::vector<sf::Vector2f> Qix::getPositions() { return positions; }

bool Qix::isIntersecting() {
	for (int i = 0; i < positions.size() - 1; i++) {
		for (int j = 0; j < pGame->getLevel().getQixBoundary().size(); j++) {
			if (isIntersecting(positions[i], positions[i + 1], pGame->getLevel().getQixBoundary()[j]->getPos(), pGame->getLevel().getQixBoundary()[j]->getCW()->getPos())) {
				return true;
			}
		}
	}
	return false;
}

bool Qix::isIntersecting(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d) {
	float denominator = ((b.x - a.x) * (d.y - c.y)) - ((b.y - a.y) * (d.x - c.x));
	float numerator1 = ((a.y - c.y) * (d.x - c.x)) - ((a.x - c.x) * (d.y - c.y));
	float numerator2 = ((a.y - c.y) * (b.x - a.x)) - ((a.x - c.x) * (b.y - a.y));

	if (denominator == 0) // parallel
		return false;
	/*if (denominator == 0)
	return numerator1 == 0 && numerator2 == 0;*/

	float r = numerator1 / denominator;
	float s = numerator2 / denominator;

	return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

void Qix::randomizeVelocity(int index) {
	float xVel = (pGame->getRandomPercent() * 2 * speed) - speed;
	float yVel = std::sqrt(speed * speed - xVel * xVel);
	if (pGame->getRandomPercent() > 0.5f) {
		yVel *= -1;
	}
	velocities[index] = sf::Vector2f(xVel, yVel);
	//std::cout << "speed: " << xVel << ", " << yVel << std::endl;
}

void Qix::move(int index, float secondsSinceLastFrame) {
	if (isIntersecting()) { // if already intersecting set pos to a different position of the qix
		int i = index;
		while (i == index)
			i = pGame->getRandomPercent() * positions.size();
		positions[index] = positions[i];
	}
	positions[index] += velocities[index] * secondsSinceLastFrame;
	// check collision
	//std::cout << "move: " << positions[index].x << ", " << positions[index].y << std::endl;
	if (isIntersecting()) {
		positions[index] -= velocities[index] * secondsSinceLastFrame;
		// get random direction velocity
		randomizeVelocity(index);
		// try moving again
		move(index, secondsSinceLastFrame);
	}

}

void Qix::update(float secondsSinceLastFrame) {
	for (int i = 0; i < positions.size(); i++) {
		move(i, secondsSinceLastFrame);
	}
}

void Qix::draw() {
	/*sf::RectangleShape dot(sf::Vector2f(pGame->getScale(), pGame->getScale()));
	dot.setOrigin(dot.getSize().x / 2, dot.getSize().y / 2);
	for (int i = 0; i < positions.size(); i++) {
		dot.setPosition(positions[i]);
		pGame->getWindow().draw(dot);
	}*/

	for (int i = 0; i < positions.size() - 1; i++) {
		sf::Vector2f difference = positions[i] - positions[i + 1];
		float length = std::sqrt(difference.x * difference.x + difference.y * difference.y) + pGame->getScale();
		float thickness = pGame->getScale();

		sf::RectangleShape line(sf::Vector2f(length, thickness));
		line.setOrigin(pGame->getScale() / 2, pGame->getScale() / 2);
		line.setPosition(positions[i]);

		if (difference.y < 0)
			line.setRotation(std::atan(-difference.x / difference.y) * 180.0f / PI + 90.0f);
		else
			line.setRotation(std::atan(-difference.x / difference.y) * 180.0f / PI - 90.0f);

		pGame->getWindow().draw(line);
	}
}