#include "stdafx.hpp"

sf::Font* Button::buttonFont = nullptr;
sf::Font* InvetoryCard::cardFont = nullptr;
unsigned QuadTree::smallestSize = 50;

BaseShape::BaseShape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) {
	this->initClass(xPos, yPos, xSize, ySize, texture);
}

BaseShape::BaseShape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture) {
	this->initClass(pos.x, pos.y, size.x, size.y, texture);
}

void BaseShape::showShape() {
	this->isHidden = false;
}

void BaseShape::hideShape() {
	this->isHidden = true;
}

void BaseShape::detachFromTree() {
	this->treeNode = nullptr;
}

void BaseShape::initClass(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) {
	this->attributes = *(AttributeBuilder().withTexture(texture).build());

	this->attributes.getComponent<SpriteComponent>()->setPos(xPos, yPos);
	this->attributes.getComponent<SpriteComponent>()->setSize(xSize, ySize);
}

Button::Button(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture, const std::string& text = "")
	: BaseShape(xPos, yPos, xSize, ySize, texture) {
	this->initClass(text);
}

Button::Button(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, const std::string& text = "")
	: BaseShape(pos, size, texture) {
	this->initClass(text);
}

void Button::addClickHanlder(std::function<void()> handler) {
	if (handler) {
		this->attributes.getComponent<MIC>()->handlers.insert({HandlerType::CLICKHANDLER, handler});
	}
}

void Button::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
	window->draw(*this->text);
}

void Button::initClass(const std::string& text) {
	if (!buttonFont) {
		Assets::loadFonts(Assets::fontsPath);
		buttonFont = Assets::getFont("ManlineSlabs_t.ttf");
	}

	try {
		this->text = new sf::Text(*buttonFont);
		this->text->setString(text);

		this->text->setOrigin(centerTo(*this->text));
		this->text->setPosition(centerTo(*this->attributes.getComponent<SpriteComponent>()->sprite));
	}
	catch (std::exception& e) {
		ERROR("Button::Button", e.what());
	}

	std::initializer_list<MIC::SingleHandler> handlers = {
		MIC::SingleHandler(HandlerType::HOVERHANDLER, [this]() { this->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255,255,255,128)); }),
		MIC::SingleHandler(HandlerType::HOVERLOSSHANDLER, [this]() { this->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255,255,255,255)); })
	};

	this->attributes.addComponent<MIC>(new MIC(handlers));
}

void Shape::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
}

QuadTree::QuadTree(float x, float y, unsigned xs, unsigned ys, uint8_t c) : xPos(x), yPos(y), xSize(xs), ySize(ys), cap(c) {}

QuadTree::~QuadTree() {
	this->elements.clear();
	delete this->ne;
	delete this->nw;
	delete this->se;
	delete this->sw;
	this->se = nullptr;
	this->ne = nullptr;
	this->sw = nullptr;
	this->nw = nullptr;
}

void QuadTree::reinsert(BaseShape* obj = nullptr) {
	auto holder = this->elements;
	this->disabled = true;
	this->elements.clear();

	for (auto& e : holder) {
		this->insert(e);
	}
	if (obj) {
		this->insert(obj);
	}
}

void QuadTree::insert(BaseShape* e) {
	if (!e) {
		return;
	}
	auto ePos = e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().position;
	auto eBounds = ePos + e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size;
	if (this->cap <= this->elements.size() && !this->hasBeenSplit) {
		if (this->split()) {
			this->reinsert(e);
		}
	}
	else if ((this->hasBeenSplit || this->disabled) && !this->cannotBeSplit) {
		this->tryChild(ePos, eBounds, e);
	}
	else if (!this->disabled) {
		this->elements.insert(e);
		e->treeNode = this;
	}
}

bool QuadTree::containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	return ((this->xPos >= pos.x && this->xPos + this->xSize <= bound.x && this->yPos < pos.y && this->yPos + this->ySize >= pos.y)
		|| (this->yPos >= pos.y && this->yPos + this->ySize <= bound.y && this->xPos < pos.x && this->xPos + this->xSize >= pos.x)
		|| (this->xPos >= pos.x && this->xPos + this->xSize <= bound.x && this->yPos >= pos.y && this->yPos + this->ySize <= bound.y));
}

bool QuadTree::containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const {
	return ((this->xPos > xPos || this->yPos > yPos) && (this->xPos + this->xSize < xBound || this->yPos + this->ySize < yBound));
}

bool QuadTree::contains(const sf::Vector2f& pos) const {
	return ((this->xPos < pos.x && this->yPos < pos.y) && (this->xPos + this->xSize >= pos.x && this->yPos + this->ySize >= pos.y));
}

