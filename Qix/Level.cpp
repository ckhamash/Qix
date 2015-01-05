#include "Level.h"
#include "Game.h"

Level::Level() { }

Level::Level(Game* pGame) : pGame(pGame) {
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
	sf::Vector2f position((intersections[3].getPos() + intersections[2].getPos()) / 2.0f);
	pGame->getStix().initializeStix(position, &intersections[3], &intersections[2]);
}

Level::~Level() { }

std::vector<Intersection>& Level::getIntersections() { return intersections; }

void Level::initLevel() {
	float top = (20 * pGame->getScale());
	float left = (8 * pGame->getScale());
	float side = pGame->getWindow().getSize().x - left * 2;
	levelBounds = sf::FloatRect(left, top, side, side);

	for (int col = 0; col < side; col += pGame->getScale()) {
		grid.push_back(std::vector<FillType>());
		for (int row = 0; row < side; row += pGame->getScale()) {
			grid[col / pGame->getScale()].push_back(FillType::Empty);
		}
	}

	intersections.clear();
	intersections.push_back(Intersection(pGame, { levelBounds.left, levelBounds.top }));
	intersections.push_back(Intersection(pGame, { levelBounds.left + levelBounds.width, levelBounds.top }));
	intersections.push_back(Intersection(pGame, { levelBounds.left + levelBounds.width, levelBounds.top + levelBounds.height }));
	intersections.push_back(Intersection(pGame, { levelBounds.left + levelBounds.width / 2, levelBounds.top + levelBounds.height }));
	intersections.push_back(Intersection(pGame, { levelBounds.left + levelBounds.width / 2, levelBounds.top + levelBounds.height / 2 }));
	intersections.push_back(Intersection(pGame, { levelBounds.left, levelBounds.top + levelBounds.height / 2 }));
	//intersections.push_back(Intersection(pGame, { levelBounds.left, levelBounds.top + levelBounds.height }));

	connectIntersections(intersections);
	/*intersections[0].setCW(&intersections[1]);
	intersections[0].setCCW(&intersections[3]);
	intersections[1].setCW(&intersections[2]);
	intersections[1].setCCW(&intersections[0]);
	intersections[2].setCW(&intersections[3]);
	intersections[2].setCCW(&intersections[1]);
	intersections[3].setCW(&intersections[0]);
	intersections[3].setCCW(&intersections[2]);*/

	qixBounds.clear();
	qixBounds.push_back(&intersections[0]);
	qixBounds.push_back(&intersections[1]);
	qixBounds.push_back(&intersections[2]);
	qixBounds.push_back(&intersections[3]);
	qixBounds.push_back(&intersections[4]);
	qixBounds.push_back(&intersections[5]);


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

void Level::fillBoundary(std::vector<Intersection*> boundary, FillType fill) {
	for (int col = 0; col < grid.size(); col++) {
		std::vector<bool> switchPoints;
		bool inside = false;
		for (int row = 0; row < grid[col].size(); row++) {
			bool change = false;
			for (int i = 0; i < boundary.size(); i++) {
				if (isBetweenHorizontal(sf::Vector2i(col, row), toGrid(boundary[i]->getPos()), toGrid(boundary[i]->getHorizontal()->getPos()))) {
					if (isBetweenVertical(sf::Vector2i(col, row), toGrid(boundary[i]->getPos()), toGrid(boundary[i]->getVertical()->getPos())) && inside) { // is intersection && not first
						// if top, no change
						if (toGrid(boundary[i]->getPos()).y < toGrid(boundary[i]->getVertical()->getPos()).y) {
							change = false;
							break;
						}
						else { // check direction of intersections
							// get direction: isleft = -1, isright = 1
							int direction = 0;
							if (toGrid(boundary[i]->getPos()).x < toGrid(boundary[i]->getHorizontal()->getPos()).x)
								direction = -1;
							else
								direction = 1;

							int topDirection = 0;
							if (toGrid(boundary[i]->getVertical()->getPos()).x < toGrid(boundary[i]->getVertical()->getHorizontal()->getPos()).x)
								topDirection = -1;
							else
								topDirection = 1;

							if (topDirection == direction) {
								change = switchPoints[toGrid(boundary[i]->getVertical()->getPos()).y];
								break;
							}
							else {
								change = !switchPoints[toGrid(boundary[i]->getVertical()->getPos()).y];
								break;
							}
						}
					}
					else {
						change = true;
					}
				}
			}
			if (change) {
				inside = !inside;
			}
			switchPoints.push_back(change);
		}
		inside = false;
		for (int row = 0; row < switchPoints.size(); row++) {
			if (inside)
				grid[col][row] = fill;
			if (switchPoints[row]) {
				if (inside) {
					inside = false;
					std::cout << "out: " << col << ", " << row << std::endl;
				}
				else {
					inside = true;
					grid[col][row] = fill;
					std::cout << "in: " << col << ", " << row << std::endl;
				}
			}
		}
	}
}

void Level::draw() {
	sf::RectangleShape dot(sf::Vector2f(pGame->getScale(), pGame->getScale()));
	dot.setOrigin(dot.getSize().x / 2, dot.getSize().y / 2);
	for (int col = 0; col < grid.size(); col++) {
		for (int row = 0; row < grid[col].size(); row++) {
			if (grid[col][row] == FillType::Fast) {
				dot.setPosition(toWindow(sf::Vector2i(col, row)) + sf::Vector2f(pGame->getScale() / 2, pGame->getScale() / 2));
				dot.setFillColor(sf::Color::Cyan);
				pGame->getWindow().draw(dot);
			}
			else if (grid[col][row] == FillType::Slow) {
				dot.setPosition(toWindow(sf::Vector2i(col, row)) + sf::Vector2f(pGame->getScale() / 2, pGame->getScale() / 2));
				dot.setFillColor(sf::Color::Red);
				pGame->getWindow().draw(dot);
			}
		}
	}
	for (int i = 0; i < intersections.size(); i++) {
		intersections[i].draw(sf::Color::White);
	}
}