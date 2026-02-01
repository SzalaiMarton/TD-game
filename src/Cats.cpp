#include "stdafx.hpp"


BlackGreyCat::BlackGreyCat() : BaseCat(0, 0, 64, 64, Assets::getTexture("black_grey_10x.png")) {
	this->initClass();
}

BlackGreyCat::BlackGreyCat(float xPos, float yPos, float xSize, float ySize) : BaseCat(xPos, yPos, xSize, ySize, Assets::getTexture("black_grey.png")) {
	this->initClass();
}

void BlackGreyCat::initClass() {
	this->stats.firerate = 5;
	this->stats.range = 80;
	this->stats.cost = 400;
	this->range = new sf::CircleShape(this->stats.range);
	this->range->setFillColor(sf::Color(0,0,0,128));
	this->range->setOrigin(this->range->getGeometricCenter());
	this->range->setPosition(this->attributes.getComponent<SpriteComponent>()->sprite->getPosition());
	this->attributes.getComponent<MIC>()->disableClick();
	this->attributes.getComponent<MIC>()->disableHover();
}

void BlackGreyCat::onUpdate() {
	for (auto& e : Renderer::getLayer(gameStateToLayerName(Game::currentState))->shapes) {
		if (dynamic_cast<Target*>(e) && this->inRange(e)) {
			fire(e);
		}
	}
}

void BlackGreyCat::fire(BaseShape* shape) {
	
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

bool BaseCat::inRange(BaseShape* shape) {
	auto d = shape->attributes.getComponent<SpriteComponent>()->sprite->getPosition() - this->attributes.getComponent<SpriteComponent>()->sprite->getPosition();
	auto distance = sqrt(pow(d.x, 2) + pow(d.y, 2));
	return distance < this->stats.range;
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
