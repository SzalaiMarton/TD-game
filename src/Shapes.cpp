#include "stdafx.hpp"

sf::Font* Button::buttonFont = nullptr;
sf::Font* InvetoryCard::cardFont = nullptr;
unsigned QuadTree::smallestSize = 200;

BaseShape::BaseShape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) {
	this->initClass(xPos, yPos, xSize, ySize, texture);
}

BaseShape::BaseShape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture) {
	this->initClass(pos.x, pos.y, size.x, size.y, texture);
}

BaseShape::~BaseShape() {
	this->parentLayer->removeShape(this);
	this->parentLayer = nullptr;
	this->detachFromEveryTree();
}

bool BaseShape::isOutOfScreen() {
	auto pos = this->attributes.getComponent<SpriteComponent>()->getPos();
	auto bound = this->attributes.getComponent<SpriteComponent>()->getBound();
	auto screenTopLeft = (sf::Vector2f)Renderer::getWindow()->getPosition();
	auto screenBotRight = (sf::Vector2f)Renderer::getWindow()->getSize() + screenTopLeft;
	return (bound.x < screenTopLeft.x || pos.x > screenBotRight.x || bound.y < screenTopLeft.y || pos.y > screenBotRight.y);
}

void BaseShape::showShape() {
	this->isHidden = false;
}

void BaseShape::hideShape() {
	this->isHidden = true;
}

void BaseShape::detachFromEveryTree() {
	for (auto it = this->treeNodes.begin(); it != this->treeNodes.end();) {
		it = this->detachFromTree(it);
	}
}

std::set<QuadTree*>::iterator BaseShape::detachFromTree(std::set<QuadTree*>::iterator it) {
	(*it)->removeShape(this);
	return this->treeNodes.erase(it);
}

void BaseShape::initClass(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) {
	this->attributes = *(AttributeBuilder().withTexture(texture).build());

	this->attributes.getComponent<SpriteComponent>()->setPos(xPos, yPos);
	this->attributes.getComponent<SpriteComponent>()->setSize(xSize, ySize);
}

void BaseShape::updateTree() {
	for (auto it = this->treeNodes.begin(); it != this->treeNodes.end();) {
		if (!(*it)->contains(this->attributes.getComponent<SpriteComponent>()->getPos(), this->attributes.getComponent<SpriteComponent>()->getBound())) {
			it = this->detachFromTree(it);
			Renderer::getLayer(gameStateToLayerName(Game::currentState))->root->insert(this);
		}
		else {
			it++;
		}
	}
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
	catch (std::exception e) {
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
	for (auto& e : this->elements) {
		this->insert(e);
	}
	if (obj) {
		this->insert(obj);
	}
	this->elements.clear();
}

void QuadTree::insert(BaseShape* e) {
	if (!e) {
		return;
	}
	

	if (!this || !this->contains(e->attributes.getComponent<SpriteComponent>()->getPos(), e->attributes.getComponent<SpriteComponent>()->getBound())) {
		return;
	}

	if (!this->hasBeenSplit) {
		if (this->elements.size() < this->cap) {
			this->elements.insert(e);
			e->treeNodes.insert(this);
			return;
		}
		this->split();
	}

	this->ne->insert(e);
	this->nw->insert(e);
	this->se->insert(e);
	this->sw->insert(e);
}

bool QuadTree::containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	float left = xPos;
	float right = xPos + xSize;
	float top = yPos;
	float bottom = yPos + ySize;

	return !(right <= pos.x || left >= bound.x || bottom <= pos.y || top >= bound.y);
}

bool QuadTree::containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const {
	float left = xPos;
	float right = xPos + xSize;
	float top = yPos;
	float bottom = yPos + ySize;

	return !(right <= xPos || left >= xBound || bottom <= yPos || top >= yBound);
}

