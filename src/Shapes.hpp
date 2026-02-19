#pragma once

class Attribute;
class Layer;
struct QuadTree;
class BaseCat;

constexpr extern uint16_t getPriceByType(CatType type);
constexpr extern uint16_t getPriceByType(TargetType type);
constexpr extern uint8_t getAmountByType(TargetType type);
extern void createCat(BaseCat*& bindTo, CatType type, float xPos, float yPos, float xSize, float ySize);


struct BaseShape {
	Attribute* attributes{};
	bool isHidden{false};
	std::set<QuadTree*> treeNodes{};
	Layer* parentLayer{};
	//debug
	std::string name{"-"};
	//debug

	BaseShape() = default;
	BaseShape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
	BaseShape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture);
	~BaseShape();

	virtual void draw(sf::RenderWindow* window) = 0;
	virtual void onUpdate() = 0;
	virtual bool isOutOfScreen();

	SpriteComponent* getSC();
	MIC* getMIC();
	void showShape();
	void hideShape();
	void detachFromEveryTree();
	std::set<QuadTree*>::iterator detachFromTree(std::set<QuadTree*>::iterator it);
	void initClass(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
	void updateTree();
};

struct Shape : public BaseShape {
	Shape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) : BaseShape(xPos, yPos, xSize, ySize, texture) {}
	Shape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture) : BaseShape(pos, size, texture) {}
	void draw(sf::RenderWindow* window) override;
	void onUpdate() override {  }
};

struct Button : public BaseShape {
	static sf::Font* buttonFont;
	sf::Text* text{};

	Button(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture, const std::string& text);
	Button(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, const std::string& text);
	~Button();

	void onUpdate() override {  }

	void addClickHanlder(std::function<void()> handler);
	void draw(sf::RenderWindow* window) override;
	void initClass(const std::string& text);
};

struct InvetoryCard : public BaseShape {
	static sf::Font* cardFont;
	sf::Sprite* displayCat{};
	sf::Text* displayCatName{};
	sf::Text* displayCatDescription{};

	InvetoryCard(float xPos, float yPos, float xSize, float ySize, CatType catType);
	InvetoryCard(const sf::Vector2f& pos, const sf::Vector2f& size, CatType catType);
	~InvetoryCard();

	void onUpdate() override {  }

	void draw(sf::RenderWindow* window) override;
	void initClass(CatType catType);
};

struct Square : public BaseShape {
	sf::RectangleShape* s{};
	bool isVisible{true};

	Square(float xPos, float yPos, float xSize, float ySize);
	~Square();

	void onUpdate() override {  }

	void draw(sf::RenderWindow* window) override;
};

struct InGameCatCard : Button {
	static float iconOffset;
	static float textOffset;

	Shape* icon{};
	BaseCat* draggedCat{};

	InGameCatCard(float xPos, float yPos, float xSize, float ySize, CatType type);
	~InGameCatCard();

	void initClass(CatType type);
	void draw(sf::RenderWindow* window) override;

	static void hoverHandler(InGameCatCard* card);
	static void hoverLossHandler(InGameCatCard* card);
};

struct InGameCatCardDesk : public Shape {
	std::vector<InGameCatCard*> cards{};

	InGameCatCardDesk(float xPos, float yPos, float xSize, float ySize, Layer* l);
	~InGameCatCardDesk();

	void draw(sf::RenderWindow* window) override;
};

struct InGameTargetCard : Button {
	static float iconOffset;
	static float textOffset;

	Shape* icon{};

	InGameTargetCard(float xPos, float yPos, float xSize, float ySize, TargetType type);
	~InGameTargetCard();

	void initClass(CatType type);
	void draw(sf::RenderWindow* window) override;

	static void hoverHandler(InGameTargetCard* card);
	static void hoverLossHandler(InGameTargetCard* card);
};

struct InGameTargetCardDesk : public Shape {
	std::vector<InGameTargetCard*> cards{};

	InGameTargetCardDesk(float xPos, float yPos, float xSize, float ySize, Layer* l);
	~InGameTargetCardDesk();

	void draw(sf::RenderWindow* window) override;
};

struct QuadTree {
	static unsigned smallestSize;
	
	float xPos, yPos;
	unsigned xSize, ySize;
	std::set<BaseShape*> elements{};
	QuadTree* ne{};
	QuadTree* nw{};
	QuadTree* se{};
	QuadTree* sw{};
	bool hasBeenSplit = false;
	//debug
	Square* s{};
	std::string name{};
	//debug

	QuadTree(float x, float y, unsigned xs, unsigned ys, std::string name);
	~QuadTree();

	void insert(BaseShape* e);
	void split();
	bool containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const;
	bool containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const;
	bool containsCaseOnePoint(const sf::Vector2f& pos) const;
	bool containsCaseOnePoint(float xPos, float yPos) const;
	bool contains(const sf::Vector2f& pos, const sf::Vector2f& bound) const;
	void removeShape(BaseShape* shape);

	BaseShape* getClicked(const sf::Vector2f& mousePos);
	QuadTree* getClickedTree(const sf::Vector2f& mousePos);
	void draw(Layer* layer);
};
