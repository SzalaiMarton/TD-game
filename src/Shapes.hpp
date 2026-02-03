#pragma once

class Attribute;
class Layer;
class QuadTree;

struct BaseShape {
	Attribute attributes{};
	bool isHidden{false};
	std::set<QuadTree*> treeNodes{};
	std::string name{"-"};
	Layer* parentLayer{};

	BaseShape() = default;
	BaseShape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
	BaseShape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture);
	~BaseShape();

	virtual void draw(sf::RenderWindow* window) = 0;
	virtual bool onUpdate() = 0;
	virtual bool isOutOfScreen();

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
	bool onUpdate() override { return false; }
};

struct Button : public BaseShape {
	static sf::Font* buttonFont;
	sf::Text* text{};

	Button(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture, const std::string& text);
	Button(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, const std::string& text);

	bool onUpdate() override { return false; }

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

	bool onUpdate() override { return false; }

	void draw(sf::RenderWindow* window) override;
	void initClass(CatType catType);
};

struct Square : public BaseShape {
	sf::RectangleShape* s{};
	bool isVisible{true};
	Square(float xPos, float yPos, float xSize, float ySize);

	bool onUpdate() override { return false; }

	void draw(sf::RenderWindow* window) override;
};

class QuadTree {
public:
	static unsigned smallestSize;
	
	float xPos, yPos;
	unsigned xSize, ySize;
	std::set<BaseShape*> elements{};
	QuadTree* ne{};
	QuadTree* nw{};
	QuadTree* se{};
	QuadTree* sw{};
	bool hasBeenSplit = false; // if the tree had been split
	uint8_t cap;
	Square* s{};

	QuadTree(float x, float y, unsigned xs, unsigned ys, uint8_t c = 10);
	~QuadTree();

	void insert(BaseShape* e);
	void reinsert(BaseShape* e);
	bool containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const;
	bool containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const;
	bool containsCaseOnePoint(const sf::Vector2f& pos) const;
	bool containsCaseOnePoint(float xPos, float yPos) const;
	bool contains(const sf::Vector2f& pos, const sf::Vector2f& bound) const;
	bool split();
	void removeShape(BaseShape* shape);

	BaseShape* getClicked(const sf::Vector2f& mousePos);
	QuadTree* getClickedTree(const sf::Vector2f& mousePos);
	void draw(Layer* layer);
};
