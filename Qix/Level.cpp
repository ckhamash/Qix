#include "Level.h"
#include "Game.h"

Level::Level() { }

Level::Level(Game* pGame) 
	: pGame(pGame)
	, fastVal(250)
	, slowVal(500)
	, levelUpVal(1000)
	, levelUpPercent(0.80f)
	, levelEndTime(2) {
	initLevel();
	/*std::vector<Intersection> fill;
	fill.push_back(Intersection(pGame, { 200, 200 }));
	fill.push_back(Intersection(pGame, { 240, 200 }));
	fill.push_back(Intersection(pGame, { 240, 210 }));
	fill.push_back(Intersection(pGame, { 220, 210 }));
	fill.push_back(Intersection(pGame, { 220, 220 }));
	fill.push_back(Intersection(pGame, { 240, 220 }));
	fill.push_back(Intersection(pGame, { 240, 230 }));
	fill.push_back(Intersection(pGame, { 200, 230 }));

	connectIntersections(fill);
	fillBoundary(fill, FillType::Fast);*/
}

Level::~Level() { }

std::vector<Intersection>& Level::getIntersections() { return intersections; }

std::vector<Intersection*>& Level::getQixBoundary() { return qixBoundary; }

int Level::getFillPercent() { return 100.0f * percentFilled; }

int Level::getLevelUpPercent() { return 100.0f * levelUpPercent; }

void Level::initLevel() {
	grid.clear();
	float top = (52 * pGame->getScale());
	float left = (8 * pGame->getScale());
	float side = pGame->getWindow().getSize().x - left * 2;
	levelBounds = sf::FloatRect(left, top, side, side);
	sf::Clock clock;
	for (int col = 0; col < side; col += pGame->getScale()) {
		std::vector<FillType> emptyRow;
		for (int row = 0; row < side; row += pGame->getScale()) {
			emptyRow.push_back(FillType::Empty);
		}
		grid.push_back(emptyRow);
	}
	generateDrawRects();

	std::cout << grid.size() << "x" << grid[0].size() << " = " << grid.size() * grid[0].size() << ": " << clock.getElapsedTime().asSeconds() << "s\n";

	for (int i = 0; i < 4096; i++) {
		intersections.push_back(Intersection(pGame, sf::Vector2f(0, 0)));
		fillIntersections.push_back(Intersection(pGame, sf::Vector2f(0, 0)));
	}
	intersections.clear();
	fillIntersections.clear();
	intersections.push_back(Intersection(pGame, { levelBounds.left, levelBounds.top }));
	intersections.push_back(Intersection(pGame, { levelBounds.left + levelBounds.width, levelBounds.top }));
	intersections.push_back(Intersection(pGame,	{ levelBounds.left + levelBounds.width, levelBounds.top + levelBounds.height }));
	intersections.push_back(Intersection(pGame,	{ levelBounds.left, levelBounds.top + levelBounds.height }));
	//intersections.push_back(Intersection(pGame, { levelBounds.left, levelBounds.top + levelBounds.height }));
	snapToGrid(intersections);

	for (int i = 0; i < intersections.size(); i++) {
		fillIntersections.push_back(intersections[i]);
	}

	connectIntersections(intersections);
	connectIntersections(fillIntersections);
	/*intersections[0].setCW(&intersections[1]);
	intersections[0].setCCW(&intersections[3]);
	intersections[1].setCW(&intersections[2]);
	intersections[1].setCCW(&intersections[0]);
	intersections[2].setCW(&intersections[3]);
	intersections[2].setCCW(&intersections[1]);
	intersections[3].setCW(&intersections[0]);
	intersections[3].setCCW(&intersections[2]);*/

	qixBoundary.clear();
	for (int i = 0; i < intersections.size(); i++)
		qixBoundary.push_back(&intersections[i]);

	sf::Vector2f position((intersections[3].getPos() + intersections[2].getPos()) / 2.0f);
	pGame->getStix().initializeStix(position, &intersections[3], &intersections[2]);

	std::vector<sf::Vector2f> qixPos;
	for (int i = 0; i < (currentLevel / 2) + 2; i++)
		qixPos.push_back(sf::Vector2f((intersections[0].getPos() + intersections[2].getPos()) / 2.0f));

	pGame->getQix().initQix(qixPos);
}

