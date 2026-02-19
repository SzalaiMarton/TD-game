#include "stdafx.hpp"

Target::Target(TargetType type) : BaseShape(0.f, 0.f, 0.f, 0.f, Assets::getTexture("default_texture.png")) {
	this->stats = createTargetStats(type);
	this->getSC()->setSize(this->stats->size);

	auto s = this->getSC()->sprite->getLocalBounds().size;
	this->getSC()->sprite->setOrigin({ s.x / 2, s.y / 2 });
}

Target::~Target() {
	delete this->stats;
}

void Target::onUpdate() {
	if (this->isSpawned) {
		if (this->stats->health <= 0) {
			this->die();
		}
		this->move();
		if (this->path.size() == 0) {
			this->onDeath();
			return;
		}
		this->updateTree();
	}
}

void Target::onDeath() {
	Renderer::getLayer(MainLayerName::INGAME)->removeShape(this);
	this->detachFromEveryTree();
	Game::toBeDeletedShapes.push(this);
}

bool Target::die() {
	auto res = createTargetStats(this->stats->childType);
	if (res) {
		this->stats = res;
		return false;
	}
	else {
		this->onDeath();
		return true;
	}
}

void Target::takeDmg(uint16_t dmg) {
	this->stats->health -= dmg;
	if (this->stats->health <= 0) {
		this->die();
	}
}

void Target::retrievePath() {
	this->path = Game::currentMap->path;
}

void Target::draw(sf::RenderWindow* window) {
	if (this->isSpawned) {
		window->draw(*this->getSC()->sprite);
	}
}

void Target::move() {
	if (this->path.empty()) {
		this->isSpawned = false;
		return;
	}

	auto self = this->getSC()->sprite;
	auto& targetPos = this->path.front();
	sf::Vector2f pos = self->getPosition();
	sf::Vector2f direction = targetPos - pos;
	float length = (float)std::sqrt(pow(direction.x, 2) + pow(direction.y, 2));

	if (length > 1.f) {
		direction /= length;
		self->move({ direction.x * this->stats->speed, direction.y * this->stats->speed });
	}
	if (pos.x == targetPos.x || pos.y == targetPos.y || length < (float)this->stats->speed) {
		this->path.pop();
	}
}

void Target::spawn(Layer* layer) {
	this->isSpawned = true;
	this->retrievePath();
	this->getSC()->sprite->setPosition({ this->path.front().x, this->path.front().y});
	layer->addShape(this);
}

constexpr uint16_t getPriceByType(TargetType type) {
	switch (type) {
	case TargetType::BASIC:
		return 1;
	default:
		return 0;
	}
}

constexpr uint8_t getAmountByType(TargetType type) {
	switch (type) {
	case TargetType::BASIC:
		return 10;
	default:
		return 0;
	}
}

TargetStats* createTargetStats(TargetType type) {
	switch (type) {
	case TargetType::BASIC:
		return createPlaceHolder();
	default:
		return nullptr;
	}
}

TargetStats* createPlaceHolder() {
	return new TargetStats(10, 2, { 30.f, 30.f }, TargetType::NONE);
}

TargetGroup::TargetGroup(uint8_t amount, TargetType type) {
	for (uint8_t i{}; i < amount; i++) {
		this->targets.push_back(new Target(type));
	}
}

void TargetGroup::spawnNext(Layer* layer) {
	this->targets.back()->spawn(layer);
	this->targets.pop_back();
}

std::pair<uint8_t, TargetGroup*> TargetGroup::initSpawn() {
	// returns the wait time and current group
	return { 10, this };
}
