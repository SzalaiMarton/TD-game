#include "stdafx.hpp"


BlackGreyCat::BlackGreyCat() : BaseCat(0, 0, 64, 64, Assets::getTexture("black_grey_10x.png")) {
	this->initClass();
}

BlackGreyCat::BlackGreyCat(float xPos, float yPos, float xSize, float ySize) : BaseCat(xPos, yPos, xSize, ySize, Assets::getTexture("black_grey.png")) {
	this->initClass();
}

void BlackGreyCat::initClass() {
	this->stats.firerate = 300;
	this->stats.range = 150;
	this->stats.cost = 400;
	this->cooldown = this->stats.firerate;
	this->range = new sf::CircleShape(this->stats.range);
	this->range->setFillColor(sf::Color(0,0,0,128));
	this->range->setOrigin(this->range->getGeometricCenter());
	this->range->setPosition(this->attributes.getComponent<SpriteComponent>()->sprite->getPosition());
	this->attributes.getComponent<MIC>()->disableClick();
	this->attributes.getComponent<MIC>()->disableHover();
}

bool BlackGreyCat::onUpdate() {
	size_t i{};
	auto& shapes = Renderer::getCurrentLayer()->shapes;
	auto res = false;
	while (i < shapes.size()) {
		auto obj = dynamic_cast<Target*>(shapes[i]);
		if (!obj) {
			i += 1;
			if (this->cooldown < this->stats.firerate) {
				this->cooldown += 1;
			}
			continue;
		}

		if (this->cooldown >= this->stats.firerate && this->inRange(obj)) { // inRange checks for nullptr
			this->cooldown = 0;
			this->rotate(obj);
			this->fire(obj);
			res = true;
			continue;
		}

		if (this->cooldown < this->stats.firerate) {
			this->cooldown += 1;
		}
		i += 1;
	}
	return res;
}

void BlackGreyCat::fire(Target* target) {
	auto pos = this->attributes.getComponent<SpriteComponent>()->getPos();
	auto direction = target->attributes.getComponent<SpriteComponent>()->getPos() - pos;
	direction /= (float)sqrt(pow(direction.x, 2) + pow(direction.y, 2));

	this->bullets.push_back(new BaseBullet(pos.x, pos.y, 20, 20, direction ,BulletType::BASIC, this));
}

void BaseCat::onDrag() {
	this->range->setPosition(this->attributes.getComponent<SpriteComponent>()->sprite->getPosition());
}

void BaseCat::onClick() {
	this->showRange();
}

void BaseCat::onClickLoss() {
	this->hideRange();
}

bool BaseCat::inRange(Target* target) {
	if (!target) {
		return false;
	}

	auto d = target->attributes.getComponent<SpriteComponent>()->sprite->getPosition() - this->attributes.getComponent<SpriteComponent>()->sprite->getPosition();
	auto distance = sqrt(pow(d.x, 2) + pow(d.y, 2));
	return distance < this->stats.range;
}

void BaseCat::deleteBullet(BaseBullet* bullet) {
	auto it = std::find(this->bullets.begin(), this->bullets.end(), bullet);
	if (it != this->bullets.end()) {
		this->bullets.erase(it);
	}
}

void BaseCat::rotate(Target* target) {
	if (!target) {
		return;
	}

	auto pos = this->attributes.getComponent<SpriteComponent>()->getPos();
	auto targetPos = target->attributes.getComponent<SpriteComponent>()->getPos();
	auto direction = targetPos - pos;
	this->attributes.getComponent<SpriteComponent>()->sprite->setRotation(sf::degrees(atan2(direction.y, direction.x) * (180 / pi) - 90.f));
}

void BaseCat::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
	if (this->range && !this->rangeHidden) {
		window->draw(*this->range);
	}
}

void BaseCat::initClass() {
	std::initializer_list<MIC::SingleHandler> handlers = {
		MIC::SingleHandler(HandlerType::CLICKHANDLER, [this]() { this->onClick(); }),
		MIC::SingleHandler(HandlerType::DRAGHANDLER, [this]() { this->onDrag(); }),
		MIC::SingleHandler(HandlerType::CLICKLOSSHANDLER, [this]() { this->onClickLoss(); })
	};

	this->attributes.addComponent<MIC>(new MIC(handlers));
	this->attributes.getComponent<SpriteComponent>()->sprite->setOrigin({ this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size.x / 2, this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size.y / 2 });
}

void BaseCat::showRange() {
	this->rangeHidden = false;
}

void BaseCat::hideRange() {
	this->rangeHidden = true;
}

BaseCat::BaseCat() : BaseShape(0, 0, 64, 64, Assets::getTexture("default_texture.png")) {
	this->initClass();
}

BaseCat::BaseCat(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) : BaseShape(xPos, yPos, xSize, ySize, texture) {
	this->initClass();
}