void Level::connectIntersections(std::vector<Intersection> &inters) {
	inters[0].setCW(&inters[1]);
	inters[0].setCCW(&inters[inters.size() - 1]);
	inters[inters.size() - 1].setCW(&inters[0]);
	inters[inters.size() - 1].setCCW(&inters[inters.size() - 2]);
	for (int i = 1; i < inters.size() - 1; i++) {
		inters[i].setCW(&inters[i + 1]);
		inters[i].setCCW(&inters[i - 1]);
	}
}

bool Level::isBetweenHorizontal(sf::Vector2i target, sf::Vector2i a, sf::Vector2i b) {
	if (a.y != target.y || b.y != target.y)
		return false;
	sf::Vector2i left = a;
	sf::Vector2i right = b;
	if (a.x > b.x) {
		left = b;
		right = a;
	}

	if (target.x >= left.x && target.x <= right.x)
		return true;
	return false;
}

bool Level::isBetweenVertical(sf::Vector2i target, sf::Vector2i a, sf::Vector2i b) {
	if (a.x != target.x || b.x != target.x)
		return false;
	sf::Vector2i top = a;
	sf::Vector2i bottom = b;
	if (a.y > b.y) {
		top = b;
		bottom = a;
	}

	if (target.y >= top.y && target.y <= bottom.y)
		return true;
	return false;
}

sf::Vector2i Level::toGrid(sf::Vector2f position) {
	sf::Vector2f gridPos = position - sf::Vector2f(levelBounds.left, levelBounds.top);
	gridPos /= (float)pGame->getScale();
	return sf::Vector2i(gridPos);
}

sf::Vector2f Level::toWindow(sf::Vector2i gridPos) { 
	sf::Vector2f position = sf::Vector2f(gridPos * pGame->getScale());
	position += sf::Vector2f(levelBounds.left, levelBounds.top);
	return position;
}

void Level::snapToGrid(std::vector<Intersection> &boundary) {
	for (int i = 0; i < boundary.size(); i++) {
		std::cout << "before: " << boundary[i].getPos().x << ", " << boundary[i].getPos().y << "\n";
		boundary[i].setPos(toWindow(toGrid(boundary[i].getPos())));
		std::cout << "after: " << boundary[i].getPos().x << ", " << boundary[i].getPos().y << "\n";
	}
}

void Level::replaceIntersections(std::vector<Intersection> &boundary) {
	// add new intersections

	std::cout << intersections.size() << "->";
	for (int i = 0; i < boundary.size(); i++) {
		bool newInter = true;
		for (int j = 0; j < intersections.size(); j++)
			if (boundary[i].getPos() == intersections[j].getPos()) {
				newInter = false;
				break;
			}
		if (newInter)
			intersections.push_back(boundary[i]);
	}
	std::cout << intersections.size() << "\n";

	// set new neighbors for intersections
	qixBoundary.clear();
	for (int i = 0; i < intersections.size(); i++) {
		for (int j = 0; j < boundary.size(); j++) {
			if (boundary[j].getPos() == intersections[i].getPos()) {
				for (int k = 0; k < intersections.size(); k++) {
					if (intersections[k].getPos() == boundary[j].getCW()->getPos())
						intersections[i].setCW(&intersections[k]);
					if (intersections[k].getPos() == boundary[j].getCCW()->getPos())
						intersections[i].setCCW(&intersections[k]);
				}
				// remove redundant intersections
				if (intersections[i].isRedundant()) {
					std::cout << "redundant: " << intersections[i].getPos().x << ", " << intersections[i].getPos().y << "\n";
					intersections[i].bridge();
				}
				else {
					qixBoundary.push_back(&intersections[i]);
				}
			}
		}
	}
	// update player's neighbors
	for (int i = 0; i < intersections.size(); i++) {
		//std::cout << pGame->getStix().getPos().x << ":" << intersections[i].getPos().x << "-" << pGame->getStix().getPos().y << ":" << intersections[i].getPos().y << std::endl;
		if (intersections[i].getPos() == sf::Vector2f(sf::Vector2i(pGame->getStix().getPos()))) {
			std::cout << "match\n";
			pGame->getStix().setCW(intersections[i].getCW());
			pGame->getStix().setCCW(&intersections[i]);
		}
	}

	snapToGrid(intersections);
}

