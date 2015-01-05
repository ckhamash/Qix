#include "Stix.h"
#include "Game.h"


Stix::Stix() { }

Stix::Stix(Game* pGame) 
	: pGame(pGame)
	, speed(200)
	, radius(2.5 * pGame->getScale()) {
	for (int i = 0; i < 2048; i++) {
		cwPath.push_back(Intersection(pGame, position));
		ccwPath.push_back(Intersection(pGame, position));
	}
	cwPath.clear();
	ccwPath.clear();
}

Stix::~Stix() { }

void Stix::initializeStix(sf::Vector2f position, Intersection* pCW, Intersection* pCCW) {
	this->position = position;
	this->pCW = pCW;
	this->pCCW = pCCW;
}

bool Stix::isIntersecting(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d) {
	float denominator = ((b.x - a.x) * (d.y - c.y)) - ((b.y - a.y) * (d.x - c.x));
	float numerator1 = ((a.y - c.y) * (d.x - c.x)) - ((a.x - c.x) * (d.y - c.y));
	float numerator2 = ((a.y - c.y) * (b.x - a.x)) - ((a.x - c.x) * (b.y - a.y));

	/*if (denominator == 0) 
		return numerator1 == 0 && numerator2 == 0;*/

	float r = numerator1 / denominator;
	float s = numerator2 / denominator;

	return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

void Stix::createNewPathIntersection() {
	// create new intersections
	cwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	ccwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	// point old to new
	cwPath[cwPath.size() - 2].setCW(&cwPath[cwPath.size() - 1]);
	ccwPath[cwPath.size() - 2].setCCW(&ccwPath[ccwPath.size() - 1]);
	//point new to old
	cwPath[cwPath.size() - 1].setCCW(&cwPath[cwPath.size() - 2]);
	ccwPath[ccwPath.size() - 1].setCW(&ccwPath[ccwPath.size() - 2]);

	std::cout << cwPath.size() << ": " << cwPath[cwPath.size() - 2].getPos().x << ", " << cwPath[cwPath.size() - 2].getPos().y << std::endl;
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

	cwPath[cwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));
	ccwPath[ccwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));

	cwPath[cwPath.size() - 1].setCW(pCW);
	ccwPath[ccwPath.size() - 1].setCCW(pCCW);

	// create two boundaries by getting cw and ccw neighbors
	std::vector<Intersection*> cwBoundary, ccwBoundary;
	cwBoundary.push_back(&cwPath[0]);
	//ccwBoundary.push_back(&ccwPath[ccwPath.size() - 1]);
	while (cwBoundary[cwBoundary.size() - 1]->getCW() != cwPath[0].getCCW()) {
		cwBoundary.push_back(cwBoundary[cwBoundary.size() - 1]->getCW());
	}
	Intersection cwCloser(pGame, cwPath[0].getCCW()->getPos());
	cwCloser.setCW(cwBoundary[0]);
	cwCloser.setCCW(cwBoundary[cwBoundary.size() - 1]);

	cwBoundary.push_back(&cwCloser);
	/*while (ccwBoundary[ccwBoundary.size() - 1]->getCCW() != cwBoundary[0]->getCCW())
		ccwBoundary.push_back(ccwBoundary[ccwBoundary.size() - 1]->getCCW());*/
	std::cout << "boundaries: " << cwBoundary.size() << std::endl;
	for (int i = 0; i < cwBoundary.size(); i++) {
		std::cout << cwBoundary[i]->getPos().x << ", " << cwBoundary[i]->getPos().y << std::endl;
	}

	pGame->getLevel().fillBoundary(cwBoundary, FillType::Slow);
	//pGame->getLevel().fillBoundary(ccwBoundary, FillType::Fast);
}

void Stix::detach(FillType type) {
	std::cout << "detach\n";
	fillType = type;
	onWall = false;

	cwPath.clear();
	ccwPath.clear();

	cwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	ccwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	cwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	ccwPath.push_back(Intersection(pGame, sf::Vector2f(sf::Vector2i(position))));
	cwPath[0].setCCW(pCCW);
	cwPath[0].setCW(&cwPath[1]);
	ccwPath[0].setCW(pCW);
	ccwPath[0].setCCW(&ccwPath[1]);
	cwPath[1].setCCW(&cwPath[0]);
	ccwPath[1].setCW(&ccwPath[1]);

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
		}

	}
}

void Stix::moveOffWall(float secondsSinceLastFrame) {
	sf::Vector2f newPosition = position;
	bool isVertical = (cwPath[cwPath.size() - 1].getPos().x == cwPath[cwPath.size() - 2].getPos().x);
	if (isVertical) {
		// get previous direction
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
			createNewPathIntersection();
			newPosition.x -= speed * secondsSinceLastFrame;
		}
		if (pGame->getInput().isCommandActive(Command::TurnRight) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown)) {
			createNewPathIntersection();
			newPosition.x += speed * secondsSinceLastFrame;
		}
	}
	else {
		// get previous direction
		bool isLeft = true;
		if (cwPath[cwPath.size() - 1].getPos().x > cwPath[cwPath.size() - 2].getPos().x)
			isLeft = false;

		if (pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnDown) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight)) {
			createNewPathIntersection();
			newPosition.y -= speed * secondsSinceLastFrame;
		}
		if (pGame->getInput().isCommandActive(Command::TurnDown) && !pGame->getInput().isCommandActive(Command::TurnUp) && !pGame->getInput().isCommandActive(Command::TurnLeft) && !pGame->getInput().isCommandActive(Command::TurnRight)) {
			createNewPathIntersection();
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
	for (int i = 0; i < pGame->getLevel().getIntersections().size(); i++) {
		if (isIntersecting(position, newPosition, pGame->getLevel().getIntersections()[i].getCW()->getPos(), pGame->getLevel().getIntersections()[i].getPos())) {
			intersecting = i;
			break;
		}
	}
	if (intersecting < 0) {
		position = newPosition;
		cwPath[cwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));
		ccwPath[ccwPath.size() - 1].setPos(sf::Vector2f(sf::Vector2i(position)));
	}
	else {
		reattach(pGame->getLevel().getIntersections()[intersecting].getCW(), &pGame->getLevel().getIntersections()[intersecting]);
	}
}

void Stix::update(float secondsSinceLastFrame) {
	if (onWall) {
		moveOnWall(secondsSinceLastFrame);
		checkDetach();
	}
	else {
		moveOffWall(secondsSinceLastFrame);
	}
}

void Stix::draw() {
	for (int i = 0; i < cwPath.size(); i++) {
		if (fillType == FillType::Fast)
			cwPath[i].draw(sf::Color::Cyan);
		else if (fillType == FillType::Slow)
			cwPath[i].draw(sf::Color::Red);
	}
	sf::RectangleShape rect(sf::Vector2f(radius * 2, radius * 2));
	rect.setOrigin(sf::Vector2f(rect.getGlobalBounds().width / 2, rect.getGlobalBounds().height / 2));
	rect.setPosition(position);
	rect.setFillColor(sf::Color::Red);
	pGame->getWindow().draw(rect);
}
