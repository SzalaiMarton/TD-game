#include "stdafx.hpp"

sf::Font* Button::buttonFont = nullptr;
sf::Font* InvetoryCard::cardFont = nullptr;
unsigned QuadTree::smallestSize = 200;
float InGameCatCard::iconOffset = -15.f;
float InGameCatCard::textOffset = 15.f;
float InGameTargetCard::iconOffset = -15.f;
float InGameTargetCard::textOffset = 15.f;

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
	auto pos = this->getSC()->getPos();
	auto bound = this->getSC()->getPos() + this->getSC()->sprite->getGlobalBounds().size;
	sf::Vector2f screenTopLeft = { 0.f, 0.f };
	auto screenBotRight = (sf::Vector2f)Renderer::getWindow()->getSize();

	return (bound.x < screenTopLeft.x || pos.x > screenBotRight.x || bound.y < screenTopLeft.y || pos.y > screenBotRight.y);
}

SpriteComponent* BaseShape::getSC() {
	return this->attributes->getComponent<SpriteComponent>();
}

MIC* BaseShape::getMIC() {
	return this->attributes->getComponent<MIC>();
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
	this->attributes = AttributeBuilder().withTexture(texture).build();

	auto bound = this->getSC()->sprite->getLocalBounds().size;
	this->getSC()->setPos(xPos, yPos);
	this->getSC()->setSize(xSize, ySize);
}

void BaseShape::updateTree() {
	for (auto it = this->treeNodes.begin(); it != this->treeNodes.end();) {
		if (!(*it)->contains(this->getSC()->getPos(), this->getSC()->getPos() + this->getSC()->sprite->getGlobalBounds().size)) {
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

Button::~Button() {
	delete text;
	text = nullptr;
}

void Button::addClickHanlder(std::function<void()> handler) {
	if (handler) {
		this->getMIC()->handlers.insert({HandlerType::CLICKHANDLER, handler});
	}
}

void Button::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
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

		auto boundText = this->text->getLocalBounds().size;
		auto boundParent = this->getSC()->sprite->getLocalBounds().size;
		auto posParent = this->getSC()->getPos();
		this->text->setOrigin({ boundText.x / 2, boundText.y / 2 });
		this->text->setPosition({ posParent.x - 10 + boundParent.x / 2, posParent.y - 10 + boundParent.y / 2 });
	}
	catch (std::exception e) {
		ERROR("Button::Button", e.what());
	}

	auto pos = this->getSC()->getPos();
	auto bound = this->getSC()->sprite->getGlobalBounds().size;
	this->getSC()->setPos(pos.x - bound.x / 2, pos.y - bound.y / 2);

	std::initializer_list<MIC::SingleHandler> handlers = {
		MIC::SingleHandler(HandlerType::HOVERHANDLER, [this]() { this->getSC()->sprite->setColor(sf::Color(255,255,255,128)); }),
		MIC::SingleHandler(HandlerType::HOVERLOSSHANDLER, [this]() { this->getSC()->sprite->setColor(sf::Color(255,255,255,255)); })
	};

	this->attributes->addComponent<MIC>(new MIC(handlers));
}

void Shape::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
}

QuadTree::QuadTree(float x, float y, unsigned xs, unsigned ys, std::string name) : xPos(x), yPos(y), xSize(xs), ySize(ys), name(name) {
	this->split();
}

QuadTree::~QuadTree() {
	elements.clear();
	delete ne;
	delete nw;
	delete se;
	delete sw;
	se = nullptr;
	ne = nullptr;
	sw = nullptr;
	nw = nullptr;
}

void QuadTree::insert(BaseShape* e) {
	if (!e || 
		!this || 
		!this->contains(e->getSC()->getPos(),
			e->getSC()->getPos() + 
			e->getSC()->sprite->getGlobalBounds().size)
		) {
		return;
	}
	
	if (!this->hasBeenSplit) {
		this->elements.insert(e);
		e->treeNodes.insert(this);
	}

	this->ne->insert(e);
	this->nw->insert(e);
	this->se->insert(e);
	this->sw->insert(e);
}

bool QuadTree::containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	float left = this->xPos;
	float right = this->xPos + this->xSize;
	float top = this->yPos;
	float bottom = this->yPos + this->ySize;

	return !(right <= pos.x || left >= bound.x || bottom <= pos.y || top >= bound.y);
}

