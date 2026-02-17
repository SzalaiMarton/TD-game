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
	bool isDisabled{true};

public:
	BaseCat();
	BaseCat(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);
	~BaseCat();

	virtual void fire(Target* target);
	virtual bool onUpdate();

	void onDrag();
	void onClick();
	void onClickLoss();
	void deleteBullet(BaseBullet* bullet);
	
	void enableCat();
	void disableCat();
	void showRange();
	void hideRange();

	bool checkForFire();
	bool inRange(Target* target);
	void rotate(Target* target);
	void draw(sf::RenderWindow* window) override;
	
	void initClass();
};

class BlackGreyCat : public BaseCat {
public:
	BlackGreyCat();
	BlackGreyCat(float xPos, float yPos, float xSize, float ySize);

	void initClass();
};

class OrangeCat : public BaseCat {
public:
	OrangeCat();
	OrangeCat(float xPos, float yPos, float xSize, float ySize);

	void initClass();
};