void Level::newStixLine(std::vector<Intersection> cwBoundary, std::vector<Intersection> ccwBoundary, FillType fill) {
	// get smallest boundary by grid size
	int firstCol = grid.size(), lastCol = 0, firstRow = grid[0].size(), lastRow = 0;
	for (int i = 0; i < cwBoundary.size(); i++) {
		if (toGrid(cwBoundary[i].getPos()).x < firstCol)
			firstCol = toGrid(cwBoundary[i].getPos()).x;
		if (toGrid(cwBoundary[i].getPos()).x > lastCol)
			lastCol = toGrid(cwBoundary[i].getPos()).x;
		if (toGrid(cwBoundary[i].getPos()).y < firstRow)
			firstRow = toGrid(cwBoundary[i].getPos()).y;
		if (toGrid(cwBoundary[i].getPos()).y > lastRow)
			lastRow = toGrid(cwBoundary[i].getPos()).y;
	}
	int cwSize = (lastCol - firstCol) * (lastRow - firstRow);

	firstCol = grid.size(), lastCol = 0, firstRow = grid[0].size(), lastRow = 0;
	for (int i = 0; i < ccwBoundary.size(); i++) {
		if (toGrid(ccwBoundary[i].getPos()).x < firstCol)
			firstCol = toGrid(ccwBoundary[i].getPos()).x;
		if (toGrid(ccwBoundary[i].getPos()).x > lastCol)
			lastCol = toGrid(ccwBoundary[i].getPos()).x;
		if (toGrid(ccwBoundary[i].getPos()).y < firstRow)
			firstRow = toGrid(ccwBoundary[i].getPos()).y;
		if (toGrid(ccwBoundary[i].getPos()).y > lastRow)
			lastRow = toGrid(ccwBoundary[i].getPos()).y;
	}
	int ccwSize = (lastCol - firstCol) * (lastRow - firstRow);

	// check if smaller contains qix
	if (cwSize < ccwSize) {
		fillBoundary(cwBoundary, fill);
		bool smallerContainsQix = false;
		for (int i = 0; i < pGame->getQix().getPositions().size(); i++) {
			sf::Vector2f pos = pGame->getQix().getPositions()[i];
			if (grid[toGrid(pos).x][toGrid(pos).y] == fill) { // if this boundary contains qix
				smallerContainsQix = true;
			}
		}

		if (smallerContainsQix) {
			fillBoundary(cwBoundary, FillType::Empty);
			// update intersections with boundary containing qix
			replaceIntersections(cwBoundary);
			// fill other boundary
			fillBoundary(ccwBoundary, fill);
		}
		else {
			replaceIntersections(ccwBoundary);
		}
	}
	else {
		fillBoundary(ccwBoundary, fill);
		bool smallerContainsQix = false;
		for (int i = 0; i < pGame->getQix().getPositions().size(); i++) {
			sf::Vector2f pos = pGame->getQix().getPositions()[i];
			if (grid[toGrid(pos).x][toGrid(pos).y] == fill) { // if this boundary contains qix
				smallerContainsQix = true;
			}
		}

		if (smallerContainsQix) {
			fillBoundary(ccwBoundary, FillType::Empty);
			// update intersections with boundary containing qix
			replaceIntersections(ccwBoundary);
			// fill other boundary
			fillBoundary(cwBoundary, fill);
		}
		else {
			replaceIntersections(cwBoundary);
		}
	}

	generateDrawRects();

	float oldPercentFilled = percentFilled;
	percentFilled = calculateFillPercent();
	float percentDiff = percentFilled - oldPercentFilled;

	std::cout << "percent: " << percentFilled << std::endl;
	if (fill == FillType::Fast) {
		pGame->addScore(100.0f * percentDiff * fastVal);
	}
	if (fill == FillType::Slow) {
		pGame->addScore(100.0f * percentDiff * slowVal);
	}

	if (percentFilled > levelUpPercent) {
		levelUp();
	}

	pGame->restartFrameTimer();
}

