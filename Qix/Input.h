#pragma once
#include "SFML\Graphics.hpp"
#include "Keyboard.h"

class Game;

enum class Command { TurnUp, TurnRight, TurnDown, TurnLeft, FastDraw, SlowDraw, Start, CloseWindow };

class Input {
private:
	Game *pGame;

	Keyboard prevKeyboard, currKeyboard;
public:
	Input();
	Input(Game *game);
	~Input();

	bool isDown(sf::Keyboard::Key key);
	bool isUp(sf::Keyboard::Key key);
	bool justPressed(sf::Keyboard::Key key);
	bool justReleased(sf::Keyboard::Key key);

	bool isCommandActive(Command command);

	void update();
};