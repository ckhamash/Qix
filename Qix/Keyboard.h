#pragma once
#include "SFML\Graphics.hpp"
#include <array>
class Keyboard {
private:
	std::array<bool, sf::Keyboard::KeyCount> keyStates;
public:
	Keyboard();
	~Keyboard();

	bool isKeyPressed(sf::Keyboard::Key key);
};