void Level::fillBoundary(std::vector<Intersection> boundary, FillType fill) {
	sf::Clock clock;

	int firstCol = grid.size(), lastCol = 0, firstRow = grid[0].size(), lastRow = 0;
	for (int i = 0; i < boundary.size(); i++) {
		if (toGrid(boundary[i].getPos()).x < firstCol)
			firstCol = toGrid(boundary[i].getPos()).x;
		if (toGrid(boundary[i].getPos()).x > lastCol)
			lastCol = toGrid(boundary[i].getPos()).x;
		if (toGrid(boundary[i].getPos()).y < firstRow)
			firstRow = toGrid(boundary[i].getPos()).y;
		if (toGrid(boundary[i].getPos()).y > lastRow)
			lastRow = toGrid(boundary[i].getPos()).y;
	}
	for (int col = firstCol; col < lastCol; col++) {
		std::vector<bool> switchPoints;
		switchPoints.assign(firstRow, false);
		bool inside = false;
		for (int row = firstRow; row < lastRow; row++) {
			bool change = false;
			switchPoints.push_back(change);
			for (int i = 0; i < boundary.size(); i++) {
				if (boundary[i].getHorizontal() != nullptr && boundary[i].getVertical() != nullptr) {
					if (isBetweenHorizontal(sf::Vector2i(col, row), toGrid(boundary[i].getPos()), toGrid(boundary[i].getHorizontal()->getPos()))) {
						if (isBetweenVertical(sf::Vector2i(col, row), toGrid(boundary[i].getPos()), toGrid(boundary[i].getVertical()->getPos())) && inside) { // is intersection && not first
							// if top, no change
							if (toGrid(boundary[i].getPos()).y < toGrid(boundary[i].getVertical()->getPos()).y) {
								change = false;
								break;
							}
							else { // check direction of intersections
								// get direction: isleft = -1, isright = 1
								int direction = 0;
								if (toGrid(boundary[i].getPos()).x < toGrid(boundary[i].getHorizontal()->getPos()).x)
									direction = -1;
								else
									direction = 1;

								int topDirection = 0;
								if (toGrid(boundary[i].getVertical()->getPos()).x < toGrid(boundary[i].getVertical()->getHorizontal()->getPos()).x)
									topDirection = -1;
								else
									topDirection = 1;

								if (topDirection == direction) {
									change = switchPoints[toGrid(boundary[i].getVertical()->getPos()).y];
									break;
								}
								else {
									change = !switchPoints[toGrid(boundary[i].getVertical()->getPos()).y];
									break;
								}
							}
						}
						else {
							change = true;
						}
					}
				}
			}
			if (change) {
				inside = !inside;
			}
			switchPoints[switchPoints.size() - 1] = change;
		}
		inside = false;
		for (int row = firstRow; row < lastRow; row++) {
			if (inside)
				grid[col][row] = fill;
			if (switchPoints[row]) {
				if (inside) {
					inside = false;
					//std::cout << "out: " << col << ", " << row << std::endl;
				}
				else {
					inside = true;
					grid[col][row] = fill;
					//std::cout << "in: " << col << ", " << row << std::endl;
				}
			}
		}
	}

	for (int i = 0; i < boundary.size(); i++) {
		fillIntersections.push_back(boundary[i]);
	}

	for (int i = 0; i < boundary.size() - 1; i++) {
		int index = (fillIntersections.size() - boundary.size()) + i;
		fillIntersections[index].setCW(&fillIntersections[index + 1]);
	}
	fillIntersections[fillIntersections.size() - 1].setCW(&fillIntersections[fillIntersections.size() - boundary.size()]);

	snapToGrid(fillIntersections);

	std::cout << grid.size() << "x" << grid[0].size() << " = " << grid.size() * grid[0].size() << ": " << clock.getElapsedTime().asSeconds() << "s\n";
}

