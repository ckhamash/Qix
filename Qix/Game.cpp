#include "Game.h"


Game::Game()
	: window(sf::VideoMode(baseResolution.x * scale, baseResolution.y * scale), "Qix", sf::Style::Close)
	, input(this)
	, stix(this)
	, level(this) {
}

Game::~Game() { }

sf::RenderWindow& Game::getWindow() { return window; }

int Game::getScale() { return scale; }

Input& Game::getInput() { return input; }

Stix& Game::getStix() { return stix; }

Level& Game::getLevel() { return level; }

void Game::update() {
	float secondsSinceLastFrame = frameTimer.restart().asSeconds();

	input.update();

	stix.update(secondsSinceLastFrame);
}

void Game::display() {
	window.clear();

	level.draw();
	stix.draw();

	window.display();
}

void Game::run() {
	while (window.isOpen()) {
		update();
		display();
	}
}