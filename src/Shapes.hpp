#pragma once

class Attribute;
class Layer;
class QuadTree;

struct BaseShape {
	Attribute attributes{};
	bool isHidden{false};
	QuadTree* treeNode{};

	BaseShape() = default;
	BaseShape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
	BaseShape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture);

	void showShape();
	void hideShape();
	void detachFromTree();
	virtual void draw(sf::RenderWindow* window) = 0;
	void initClass(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
};

struct Shape : public BaseShape {
	Shape(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture) : BaseShape(xPos, yPos, xSize, ySize, texture) {}
	Shape(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture) : BaseShape(pos, size, texture) {}
	void draw(sf::RenderWindow* window);
};

struct Button : public BaseShape {
	static sf::Font* buttonFont;
	sf::Text* text;

	Button(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture, const std::string& text);
	Button(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, const std::string& text);

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

	void draw(sf::RenderWindow* window) override;
	void initClass(CatType catType);
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
	bool disabled = false;	// if the tree had been reinserted -> cannot hold elements
	bool cannotBeSplit = false;	// if the size has reached the limit
	uint8_t cap;

	QuadTree(float x, float y, unsigned xs, unsigned ys, uint8_t c = 10);
	~QuadTree();

	void reinsert(BaseShape* e);
	void insert(BaseShape* e);
	bool containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const; // true if either x or y axis is in the target object 
	bool containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const; // true if either x or y axis is in the target object 
	bool contains(const sf::Vector2f& pos) const;
	bool contains(float xPos, float yPos) const;
	bool split();
	void tryChild(const sf::Vector2f& ePos, const sf::Vector2f& eBound, BaseShape* e);

	BaseShape* getClicked(const sf::Vector2f& mousePos);
};
