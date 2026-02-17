#include "stdafx.hpp"

sf::Font* Button::buttonFont = nullptr;
sf::Font* InvetoryCard::cardFont = nullptr;
unsigned QuadTree::smallestSize = 200;
float InGameCard::iconOffset = -15.f;
float InGameCard::textOffset = 15.f;

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
	auto bound = this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size;
	sf::Vector2f screenTopLeft = { 0.f, 0.f };
	auto screenBotRight = (sf::Vector2f)Renderer::getWindow()->getSize();

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

	auto bound = this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size;
	this->attributes.getComponent<SpriteComponent>()->setPos(xPos, yPos);
	this->attributes.getComponent<SpriteComponent>()->setSize(xSize, ySize);
}

void BaseShape::updateTree() {
	for (auto it = this->treeNodes.begin(); it != this->treeNodes.end();) {
		if (!(*it)->contains(this->attributes.getComponent<SpriteComponent>()->getPos(), this->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size)) {
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

		auto boundText = this->text->getLocalBounds().size;
		auto boundParent = this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size;
		auto posParent = this->attributes.getComponent<SpriteComponent>()->getPos();
		this->text->setOrigin({ boundText.x / 2, boundText.y / 2 });
		this->text->setPosition({ posParent.x - 10 + boundParent.x / 2, posParent.y - 10 + boundParent.y / 2 });
	}
	catch (std::exception e) {
		ERROR("Button::Button", e.what());
	}

	auto pos = this->attributes.getComponent<SpriteComponent>()->getPos();
	auto bound = this->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size;
	this->attributes.getComponent<SpriteComponent>()->setPos(pos.x - bound.x / 2, pos.y - bound.y / 2);

	std::initializer_list<MIC::SingleHandler> handlers = {
		MIC::SingleHandler(HandlerType::HOVERHANDLER, [this]() { this->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255,255,255,128)); }),
		MIC::SingleHandler(HandlerType::HOVERLOSSHANDLER, [this]() { this->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255,255,255,255)); })
	};

	this->attributes.addComponent<MIC>(new MIC(handlers));
	//this->attributes.getComponent<SpriteComponent>()->sprite->setOrigin({ 0, 0 });
}

void Shape::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
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
		!this->contains(e->attributes.getComponent<SpriteComponent>()->getPos(), 
			e->attributes.getComponent<SpriteComponent>()->getPos() + 
			e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size)
		) {
		//debug
		//if (this && dynamic_cast<InGameCard*>(e)) {
		//	LOG(e << " was not inserted to " << this->name);
		//	LOG((this->contains(e->attributes.getComponent<SpriteComponent>()->getPos(),
		//		e->attributes.getComponent<SpriteComponent>()->getPos() +
		//		e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size)
		//		));
		//	LOG(e->attributes.getComponent<SpriteComponent>()->getPos().x << " " <<
		//		e->attributes.getComponent<SpriteComponent>()->getPos().y << "    " << 
		//		(e->attributes.getComponent<SpriteComponent>()->getPos() + 
		//			e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size).x << " " <<
		//		(e->attributes.getComponent<SpriteComponent>()->getPos() +
		//			e->attributes.getComponent<SpriteComponent>()->sprite->getGlobalBounds().size).y
		//	);
		//}
		//debug
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

InvetoryCard::~InvetoryCard() {
	delete displayCat;
	displayCat = nullptr;
	delete displayCatDescription;
	displayCatDescription = nullptr;
	delete displayCatName;
	displayCatName = nullptr;
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

InGameCard::InGameCard(float xPos, float yPos, float xSize, float ySize, CatType type) : Button(xPos, yPos, xSize, ySize, Assets::getTexture(TextureName::ingameCard), std::to_string(getPriceByType(type))) {
	this->text->setPosition({ xPos, yPos + InGameCard::textOffset });
	
	this->icon = new Shape(xPos, yPos, xSize / 2 + 15, ySize / 2, Assets::getTexture(catTypeToString(type) + ".png"));
	auto iconBound = this->icon->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size;
	this->icon->attributes.getComponent<SpriteComponent>()->sprite->setOrigin({ iconBound.x / 2, iconBound.y / 2 });
	this->icon->attributes.getComponent<SpriteComponent>()->setPos({ xPos, yPos + InGameCard::iconOffset });

	this->attributes.getComponent<MIC>()->disableClick();
	this->attributes.getComponent<MIC>()->disableBaseDrag();
	this->attributes.getComponent<MIC>()->bindDragHandler([this, type]() {
			createCat(this->draggedCat, type, 0.f, 0.f, 60.f, 40.f);
			this->draggedCat->attributes.getComponent<SpriteComponent>()->setPos((sf::Vector2f)sf::Mouse::getPosition(*Renderer::getWindow()));
		});
	this->attributes.getComponent<MIC>()->bindDragLoss([this]() {
			this->draggedCat->updateTree();
			this->draggedCat = nullptr;
		});
}

InGameCard::~InGameCard() {
	delete icon;
	icon = nullptr;
}

void InvetoryCard::initClass(CatType catType) {
	auto dC = Assets::getTexture(catTypeToString(catType) + ".png");
	this->displayCat = dC ? new sf::Sprite(*dC) : nullptr;

	try {
		auto cardSprite = this->attributes.getComponent<SpriteComponent>()->sprite;
		
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

	this->attributes.getComponent<SpriteComponent>()->sprite->setOrigin({ 0.f, this->attributes.getComponent<SpriteComponent>()->sprite->getLocalBounds().size.y / 2 });
}

void InGameCard::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
	window->draw(*this->icon->attributes.getComponent<SpriteComponent>()->sprite);
	window->draw(*this->text);
}

void InGameCard::hoverHandler(InGameCard* card) {
	if (card) {
		card->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255, 255, 255, 128));
		card->icon->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255, 255, 255, 128));
		card->text->setFillColor(sf::Color(255, 255, 255, 128));
	}
}

void InGameCard::hoverLossHandler(InGameCard* card) {
	if (card) {
		card->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255, 255, 255, 255));
		card->icon->attributes.getComponent<SpriteComponent>()->sprite->setColor(sf::Color(255, 255, 255, 255));
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

InGameCardDesk::InGameCardDesk(float xPos, float yPos, float xSize, float ySize, Layer* l)
	: Shape(xPos, yPos, xSize, ySize, Assets::getTexture("default_texture.png")) {
	this->cards.reserve(Game::selectedCats.size());
	auto startingPos = this->attributes.getComponent<SpriteComponent>()->getPos();
	startingPos.x += 80;
	startingPos.y += 50;
	int i = 0;
	for (auto& e : Game::selectedCats) {
		this->cards.push_back(dynamic_cast<InGameCard*>(l->addShape(new InGameCard(startingPos.x + (100 * i), startingPos.y, 80, 80, e))));
		i += 1;
	}
}

void InGameCardDesk::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
	for (auto& e : this->cards) {
		e->draw(window);
	}
}
