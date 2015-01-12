#include "Stix.h"
#include "Game.h"


Stix::Stix() { }

Stix::Stix(Game* pGame) 
	: pGame(pGame)
	, speed(100 * pGame->getScale())
	, radius(2.5 * pGame->getScale())
	, deathTime(2)
	, respawnTime(0.1f) {
	deathTimer = deathTime;
	respawnTimer = respawnTime;
	for (int i = 0; i < 2048; i++) {
		cwPath.push_back(Intersection(pGame, position));
		ccwPath.push_back(Intersection(pGame, position));
	}
	cwPath.clear();
	ccwPath.clear();
}

Stix::~Stix() { }

sf::Vector2f Stix::getPos() { return position; }

int Stix::getLives() { return lives; }

void Stix::setCW(Intersection* pCW) { this->pCW = pCW; }
void Stix::setCCW(Intersection* pCCW) { this->pCCW = pCCW; }

void Stix::initializeStix(sf::Vector2f position, Intersection* pCW, Intersection* pCCW) {
	this->position = position;
	this->pCW = pCW;
	this->pCCW = pCCW;
}

bool Stix::isIntersecting(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d) {
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

bool Stix::isDying() { return deathTimer < deathTime; }

bool Stix::isRespawning() { return respawnTime < respawnTimer; }

void Stix::createCWPathIntersection(sf::Vector2f pos) {
	std::cout << "CW-" << cwPath.size() << ": " << cwPath[cwPath.size() - 1].getPos().x << ", " << cwPath[cwPath.size() - 1].getPos().y << std::endl;
	// create new intersections
	cwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(pos))));
	// point old to new
	cwPath[cwPath.size() - 2].setCW(&cwPath[cwPath.size() - 1]);
	//point new to old
	cwPath[cwPath.size() - 1].setCCW(&cwPath[cwPath.size() - 2]);

}

void Stix::createCCWPathIntersection(sf::Vector2f pos) {
	std::cout << "CCW-" << ccwPath.size() << ": " << ccwPath[ccwPath.size() - 1].getPos().x << ", " << ccwPath[ccwPath.size() - 1].getPos().y << std::endl;
	ccwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(pos))));
	ccwPath[ccwPath.size() - 2].setCCW(&ccwPath[ccwPath.size() - 1]);
	ccwPath[ccwPath.size() - 1].setCW(&ccwPath[ccwPath.size() - 2]);
}

void Stix::checkPathCross() {
	int last = cwPath.size() - 1;
	if (cwPath.size() > 4) {
		for (int i = 0; i < cwPath.size() - 3; i++) {
			if (isIntersecting(cwPath[last - 1].getPos(), cwPath[last].getPos(), cwPath[i].getPos(), cwPath[i + 1].getPos())) {
				kill();
				break;
			}
		}
	}
	bool intersecting = false;
	if (cwPath.size() > 1) {
		for (int i = 0; i < cwPath.size() - 1; i++) {
			for (int j = 0; j < pGame->getQix().getPositions().size() - 1; j++) {
				if (isIntersecting(cwPath[i].getPos(), cwPath[i + 1].getPos(), pGame->getQix().getPositions()[j], pGame->getQix().getPositions()[j + 1])) {
					intersecting = true;
					kill();
					break;
				}
			}
			if (intersecting)
				break;
		}
	}
}

void Stix::kill() {
	deathTimer = 0;
}

void Stix::restart() {
	lives = 3;
}

void Stix::respawn() {
	lives--;
	position = cwPath[0].getPos();
	pCCW = cwPath[0].getCCW();
	pCW = ccwPath[0].getCW();
	cwPath.clear();
	ccwPath.clear();
	onWall = true;
	respawnTimer = 0;
}