bool QuadTree::containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const {
	float left = this->xPos;
	float right = this->xPos + this->xSize;
	float top = this->yPos;
	float bottom = this->yPos + this->ySize;

	return !(right < xPos || left > xBound || bottom < yPos || top > yBound);
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

void QuadTree::split() {
	if (this->xSize <= smallestSize || this->ySize <= smallestSize) {
		return;
	}

	this->hasBeenSplit = true;
	this->nw = new QuadTree(this->xPos, this->yPos, this->xSize / 2, this->ySize / 2, name + "nw");
	this->ne = new QuadTree(this->xPos + this->xSize / 2, this->yPos, this->xSize / 2, this->ySize / 2, name + "ne");
	this->sw = new QuadTree(this->xPos, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2, name + "sw");
	this->se = new QuadTree(this->xPos + this->xSize / 2, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2, name + "se");
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
		if (e->getSC()->sprite->getGlobalBounds().contains(mousePos) && !e->isHidden && e->attributes->hasComponent<MIC>()) {
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

InvetoryCard::~InvetoryCard() {
	delete displayCat;
	displayCat = nullptr;
	delete displayCatDescription;
	displayCatDescription = nullptr;
	delete displayCatName;
	displayCatName = nullptr;
}

void InvetoryCard::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
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

InGameCatCard::InGameCatCard(float xPos, float yPos, float xSize, float ySize, CatType type) : Button(xPos, yPos, xSize, ySize, Assets::getTexture(TextureName::ingameCard), std::to_string(getPriceByType(type))) {
	this->text->setPosition({ xPos, yPos + InGameCatCard::textOffset });
	
	this->icon = new Shape(xPos, yPos, xSize / 2 + 15, ySize / 2, Assets::getTexture(catTypeToTextureName(type)));
	auto iconBound = this->icon->getSC()->sprite->getLocalBounds().size;
	this->icon->getSC()->sprite->setOrigin({ iconBound.x / 2, iconBound.y / 2 });
	this->icon->getSC()->setPos({ xPos, yPos + InGameCatCard::iconOffset });

	this->getMIC()->disableClick();
	this->getMIC()->disableBaseDrag();
	this->getMIC()->bindDragHandler([this, type]() {
			if (!this->draggedCat) {
				createCat(this->draggedCat, type, -100.f, -100.f, 60.f, 40.f);
			}
			Game::isPlacementValid(this->draggedCat);
			this->draggedCat->showRange();
			this->draggedCat->getMIC()->onDrag(sf::Mouse::getPosition(*Renderer::getWindow()), this);
			this->draggedCat->getSC()->setPos((sf::Vector2f)sf::Mouse::getPosition(*Renderer::getWindow()));
		});
	this->getMIC()->bindDragLoss([this]() {
			Game::placingEffectPlayed = false;
			this->draggedCat->place();
			this->draggedCat = nullptr;
		});
}

InGameCatCard::~InGameCatCard() {
	delete icon;
	icon = nullptr;
	draggedCat = nullptr;
}

InGameTargetCard::InGameTargetCard(float xPos, float yPos, float xSize, float ySize, TargetType type) : Button(xPos, yPos, xSize, ySize, Assets::getTexture(TextureName::ingameCard), std::to_string(getPriceByType(type))) {
	this->text->setPosition({ xPos, yPos + InGameTargetCard::textOffset });

	this->icon = new Shape(xPos, yPos, xSize / 2 + 15, ySize / 2, Assets::getTexture(targetTypeToTextureName(type)));
	auto iconBound = this->icon->getSC()->sprite->getLocalBounds().size;
	this->icon->getSC()->sprite->setOrigin({ iconBound.x / 2, iconBound.y / 2 });
	this->icon->getSC()->setPos({ xPos, yPos + InGameTargetCard::iconOffset });

	this->getMIC()->disableBaseDrag();
	this->getMIC()->disableDrag();
	this->getMIC()->bindClickHandler([type]() {
			Game::queueTarget(getAmountByType(type), type);
		});
}

InGameTargetCard::~InGameTargetCard() {
	delete icon;
	icon = nullptr;
}

void InGameTargetCard::hoverHandler(InGameTargetCard* card) {
	if (card) {
		card->getSC()->sprite->setColor(sf::Color(255, 255, 255, 128));
		card->icon->getSC()->sprite->setColor(sf::Color(255, 255, 255, 128));
		card->text->setFillColor(sf::Color(255, 255, 255, 128));
	}
}

void InGameTargetCard::hoverLossHandler(InGameTargetCard* card) {
	if (card) {
		card->getSC()->sprite->setColor(sf::Color(255, 255, 255, 255));
		card->icon->getSC()->sprite->setColor(sf::Color(255, 255, 255, 255));
		card->text->setFillColor(sf::Color(255, 255, 255, 255));
	}
}

void InvetoryCard::initClass(CatType catType) {
	auto dC = Assets::getTexture(catTypeToTextureName(catType));
	this->displayCat = dC ? new sf::Sprite(*dC) : nullptr;

	try {
		auto cardSprite = this->getSC()->sprite;
		
		InvetoryCard::cardFont = Assets::getFont("ManlineSlabs_t.ttf");
		this->displayCatName = new sf::Text(*InvetoryCard::cardFont);
		this->displayCatName->setString(catTypeToString(catType));
		
		auto bound = this->displayCatName->getLocalBounds().size;
		this->displayCatName->setOrigin({ bound.x / 2, bound.y / 2 });
		this->displayCatName->setPosition({ cardSprite->getLocalBounds().size.x / 3, cardSprite->getLocalBounds().size.y / 4 + 12 });
		this->displayCatDescription = new sf::Text(*InvetoryCard::cardFont);
	}
	catch (std::exception e) {
		ERROR("InvetoryCard::InvetoryCard", e.what());
	}

	this->getSC()->sprite->setOrigin({ 0.f, this->getSC()->sprite->getLocalBounds().size.y / 2 });
}

void InGameTargetCard::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
	window->draw(*this->icon->getSC()->sprite);
	window->draw(*this->text);
}

void InGameCatCard::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
	window->draw(*this->icon->getSC()->sprite);
	window->draw(*this->text);
}

void InGameCatCard::hoverHandler(InGameCatCard* card) {
	if (card) {
		card->getSC()->sprite->setColor(sf::Color(255, 255, 255, 128));
		card->icon->getSC()->sprite->setColor(sf::Color(255, 255, 255, 128));
		card->text->setFillColor(sf::Color(255, 255, 255, 128));
	}
}

void InGameCatCard::hoverLossHandler(InGameCatCard* card) {
	if (card) {
		card->getSC()->sprite->setColor(sf::Color(255, 255, 255, 255));
		card->icon->getSC()->sprite->setColor(sf::Color(255, 255, 255, 255));
		card->text->setFillColor(sf::Color(255, 255, 255, 255));
	}
}

Square::Square(float xPos, float yPos, float xSize, float ySize) : BaseShape() {
	this->s = new sf::RectangleShape({ xSize, ySize });
	this->s->setPosition({xPos, yPos});
	this->s->setFillColor(sf::Color(0,0,0,0));
	this->s->setOutlineThickness(5);
	this->s->setOutlineColor(sf::Color(sf::Color::Red));
}

Square::~Square() {
	delete s;
	s = nullptr;
}

void Square::draw(sf::RenderWindow* window) {
	if (this->s && this->isVisible) {
		window->draw(*this->s);
	}
}

InGameCatCardDesk::InGameCatCardDesk(float xPos, float yPos, float xSize, float ySize, Layer* l)
	: Shape(xPos, yPos, xSize, ySize, Assets::getTexture("default_texture.png")) {
	this->cards.reserve(Game::selectedCats.size());
	auto startingPos = this->getSC()->getPos();
	startingPos.x += 80;
	startingPos.y += 50;
	int i = 0;
	for (auto& e : Game::selectedCats) {
		this->cards.push_back(dynamic_cast<InGameCatCard*>(l->addShape(new InGameCatCard(startingPos.x + (100 * i), startingPos.y, 80, 80, e))));
		i += 1;
	}
}

InGameCatCardDesk::~InGameCatCardDesk() {
	for (auto& e : this->cards) {
		delete e;
	}
	this->cards.clear();
}

void InGameCatCardDesk::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
	for (auto& e : this->cards) {
		e->draw(window);
	}
}

InGameTargetCardDesk::InGameTargetCardDesk(float xPos, float yPos, float xSize, float ySize, Layer* l)
	: Shape(xPos, yPos, xSize, ySize, Assets::getTexture("default_texture.png")) {
	this->cards.reserve(Game::availableTargets.size());
	auto startingPos = this->getSC()->getPos();
	startingPos.x += 50;
	startingPos.y += 80;
	int i = 0;
	for (auto& e : Game::availableTargets) {
		this->cards.push_back(dynamic_cast<InGameTargetCard*>(l->addShape(new InGameTargetCard(startingPos.x, startingPos.y + (100 * i), 80, 80, e))));
		i += 1;
	}
}

InGameTargetCardDesk::~InGameTargetCardDesk() {
	for (auto& e : this->cards) {
		delete e;
	}
	this->cards.clear();
}

void InGameTargetCardDesk::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
	for (auto& e : this->cards) {
		e->draw(window);
	}
}