float Level::calculateFillPercent() {
	int total = grid.size() * grid[0].size();
	int filled = 0;
	for (int col = 0; col < grid.size(); col++)
		for (int row = 0; row < grid[col].size(); row++)
			if (grid[col][row] != FillType::Empty)
				filled++;

	std::cout << filled << "/" << total << std::endl;
	return (float)filled / (float)total;
}

void Level::levelUp() {
	float percentDiff = percentFilled - levelUpPercent;
	pGame->addScore(100.0f * percentDiff * levelUpVal);
	currentLevel++;
	initLevel();
}

void Level::restart() {
	currentLevel = 0;
	initLevel();
}

void Level::generateDrawRects() {
	gridRects.clear();
	for (int col = 0; col < grid.size(); col++) {
		FillType fill = FillType::Empty;
		sf::RectangleShape column(sf::Vector2f(pGame->getScale(), pGame->getScale()));
		column.setOrigin(column.getSize().x / 2, column.getSize().y / 2);
		for (int row = 0; row < grid[col].size(); row++) {
			if (fill != FillType::Empty && grid[col][row] == fill) { // add to existing rect
				column.setSize({ column.getSize().x, column.getSize().y + pGame->getScale() });
			}
			else if (grid[col][row] != fill) { // draw column start new one
				if (fill == FillType::Fast) {
					column.setFillColor(sf::Color::Cyan);
					gridRects.push_back(column);
				}
				else if (fill == FillType::Slow) {
					column.setFillColor(sf::Color::Red);
					gridRects.push_back(column);
				}
				fill = grid[col][row];
				column.setSize(sf::Vector2f(pGame->getScale(), pGame->getScale()));
				column.setOrigin(column.getSize().x / 2, column.getSize().y / 2);
				column.setPosition(toWindow({ col, row }));
			}
			if (row == grid[col].size() - 1) { // finish drawing last column			
				if (fill == FillType::Fast) {
					column.setFillColor(sf::Color::Cyan);
					gridRects.push_back(column);
				}
				else if (fill == FillType::Slow) {
					column.setFillColor(sf::Color::Red);
					gridRects.push_back(column);
				}
			}
			/*if (grid[col][row] == FillType::Fast) {
			dot.setPosition(toWindow(sf::Vector2i(col, row)) + sf::Vector2f(pGame->getScale() / 2, pGame->getScale() / 2));
			dot.setFillColor(sf::Color::Cyan);
			pGame->getWindow().draw(dot);
			}
			else if (grid[col][row] == FillType::Slow) {
			dot.setPosition(toWindow(sf::Vector2i(col, row)) + sf::Vector2f(pGame->getScale() / 2, pGame->getScale() / 2));
			dot.setFillColor(sf::Color::Red);
			pGame->getWindow().draw(dot);
			}*/
		}
	}
}

void Level::draw() {
	//sf::RectangleShape dot(sf::Vector2f(pGame->getScale(), pGame->getScale()));
	//dot.setOrigin(dot.getSize().x / 2, dot.getSize().y / 2);
	for (int col = 0; col < grid.size(); col++) {
		for (int row = 0; row < grid[col].size(); row++) {
		}
	}

	for (int i = 0; i < gridRects.size(); i++) {
		pGame->getWindow().draw(gridRects[i]);
	}
	
	for (int i = 0; i < fillIntersections.size(); i++) {
		fillIntersections[i].draw(sf::Color::White);
	}
}