void Stix::reattach(Intersection* pNewCW, Intersection* pNewCCW) {
	std::cout << "reattach\n";
	pCW = pNewCW;
	pCCW = pNewCCW;
	onWall = true;

	bool isVertical = (pCW->getPos().x == pCCW->getPos().x);
	if (isVertical)
		position.x = pCW->getPos().x;
	else
		position.y = pCW->getPos().y;

	// get intersection for last position of player path
	cwPath[cwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));
	ccwPath[ccwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));

	std::cout << "CW-" << cwPath.size() << ": " << cwPath[cwPath.size() - 1].getPos().x << ", " << cwPath[cwPath.size() - 1].getPos().y << std::endl;
	std::cout << "CCW-" << ccwPath.size() << ": " << ccwPath[ccwPath.size() - 1].getPos().x << ", " << ccwPath[ccwPath.size() - 1].getPos().y << std::endl;

	// set neighbors for latest intersections
	cwPath[cwPath.size() - 1].setCW(pCW);
	ccwPath[ccwPath.size() - 1].setCCW(pCCW);
	cwPath[cwPath.size() - 1].setCCW(&cwPath[cwPath.size() - 2]);
	ccwPath[ccwPath.size() - 1].setCW(&ccwPath[ccwPath.size() - 2]);
	// point old to new
	ccwPath[ccwPath.size() - 2].setCCW(&ccwPath[ccwPath.size() - 1]);
	cwPath[cwPath.size() - 2].setCW(&cwPath[cwPath.size() - 1]);

	// check if last intersection is on same wall as first intersection
	if (cwPath[cwPath.size() - 1].getCW() == ccwPath[0].getCW()) {
		// set up next intersection according to relative location of first and last intersections
		std::cout << "same wall ";
		sf::Vector2f cwPos = ccwPath[0].getCW()->getPos();
		sf::Vector2f first = cwPath[0].getPos();
		sf::Vector2f last = cwPath[cwPath.size() - 1].getPos();
		first -= cwPos;
		last -= cwPos;
		float firstDist = std::sqrt(first.x * first.x + first.y * first.y);
		float lastDist = std::sqrt(last.x * last.x + last.y * last.y);
		if (lastDist < firstDist) {
			std::cout << "closer to cw\n";
			createCWPathIntersection(cwPath[cwPath.size() - 1].getCW()->getPos());
			pNewCW = pNewCW->getCW();
			cwPath[cwPath.size() - 1].setCW(pNewCW);
		}
		else {
			std::cout << "further from cw\n";
			createCCWPathIntersection(ccwPath[ccwPath.size() - 1].getCCW()->getPos());
			pNewCCW = pNewCCW->getCCW();
			ccwPath[ccwPath.size() - 1].setCCW(pNewCCW);
		}
	}

	// create new CW intersections
	while (cwPath[cwPath.size() - 1].getCW() != ccwPath[0].getCW()) {
		createCWPathIntersection(cwPath[cwPath.size() - 1].getCW()->getPos());
		pNewCW = pNewCW->getCW();
		cwPath[cwPath.size() - 1].setCW(pNewCW);
	}


	while (ccwPath[ccwPath.size() - 1].getCCW() != cwPath[0].getCCW()) {
		createCCWPathIntersection(ccwPath[ccwPath.size() - 1].getCCW()->getPos());
		pNewCCW = pNewCCW->getCCW();
		ccwPath[ccwPath.size() - 1].setCCW(pNewCCW);
	}

	//cwPath.erase(cwPath.begin() + cwPath.size() - 1);
	//ccwPath.erase(ccwPath.begin() + ccwPath.size() - 1);
	// close cwPath
	cwPath[cwPath.size() - 1].setCW(&cwPath[0]);
	cwPath[0].setCCW(&cwPath[cwPath.size() - 1]);
	// close ccwPath
	ccwPath[ccwPath.size() - 1].setCCW(&ccwPath[0]);
	ccwPath[0].setCW(&ccwPath[ccwPath.size() - 1]);

	// remove last intersections if on same location as first (player started line at a corner)
	if (cwPath[0].getPos() == cwPath[cwPath.size() - 1].getPos()) {
		cwPath[cwPath.size() - 1].bridge();
		cwPath.erase(cwPath.begin() + cwPath.size() - 1);
		std::cout << "sameCW\n";
	}
	if (ccwPath[0].getPos() == ccwPath[ccwPath.size() - 1].getPos()) {
		ccwPath[ccwPath.size() - 1].bridge();
		ccwPath.erase(ccwPath.begin() + ccwPath.size() - 1);
		std::cout << "sameCCW\n";
	}

	for (int i = 0; i < cwPath.size(); i++) {
		if (cwPath[i].isRedundant()) {
			std::cout << "CWredundant: " << cwPath[i].getPos().x << ", " << cwPath[i].getPos().y << "\n";
			cwPath[i].bridge();
			//cwPath.erase(cwPath.begin() + i);
			//i--;
		}
	}
	for (int i = 0; i < ccwPath.size(); i++) {
		if (ccwPath[i].isRedundant()) {
			std::cout << "CCWredundant: " << ccwPath[i].getPos().x << ", " << ccwPath[i].getPos().y << "\n";
			ccwPath[i].bridge();
			//ccwPath.erase(ccwPath.begin() + i);
			//i--;
		}
	}

	std::cout << "CW-" << cwPath.size() << std::endl;
	std::cout << "CCW-" << ccwPath.size() << std::endl;

	pGame->getLevel().newStixLine(cwPath, ccwPath, fillType);

	cwPath.clear();
	ccwPath.clear();
}

