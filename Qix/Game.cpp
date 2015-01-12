#include "Game.h"


Game::Game()
	: window(sf::VideoMode(baseResolution.x * scale, baseResolution.y * scale), "Qix", sf::Style::Close)
	, input(this)
	, stix(this)
	, qix(this)
	, level(this) {
	percentDistribution = std::uniform_real_distribution<float>(0, 1);
	loadHiScore();
	initText();
}

Game::~Game() { }

sf::RenderWindow& Game::getWindow() { return window; }

void Game::initText() {
	font.loadFromFile("Fonts/qix-large.ttf");

	scoreText = sf::Text();
	scoreText.setFont(font);
	scoreText.setColor(sf::Color::White);
	scoreText.setCharacterSize(8 * scale);
	scoreText.setString("SCORE");
	scoreText.setPosition(window.getSize().x - scoreText.getGlobalBounds().width, 0);

	scoreCounter = sf::Text();
	scoreCounter.setFont(font);
	scoreCounter.setColor(sf::Color::White);
	scoreCounter.setCharacterSize(8 * scale);
	scoreCounter.setString(std::to_string(score));
	scoreCounter.setPosition(window.getSize().x - scoreCounter.getGlobalBounds().width, scoreText.getCharacterSize());

	hiScoreText = sf::Text();
	hiScoreText.setFont(font);
	hiScoreText.setColor(sf::Color::White);
	hiScoreText.setCharacterSize(8 * scale);
	hiScoreText.setString("HIGH SCORE");
	hiScoreText.setPosition(0, 0);

	hiScoreCounter = sf::Text();
	hiScoreCounter.setFont(font);
	hiScoreCounter.setColor(sf::Color::White);
	hiScoreCounter.setCharacterSize(8 * scale);
	hiScoreCounter.setString(std::to_string(hiScore));
	hiScoreCounter.setPosition(0, hiScoreText.getCharacterSize());

	percentText = sf::Text();
	percentText.setFont(font);
	percentText.setColor(sf::Color::White);
	percentText.setCharacterSize(8 * scale);
	percentText.setString("PERCENT");
	percentText.setPosition(window.getSize().x / 2 - percentText.getGlobalBounds().width / 2, hiScoreText.getCharacterSize() * 3);

	percentCounter = sf::Text();
	percentCounter.setFont(font);
	percentCounter.setColor(sf::Color::White);
	percentCounter.setCharacterSize(8 * scale);
	percentCounter.setString(std::to_string(level.getFillPercent()) + "% " + std::to_string(level.getLevelUpPercent()) + "%");
	percentCounter.setPosition(window.getSize().x / 2 - percentCounter.getGlobalBounds().width / 2, hiScoreText.getCharacterSize() * 3 + percentText.getCharacterSize());

	gameOverText = sf::Text();
	gameOverText.setFont(font);
	gameOverText.setColor(sf::Color::White);
	gameOverText.setCharacterSize(16 * scale);
	gameOverText.setString("GAME OVER");
	gameOverText.setPosition(window.getSize().x / 2 - gameOverText.getGlobalBounds().width / 2, window.getSize().y / 2);
}

int Game::getScale() { return scale; }

float Game::getRandomPercent() { return percentDistribution(randomEngine); }

float Game::restartFrameTimer() { return frameTimer.restart().asSeconds(); }

Input& Game::getInput() { return input; }

Stix& Game::getStix() { return stix; }

Qix& Game::getQix() { return qix; }

Level& Game::getLevel() { return level; }

void Game::addScore(int value) { 
	score += value;
	scoreCounter.setString(std::to_string(score));
	scoreCounter.setPosition(window.getSize().x - scoreCounter.getGlobalBounds().width, scoreText.getCharacterSize());
	if (score > hiScore) {
		hiScore = score;
		hiScoreCounter.setString(std::to_string(hiScore));
		saveHiScore();
	}
	percentCounter.setString(std::to_string(level.getFillPercent()) + "% " + std::to_string(level.getLevelUpPercent()) + "%");
	percentCounter.setPosition(window.getSize().x / 2 - percentCounter.getGlobalBounds().width / 2, hiScoreText.getCharacterSize() * 3 + percentText.getCharacterSize());
	std::cout << "score: " << score << std::endl;
}

void Game::loadHiScore() {
	std::ifstream saveFile;
	saveFile.open("hiscore.txt");
	if (saveFile.is_open()) {
		std::string line;
		std::getline(saveFile, line);
		hiScore = std::stoi(line);
	}
	else
		hiScore = 0;
	saveFile.close();
}

void Game::saveHiScore() {
	std::ofstream saveFile;
	saveFile.open("hiscore.txt");
	saveFile << hiScore;
	saveFile.close();
}

void Game::restart() {
	score = 0;
	stix.restart();
	level.restart();
}

void Game::update() {
	float secondsSinceLastFrame = frameTimer.restart().asSeconds();

	input.update();

	if (input.isCommandActive(Command::CloseWindow)) {
		window.close();
	}

	if (stix.getLives() > 0) {
		stix.update(secondsSinceLastFrame);
		qix.update(secondsSinceLastFrame);
	}
	else if (input.isCommandActive(Command::Start)) {
		restart();
	}
}

void Game::display() {
	window.clear();

	window.draw(scoreText);
	window.draw(scoreCounter);

	window.draw(hiScoreText);
	window.draw(hiScoreCounter);

	window.draw(percentText);
	window.draw(percentCounter);

	if (stix.getLives() < 1) {
		//game over
		window.draw(gameOverText);
	}

	level.draw();
	qix.draw();
	stix.draw();

	window.display();
}

void Game::run() {
	level.initLevel();
	while (window.isOpen()) {
		update();
		display();
	}
}