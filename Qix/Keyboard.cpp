#include "Keyboard.h"

Keyboard::Keyboard() {
	for (int i = 0; i < keyStates.size(); i++) {
		keyStates[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));
	}
}

Keyboard::~Keyboard() { }

bool Keyboard::isKeyPressed(sf::Keyboard::Key key) { return keyStates[key]; }