void Stix::detach(FillType type) {
	std::cout << "detach\n";
	fillType = type;
	onWall = false;

	cwPath.clear();
	ccwPath.clear();

	cwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	ccwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	std::cout << "CW-" << cwPath.size() << ": " << cwPath[cwPath.size() - 1].getPos().x << ", " << cwPath[cwPath.size() - 1].getPos().y << std::endl;
	std::cout << "CCW-" << ccwPath.size() << ": " << ccwPath[ccwPath.size() - 1].getPos().x << ", " << ccwPath[ccwPath.size() - 1].getPos().y << std::endl;
	cwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	ccwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	cwPath[0].setCCW(pCCW);
	cwPath[0].setCW(&cwPath[1]);
	ccwPath[0].setCW(pCW);
	ccwPath[0].setCCW(&ccwPath[1]);
	cwPath[1].setCCW(&cwPath[0]);
	ccwPath[1].setCW(&ccwPath[0]);
}

void Stix::checkDetach() {
	// get orientation
	bool isVertical = (pCW->getPos().x == pCCW->getPos().x);
	if (isVertical) {
		// get detach direction
		bool detachRight = true;
		if (pCW->getPos().y > pCCW->getPos().y)
			detachRight = false;

		if (detachRight && pGame->getInput().isCommandActive(Command::TurnRight)) {
			if (pGame->getInput().isCommandActive(Command::FastDraw)) {
				detach(FillType::Fast);
				position.x += pGame->getScale();
			}
			else if (pGame->getInput().isCommandActive(Command::SlowDraw)) {
				detach(FillType::Slow);
				position.x += pGame->getScale();
			}
		}
		else if (!detachRight && pGame->getInput().isCommandActive(Command::TurnLeft)) {
			if (pGame->getInput().isCommandActive(Command::FastDraw)) {
				detach(FillType::Fast);
				position.x -= pGame->getScale();
			}
			else if (pGame->getInput().isCommandActive(Command::SlowDraw)) {
				detach(FillType::Slow);
				position.x -= pGame->getScale();
			}
		}
	}
	else {
		// get detach direction
		bool detachDown = true;
		if (pCW->getPos().x < pCCW->getPos().x)
			detachDown = false;

		if (detachDown && pGame->getInput().isCommandActive(Command::TurnDown)) {
			if (pGame->getInput().isCommandActive(Command::FastDraw)) {
				detach(FillType::Fast);
				position.y += pGame->getScale();
			}
			else if (pGame->getInput().isCommandActive(Command::SlowDraw)) {
				detach(FillType::Slow);
				position.y += pGame->getScale();
			}
		}
		else if (!detachDown && pGame->getInput().isCommandActive(Command::TurnUp)) {
			if (pGame->getInput().isCommandActive(Command::FastDraw)) {
				detach(FillType::Fast);
				position.y -= pGame->getScale();
			}
			else if (pGame->getInput().isCommandActive(Command::SlowDraw)) {
				detach(FillType::Slow);
				position.y -= pGame->getScale();
			}
		}
	}
}

