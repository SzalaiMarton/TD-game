#pragma once

struct Shape;
class Target;

constexpr float pi = 3.14159f;
constexpr uint16_t blackGreyPrice = 800;
constexpr uint16_t orangePrice = 1000;

constexpr uint16_t getPriceByType(CatType type);
void createCat(BaseCat*& bindTo, CatType type, float xPos, float yPos, float xSize, float ySize);

struct CatStat {
	uint16_t firerate{};
	uint16_t cost{};
	uint8_t range{};

	CatStat() {};
};

class BaseCat : public BaseShape {
protected:
	CatStat stats{};
	bool rangeHidden{true};
	sf::CircleShape* range{};
	std::vector<BaseBullet*> bullets{};
	uint16_t cooldown{};

public:
	BaseCat();
	BaseCat(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
	~BaseCat();

	virtual void fire(Target* target) = 0;

	void onDrag();
	void onClick();
	void onClickLoss();
	bool inRange(Target* target);
	void deleteBullet(BaseBullet* bullet);

	void showRange();
	void hideRange();

	void rotate(Target* target);
	void draw(sf::RenderWindow* window) override;
	void initClass();
};

class BlackGreyCat : public BaseCat {
public:
	BlackGreyCat();
	BlackGreyCat(float xPos, float yPos, float xSize, float ySize);

	void initClass();

	bool onUpdate() override;
	void fire(Target* target) override;
};

class OrangeCat : public BaseCat {
public:
	OrangeCat();
	OrangeCat(float xPos, float yPos, float xSize, float ySize);

	void initClass();

	bool onUpdate() override { return false; };
	void fire(Target* target) override {};
};

