#include "stdafx.hpp"

BaseBullet::BaseBullet(float xPos, float yPos, float xSize, float ySize, const sf::Vector2f& directionVector, BulletType type, BaseCat* parent)
	: BaseShape(xPos, yPos, xSize, ySize, Assets::getTexture("default_texture.png")) {
	auto package = createBullet(type);
	this->getSC()->sprite->setTexture(*package.first);
	this->stats = package.second;
	this->stats->directionVector = directionVector;
	this->parent = parent;
}

BaseBullet::~BaseBullet() {
	delete this->stats;
}

void BaseBullet::onUpdate() {
	this->move();
	this->updateTree();
	if (this->isOutOfScreen() || (this->checkCollision() && this->stats->penetration == 0)) {
		this->onDeath();
	}
}

void BaseBullet::onDeath() {
	Game::toBeDeletedShapes.push(this);
}

void BaseBullet::onCollision(Target* target) {
	target->takeDmg(this->stats->dmg);
	this->stats->penetration = this->stats->penetration > 0 ? this->stats->penetration - 1 : 0;
}

bool BaseBullet::checkCollision() {
	for (auto& e : this->treeNodes) {
		for (auto& obj : e->elements) {
			if (this->isColliding(dynamic_cast<Target*>(obj))) {
				this->onCollision(dynamic_cast<Target*>(obj));
				return true;
			}
		}
	}
	return false;
}

bool BaseBullet::isColliding(Target* target) {
	if (!target) {
		return false;
	}

	auto pos = this->getSC()->getPos();
	auto bound = pos + this->getSC()->sprite->getGlobalBounds().size;
	auto targetPos = target->getSC()->getPos();
	auto targetBound = targetPos + target->getSC()->sprite->getGlobalBounds().size;

	return !(pos.x > targetBound.x || pos.y > targetBound.y || bound.x < targetPos.x || bound.y < targetPos.y);
}

void BaseBullet::move() {
	this->getSC()->sprite->move(this->stats->directionVector * (float)this->stats->speed);
}

void BaseBullet::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
}

std::pair<sf::Texture*, BulletStat*> createBullet(BulletType type) {
	switch (type) {
	case BulletType::BASIC:
		return createBasic();
	}
	return createBasic();
}

std::pair<sf::Texture*, BulletStat*> createBasic() {
	return {Assets::getTexture("default_texture.png"), new BulletStat(10, 10, 2)};
}