void Stix::moveOnWall(float secondsSinceLastFrame) { 
	// get orientation
	bool isVertical = (pCW->getPos().x == pCCW->getPos().x);
	// check inputs, move according to orientation
	if (isVertical) { // can move up/down or change wall if at an intersection
		if (pGame->getInput().isCommandActive(Command::TurnUp)) 
			position.y -= speed * secondsSinceLastFrame;
		
		if (pGame->getInput().isCommandActive(Command::TurnDown)) 
			position.y += speed * secondsSinceLastFrame;
		
		// check if passed an intersection
		Intersection* top = pCW;
		Intersection* bottom = pCCW;
		if (bottom->getPos().y < top->getPos().y) {
			Intersection* temp = top;
			top = bottom;
			bottom = temp;
		}

		if (position.y < top->getPos().y)
			position = top->getPos();
		if (position.y > bottom->getPos().y)
			position = bottom->getPos();

		if (position == pCW->getPos()) { // at clockwise intersection
			// get direction of clockwise neighbor
			bool isRight = true;
			if (pCW->getPos().x > pCW->getCW()->getPos().x)
				isRight = false;

			if (isRight && pGame->getInput().isCommandActive(Command::TurnRight)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
			if (!isRight && pGame->getInput().isCommandActive(Command::TurnLeft)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
			if (pCW == top && pGame->getInput().isCommandActive(Command::TurnUp)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
			if (pCW == bottom && pGame->getInput().isCommandActive(Command::TurnDown)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
		}
		else if (position == pCCW->getPos()) { // at counter-clockwise intersection
			// get direction of counter-clockwise neighbor
			bool isRight = true;
			if (pCCW->getPos().x > pCCW->getCCW()->getPos().x)
				isRight = false;

			if (isRight && pGame->getInput().isCommandActive(Command::TurnRight)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
			if (!isRight && pGame->getInput().isCommandActive(Command::TurnLeft)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
			if (pCCW == top && pGame->getInput().isCommandActive(Command::TurnUp)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
			if (pCCW == bottom && pGame->getInput().isCommandActive(Command::TurnDown)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
		}
	}
	else {// can move left/right or change wall if at an intersection
		if (pGame->getInput().isCommandActive(Command::TurnLeft))
			position.x -= speed * secondsSinceLastFrame;

		if (pGame->getInput().isCommandActive(Command::TurnRight))
			position.x += speed * secondsSinceLastFrame;

		// check if passed an intersection
		Intersection* left = pCW;
		Intersection* right = pCCW;
		if (right->getPos().x < left->getPos().x) {
			Intersection* temp = left;
			left = right;
			right = temp;
		}

		if (position.x < left->getPos().x)
			position = left->getPos();
		if (position.x > right->getPos().x)
			position = right->getPos();

		if (position == pCW->getPos()) { // at clockwise intersection
			// get direction of clockwise neighbor
			bool isDown = true;
			if (pCW->getPos().y > pCW->getCW()->getPos().y)
				isDown = false;

			if (isDown && pGame->getInput().isCommandActive(Command::TurnDown)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
			if (!isDown && pGame->getInput().isCommandActive(Command::TurnUp)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
			if (pCW == right && pGame->getInput().isCommandActive(Command::TurnRight)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
			if (pCW == left && pGame->getInput().isCommandActive(Command::TurnLeft)) {
				pCCW = pCW;
				pCW = pCW->getCW();
			}
		}
		else if (position == pCCW->getPos()) { // at counter-clockwise intersection
			// get direction of counter-clockwise neighbor
			bool isDown = true;
			if (pCCW->getPos().y > pCCW->getCCW()->getPos().y)
				isDown = false;

			if (isDown && pGame->getInput().isCommandActive(Command::TurnDown)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
			if (!isDown && pGame->getInput().isCommandActive(Command::TurnUp)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
			if (pCCW == right && pGame->getInput().isCommandActive(Command::TurnRight)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
			if (pCCW == left && pGame->getInput().isCommandActive(Command::TurnLeft)) {
				pCW = pCCW;
				pCCW = pCCW->getCCW();
			}
		}

	}
}

void Stix::moveOffWall(float secondsSinceLastFrame) {
	sf::Vector2f newPosition = position;
	bool isVertical = (cwPath[cwPath.size() - 1].getPos().x == cwPath[cwPath.size() - 2].getPos().x);
	bool isHorizontal = (cwPath[cwPath.size() - 1].getPos().y == cwPath[cwPath.size() - 2].getPos().y);
	if (isVertical && !isHorizontal) {
		// get previous direction
		std::cout << "vertical\n";
		bool isUp = true;
		if (cwPath[cwPath.size() - 1].getPos().y > cwPath[cwPath.size() - 2].getPos().y)
			isUp = false;

		if (isUp && pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight)) {
			newPosition.y -= speed * secondsSinceLastFrame;
		}
		if (!isUp && pGame->getInput().isCommandActive(Command::TurnDown) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight)) {
			newPosition.y += speed * secondsSinceLastFrame;
		}
		if (pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown)) {
			createCWPathIntersection(position);
			createCCWPathIntersection(position);
			newPosition.x -= speed * secondsSinceLastFrame;
		}
		if (pGame->getInput().isCommandActive(Command::TurnRight) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown)) {
			createCWPathIntersection(position);
			createCCWPathIntersection(position);
			newPosition.x += speed * secondsSinceLastFrame;
		}
	}
	if (isHorizontal && !isVertical) {
		// get previous direction
		std::cout << "horizontal\n";
		bool isLeft = true;
		if (cwPath[cwPath.size() - 1].getPos().x > cwPath[cwPath.size() - 2].getPos().x)
			isLeft = false;

		if (pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight)) {
			createCWPathIntersection(position);
			createCCWPathIntersection(position);
			newPosition.y -= speed * secondsSinceLastFrame;
		}
		if (pGame->getInput().isCommandActive(Command::TurnDown) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight)) {
			createCWPathIntersection(position);
			createCCWPathIntersection(position);
			newPosition.y += speed * secondsSinceLastFrame;
		}
		if (isLeft && pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown)) {
			newPosition.x -= speed * secondsSinceLastFrame;
		}
		if (!isLeft && pGame->getInput().isCommandActive(Command::TurnRight) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown)) {
			newPosition.x += speed * secondsSinceLastFrame;
		}
	}

	int intersecting = -1;
	for (int i = 0; i < pGame->getLevel().getQixBoundary().size(); i++) {
		if (isIntersecting(position, newPosition, pGame->getLevel().getQixBoundary()[i]->getCW()->getPos(), pGame->getLevel().getQixBoundary()[i]->getPos())) {
			intersecting = i;
			std::cout << "intersect\n";
			break;
		}
	}
	if (intersecting < 0) {
		/*sf::Vector2f collisionCheck = newPosition - position;
		float length = std::sqrt(collisionCheck.x * collisionCheck.x + collisionCheck.y * collisionCheck.y);
		collisionCheck /= length; // now a unit vector
		collisionCheck *= radius;
		std::cout << std::sqrt(collisionCheck.x * collisionCheck.x + collisionCheck.y * collisionCheck.y) << std::endl;
		collisionCheck = position + collisionCheck;*/
		position = newPosition;
		cwPath[cwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));
		ccwPath[ccwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));
	}
	else if (intersecting >= 0) {
		reattach(pGame->getLevel().getQixBoundary()[intersecting]->getCW(), pGame->getLevel().getQixBoundary()[intersecting]);
	}
}

void Stix::update(float secondsSinceLastFrame) {
	if (respawnTimer >= respawnTime && deathTimer >= deathTime) {
		if (onWall) {
			moveOnWall(secondsSinceLastFrame);
			checkDetach();
		}
		else {
			moveOffWall(secondsSinceLastFrame);
			checkPathCross();
		}
	}
	else {
		deathTimer += secondsSinceLastFrame;
		if (deathTimer >= deathTime && respawnTimer >= respawnTime) {
			respawn();
		}
		respawnTimer += secondsSinceLastFrame;
	}
}

void Stix::draw() {
	for (int i = 1; i < cwPath.size(); i++) {
		if (fillType == FillType::Fast)
			cwPath[i].draw(sf::Color::Cyan);
		else if (fillType == FillType::Slow)
			cwPath[i].draw(sf::Color::Red);
	}

	sf::RectangleShape rect(sf::Vector2f(radius * 2, radius * 2));
	rect.setOrigin(sf::Vector2f(rect.getGlobalBounds().width / 2, rect.getGlobalBounds().height / 2));
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::Red);
	rect.setOutlineThickness(pGame->getScale());
	rect.setRotation(45);
	rect.setScale(0.5f, 0.5f);
	for (int i = 0; i < lives; i++) {
		rect.setPosition(pGame->getWindow().getSize().x - 2 * rect.getGlobalBounds().width, (24 + 8 * i) * pGame->getScale());
		pGame->getWindow().draw(rect);
	}


	rect.setPosition(position);
	rect.setScale(1, 1);
	if (isDying()) {
		float scale = 1 + (deathTimer / deathTime) * 150;
		rect.setScale(scale, scale);
	}
	pGame->getWindow().draw(rect);


}
