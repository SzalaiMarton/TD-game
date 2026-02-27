#include "stdafx.hpp"

void MeleeCat::checkForFire() {
	this->cooldown += 1;
	if (this->cooldown < this->stats->firerate) {
		return;
	}

	auto reservedPen = this->stats->meleePen;

	std::unordered_map<Target*, bool> alreadyhit{};
	
	// not sure why this is here
	//alreadyhit.insert(std::pair<Target*, bool>(dynamic_cast<Target*>(Game::beingDraggedShape), true));
	
	auto& shapes = Game::currentLayer->shapes;
	auto fired = false;
	size_t i{};
	while (i < shapes.size()) {
		auto obj = dynamic_cast<Target*>(shapes[i]);
		this->stats->meleePen = reservedPen;
		if (!obj) {
			i += 1;
			continue;
		}

		if (this->cooldown >= this->stats->firerate && this->inRange(obj) && alreadyhit.find(obj) == alreadyhit.end()) { // inRange checks for nullptr
			while (this->stats->meleePen > 0) {
				this->fire(obj);
			}
			alreadyhit.insert(std::pair<Target*, bool>(obj, true));
			fired = true;
			continue;
		}

		i += 1;
	}

	if (fired) {
		this->cooldown = 0;
	}
}

void MeleeCat::fire(Target* target) {
	target->takeDmg(this->stats->meleeDmg);
	this->stats->meleePen = this->stats->meleePen > 0 ? this->stats->meleePen - 1 : 0;
}

void BaseCat::onUpdate() {
	if (!isDisabled) {
		this->checkForFire();
	}
}

void BaseCat::fire(Target* target) {
	auto pos = this->getSC()->getPos();
	auto direction = target->getSC()->getPos() - pos;
	direction /= (float)sqrt(pow(direction.x, 2) + pow(direction.y, 2));
	this->bullets.push_back(dynamic_cast<BaseBullet*>(Game::currentLayer->addShape(new BaseBullet(pos.x, pos.y, 20, 20, direction, BulletType::BASIC, this))));
}

void BaseCat::onDrag() {
	this->range->setPosition(this->getSC()->sprite->getPosition());
}

void BaseCat::onClick() {
	this->showRange();
}

void BaseCat::onClickLoss() {
	this->hideRange();
}

void RangedCat::checkForFire() {
	this->cooldown += 1;
	if (this->cooldown < this->stats->firerate) {
		return;
	}

	size_t i{};
	auto& shapes = Game::currentLayer->shapes;
	while (i < shapes.size()) {
		auto obj = dynamic_cast<Target*>(shapes[i]);
		if (!obj) {
			i += 1;
			continue;
		}

		if (this->cooldown >= this->stats->firerate && this->inRange(obj)) { // inRange checks for nullptr
			this->cooldown = 0;
			this->rotate(obj);
			this->fire(obj);
			continue;
		}

		i += 1;
	}
}

bool BaseCat::inRange(Target* target) {
	if (!target) {
		return false;
	}

	auto d = target->getSC()->sprite->getPosition() - this->getSC()->sprite->getPosition();
	auto distance = sqrt(pow(d.x, 2) + pow(d.y, 2));
	return distance < this->stats->range;
}

void BaseCat::deleteBullet(BaseBullet* bullet) {
	auto it = std::find(this->bullets.begin(), this->bullets.end(), bullet);
	if (it != this->bullets.end()) {
		this->bullets.erase(it);
	}
}

void BaseCat::enableCat() {
	this->isDisabled = false;
}

void BaseCat::disableCat() {
	this->isDisabled = true;
}

void BaseCat::rotate(Target* target) {
	if (!target) {
		return;
	}

	auto pos = this->getSC()->getPos();
	auto targetPos = target->getSC()->getPos();
	auto direction = targetPos - pos;
	this->getSC()->sprite->setRotation(sf::degrees(atan2(direction.y, direction.x) * (180 / pi) - 90.f));
}

void BaseCat::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
	if (this->range && !this->rangeHidden) {
		window->draw(*this->range);
	}
}

void BaseCat::initClass(CatType type) {
	std::initializer_list<MIC::SingleHandler> handlers = {
		MIC::SingleHandler(HandlerType::CLICKHANDLER, [this]() { this->onClick(); }),
		MIC::SingleHandler(HandlerType::DRAGHANDLER, [this]() { this->onDrag(); }),
		MIC::SingleHandler(HandlerType::CLICKLOSSHANDLER, [this]() { this->onClickLoss(); })
	};

	this->stats = Stats::getStatByType(type);

	this->getSC()->setSize(this->stats->size.xSize, this->stats->size.ySize);
	this->getSC()->sprite->setOrigin({
		this->getSC()->sprite->getLocalBounds().size.x / 2,
		this->getSC()->sprite->getLocalBounds().size.y / 2
		});

	this->cooldown = this->stats->firerate;
	this->range = new sf::CircleShape(2.f);
	this->range->setFillColor(sf::Color(0, 0, 0, 128));
	this->range->setRadius(this->stats->range);
	this->range->setOrigin(this->range->getGeometricCenter());
	this->range->setPosition(this->getSC()->sprite->getPosition());

	this->attributes->addComponent<MIC>(new MIC(handlers));
	this->attributes->getComponent<MIC>()->disableHover();
}

void BaseCat::showRange() {
	this->rangeHidden = false;
}

void BaseCat::hideRange() {
	this->rangeHidden = true;
}

void BaseCat::enablePlace() {
	this->canBePlaced = true;
	this->range->setFillColor(sf::Color(0, 0, 0, 128));
}

void BaseCat::disablePlace() {
	this->canBePlaced = false;
	this->range->setFillColor(sf::Color(255, 0, 0, 128));
}

void BaseCat::place() {
	if (this->canBePlaced) {
		this->updateTree();
		this->enableCat();
		this->hideRange();
	}
	else {
		this->~BaseCat();
	}
}

BaseCat::BaseCat() : BaseShape(0, 0, 64, 64, Assets::getTexture(catTypeToTextureName(CatType::BLACKGREY))) {
	this->initClass(CatType::BLACKGREY);
}

BaseCat::BaseCat(float xPos, float yPos, CatType type) 
	: BaseShape(xPos, yPos, 0, 0, Assets::getTexture(catTypeToTextureName(type))) {
	this->initClass(type);
}

BaseCat::~BaseCat() {
	delete range;
	range = nullptr;
}

void createCat(BaseCat*& bindTo, CatType type, float xPos, float yPos) {
	auto layer = Game::currentLayer;
	switch (type) {
	case CatType::BLACKGREY:
		bindTo = new BlackGreyCat(xPos, yPos);
		break;
	case CatType::ORANGE:
		bindTo = new OrangeCat(xPos, yPos);
		break;
	default:
		bindTo = new BlackGreyCat(xPos, yPos);
		break;
	}
	layer->addShape(bindTo, true);
}
