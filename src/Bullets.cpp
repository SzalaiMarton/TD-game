#include "stdafx.hpp"

BaseBullet::BaseBullet(float xPos, float yPos, float xSize, float ySize, const sf::Vector2f& directionVector, BulletType type, BaseCat* parent)
	: BaseShape(xPos, yPos, xSize, ySize, Assets::getTexture("default_texture.png")) {
	auto package = createBullet(type);
	this->attributes.getComponent<SpriteComponent>()->sprite->setTexture(*package.first);
	this->stats = package.second;
	this->stats->directionVector = directionVector;
	this->parent = parent;
	Renderer::getCurrentLayer()->addShape(this);
}

BaseBullet::~BaseBullet() {
	delete this->stats;
}

bool BaseBullet::onUpdate() {
	this->move();
	if (this->isOutOfScreen()) {
		this->~BaseBullet();
		return true;
	}
	if (this->checkCollision()) {
		LOG("collision");
		return true;
	}
	this->updateTree();
	return false;
}

void BaseBullet::onCollision(Target* target) {
	target->takeDmg(this->stats->dmg);
	this->parent->deleteBullet(this);
	this->~BaseBullet();
}

bool BaseBullet::checkCollision() {
	LOG(this->treeNodes.size());
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

	auto pos = this->attributes.getComponent<SpriteComponent>()->getPos();
	auto bound = this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size;
	auto targetPos = target->attributes.getComponent<SpriteComponent>()->getPos();
	auto targetBound = target->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size;

	return !(pos.x > targetBound.x || pos.y > targetBound.y || bound.x < targetPos.x || bound.y < targetPos.y);
}

void BaseBullet::move() {
	this->attributes.getComponent<SpriteComponent>()->sprite->move(this->stats->directionVector * (float)this->stats->speed);
}

void BaseBullet::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
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