bool QuadTree::contains(float xPos, float yPos) const {
	return ((this->xPos < xPos && this->yPos < yPos) && (this->xPos + this->xSize >= xPos && this->yPos + this->ySize >= yPos));
}

bool QuadTree::split() {
	this->hasBeenSplit = true;
	if (this->xSize <= smallestSize) {
		this->cannotBeSplit = true;
		return false;
	}

	if (!this->nw) {
		this->nw = new QuadTree(this->xPos, this->yPos, this->xSize / 2, this->ySize / 2, cap);
	}
	if (!this->ne) {
		this->ne = new QuadTree(this->xPos + this->xSize / 2, this->yPos, this->xSize / 2, this->ySize / 2, cap);
	}
	if (!this->sw) {
		this->sw = new QuadTree(this->xPos, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2, cap);
	}
	if (!this->se) {
		this->se = new QuadTree(this->xPos + this->xSize / 2, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2, cap);
	}

	return true;
}

void QuadTree::tryChild(const sf::Vector2f& ePos, const sf::Vector2f& eBounds, BaseShape* e) {
	if (this->ne->contains(ePos)) {
		this->ne->insert(e);
	}
	if (this->nw->contains(ePos)) {
		this->nw->insert(e);
	}
	if (this->se->contains(ePos)) {
		this->se->insert(e);
	}
	if (this->sw->contains(ePos)) {
		this->sw->insert(e);
	}

	if (this->ne->contains(eBounds)) {
		this->ne->insert(e);
	}
	if (this->nw->contains(eBounds)) {
		this->nw->insert(e);
	}
	if (this->se->contains(eBounds)) {
		this->se->insert(e);
	}
	if (this->sw->contains(eBounds)) {
		this->sw->insert(e);
	}

	if (this->ne->containsCaseOverlap(ePos, eBounds)) {
		this->ne->insert(e);
	}
	if (this->nw->containsCaseOverlap(ePos, eBounds)) {
		this->nw->insert(e);
	}
	if (this->se->containsCaseOverlap(ePos, eBounds)) {
		this->se->insert(e);
	}
	if (this->sw->containsCaseOverlap(ePos, eBounds)) {
		this->sw->insert(e);
	}
}

BaseShape* QuadTree::getClicked(const sf::Vector2f& mousePos) {
	if (this->ne && this->ne->contains(mousePos)) {
		this->ne->getClicked(mousePos);
	}
	else if (this->nw && this->nw->contains(mousePos)) {
		this->nw->getClicked(mousePos);
	}
	else if (this->sw && this->sw->contains(mousePos)) {
		this->sw->getClicked(mousePos);
	}
	else if (this->se && this->se->contains(mousePos)) {
		this->se->getClicked(mousePos);
	}
	else {
		for (auto& e : this->elements) {
			if (e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().contains(mousePos) && !e->isHidden && e->attributes.hasComponent<MIC>()) {
				return e;
			}
		}
	}
	return nullptr;
}

InvetoryCard::InvetoryCard(float xPos, float yPos, float xSize, float ySize, CatType catType) 
	: BaseShape(xPos, yPos, xSize, ySize, Assets::getTexture("inventory_card.png")) {
	this->initClass(catType);
}

InvetoryCard::InvetoryCard(const sf::Vector2f& pos, const sf::Vector2f& size, CatType catType)
	: BaseShape(pos, size, Assets::getTexture("inventory_card.png")) {
	this->initClass(catType);
}

void InvetoryCard::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
	if (this->displayCat) {
		window->draw(*this->displayCat);
	}
	if (this->displayCatDescription) {
		window->draw(*this->displayCatDescription);
	}
	if (this->displayCatName) {
		window->draw(*this->displayCatName);
	}
}

void InvetoryCard::initClass(CatType catType) {
	auto dC = Assets::getTexture(catTypeToString(catType));
	this->displayCat = dC ? new sf::Sprite(*dC) : nullptr;

	try {
		auto cardSprite = this->attributes.getComponent<SpriteComponent>()->sprite;

		InvetoryCard::cardFont = Assets::getFont("ManlineSlabs_t.ttf");
		this->displayCatName = new sf::Text(*InvetoryCard::cardFont);
		this->displayCatName->setString(catTypeToString(catType));
		this->displayCatName->setOrigin(centerTo(*this->displayCatName));
		this->displayCatName->setPosition({ cardSprite->getLocalBounds().size.x / 3, cardSprite->getLocalBounds().size.y / 4 + 12 });

		this->displayCatDescription = new sf::Text(*InvetoryCard::cardFont);
	}
	catch (std::exception& e) {
		ERROR("InvetoryCard::InvetoryCard", e.what());
	}

	this->attributes.getComponent<SpriteComponent>()->sprite->setOrigin({ 0.f, this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size.y / 2 });
}