bool QuadTree::containsCaseOnePoint(const sf::Vector2f& pos) const {
	return ((this->xPos < pos.x && this->yPos < pos.y) && (this->xPos + this->xSize >= pos.x && this->yPos + this->ySize >= pos.y));
}

bool QuadTree::containsCaseOnePoint(float xPos, float yPos) const {
	return ((this->xPos < xPos && this->yPos < yPos) && (this->xPos + this->xSize >= xPos && this->yPos + this->ySize >= yPos));
}

bool QuadTree::contains(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	return this->containsCaseOnePoint(pos) || this->containsCaseOnePoint(bound) || this->containsCaseOverlap(pos, bound);
}

bool QuadTree::split() {
	this->hasBeenSplit = true;
	if (this->xSize <= smallestSize || this->ySize <= smallestSize) {
		return false;
	}

	this->nw = new QuadTree(this->xPos, this->yPos, this->xSize / 2, this->ySize / 2, cap);
	this->ne = new QuadTree(this->xPos + this->xSize / 2, this->yPos, this->xSize / 2, this->ySize / 2, cap);
	this->sw = new QuadTree(this->xPos, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2, cap);
	this->se = new QuadTree(this->xPos + this->xSize / 2, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2, cap);

	this->reinsert();

	return true;
}

BaseShape* QuadTree::getClicked(const sf::Vector2f& mousePos) {
	if (this->ne && this->ne->containsCaseOnePoint(mousePos)) {
		return this->ne->getClicked(mousePos);
	}
	if (this->nw && this->nw->containsCaseOnePoint(mousePos)) {
		return this->nw->getClicked(mousePos);
	}
	if (this->sw && this->sw->containsCaseOnePoint(mousePos)) {
		return this->sw->getClicked(mousePos);
	}
	if (this->se && this->se->containsCaseOnePoint(mousePos)) {
		return this->se->getClicked(mousePos);
	}
	for (auto& e : this->elements) {
		if (e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().contains(mousePos) && !e->isHidden && e->attributes.hasComponent<MIC>()) {
			return e;
		}
	}
	return nullptr;
}

QuadTree* QuadTree::getClickedTree(const sf::Vector2f& mousePos) {
	if (this->ne && this->ne->containsCaseOnePoint(mousePos)) {
		return this->ne->getClickedTree(mousePos);
	}
	if (this->nw && this->nw->containsCaseOnePoint(mousePos)) {
		return this->nw->getClickedTree(mousePos);
	}
	if (this->sw && this->sw->containsCaseOnePoint(mousePos)) {
		return this->sw->getClickedTree(mousePos);
	}
	if (this->se && this->se->containsCaseOnePoint(mousePos)) {
		return this->se->getClickedTree(mousePos);
	}
	return this;
}

void QuadTree::draw(Layer* layer) {
	if (!this->s) {
		this->s = new Square(this->xPos, this->yPos, (float)this->xSize, (float)this->ySize);
		layer->addShape(this->s, true);
	}
	if (this->ne) {
		this->ne->draw(layer);
	}
	if (this->sw) {
		this->sw->draw(layer);
	}
	if (this->nw) {
		this->nw->draw(layer);
	}
	if (this->se) {
		this->se->draw(layer);
	}
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
	catch (std::exception e) {
		ERROR("InvetoryCard::InvetoryCard", e.what());
	}

	this->attributes.getComponent<SpriteComponent>()->sprite->setOrigin({ 0.f, this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size.y / 2 });
}

Square::Square(float xPos, float yPos, float xSize, float ySize) : BaseShape() {
	this->s = new sf::RectangleShape({ xSize, ySize });
	this->s->setPosition({xPos, yPos});
	this->s->setFillColor(sf::Color(0,0,0,0));
	this->s->setOutlineThickness(5);
	this->s->setOutlineColor(sf::Color(sf::Color::Red));
}

void Square::draw(sf::RenderWindow* window) {
	if (this->s && this->isVisible) {
		window->draw(*this->s);
	}
}
