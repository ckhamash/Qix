#include "Input.h"
#include "Game.h"

Input::Input() { }

Input::Input(Game *game)
	: pGame(game) { }

Input::~Input() { }

bool Input::isDown(sf::Keyboard::Key key) { return currKeyboard.isKeyPressed(key); }

bool Input::isUp(sf::Keyboard::Key key) { return !currKeyboard.isKeyPressed(key); }

bool Input::justPressed(sf::Keyboard::Key key) {
	return (!prevKeyboard.isKeyPressed(key) && isDown(key));
}

bool Input::justReleased(sf::Keyboard::Key key) {
	return (prevKeyboard.isKeyPressed(key) && !isDown(key));
}

bool Input::isCommandActive(Command command) {
	switch (command) {
	case Command::TurnUp:
		return isDown(sf::Keyboard::Up);
		break;
	case Command::TurnRight:
		return isDown(sf::Keyboard::Right);
		break;
	case Command::TurnDown:
		return isDown(sf::Keyboard::Down);
		break;
	case Command::TurnLeft:
		return isDown(sf::Keyboard::Left);
		break;
	case Command::FastDraw:
		return isDown(sf::Keyboard::Z);
		break;
	case Command::SlowDraw:
		return isDown(sf::Keyboard::X);
		break;
	case Command::Start:
		return justPressed(sf::Keyboard::Space);
		break;
	case Command::CloseWindow:
		sf::Event event;
		while (pGame->getWindow().pollEvent(event))
			if (event.type == sf::Event::Closed)
				return true;
		break;
	}
}

void Input::update() {
	prevKeyboard = currKeyboard;
	currKeyboard = Keyboard();
}