#pragma once

struct Shape;
class Target;

constexpr float pi = 3.14159f;

void createCat(BaseCat*& bindTo, CatType type, float xPos, float yPos);

class BaseCat : public BaseShape {
protected:
	Stats::CatStat* stats{};
	bool rangeHidden{true};
	sf::CircleShape* range{};
	std::vector<BaseBullet*> bullets{};
	uint16_t cooldown{};
	bool isDisabled{true};
	bool canBePlaced{true};

public:
	BaseCat();
	BaseCat(float xPos, float yPos, CatType type);
	~BaseCat();

	virtual void checkForFire() = 0;
	virtual void fire(Target* target);
	virtual void onUpdate();

	void onDrag();
	void onClick();
	void onClickLoss();
	void deleteBullet(BaseBullet* bullet);
	
	void enableCat();
	void disableCat();
	void showRange();
	void hideRange();
	void enablePlace();
	void disablePlace();

	void place();

	bool inRange(Target* target);
	void rotate(Target* target);
	void draw(sf::RenderWindow* window) override;
	
	void initClass(CatType type);
};

class RangedCat : public BaseCat {
public:
	RangedCat(CatType type)
		: BaseCat(0, 0, type) {}
	RangedCat(float xPos, float yPos, CatType type) 
		: BaseCat(xPos, yPos, type) {}

	void checkForFire() override;
};

class MeleeCat : public BaseCat {
public:
	MeleeCat(CatType type)
		: BaseCat(0, 0, type) {}
	MeleeCat(float xPos, float yPos, CatType type)
		: BaseCat(xPos, yPos, type) {}

	void checkForFire() override;
	void fire(Target* target) override;
};

class BlackGreyCat : public RangedCat {
public:
	BlackGreyCat() 
		: RangedCat(CatType::BLACKGREY) {}
	BlackGreyCat(float xPos, float yPos) 
		: RangedCat(xPos, yPos, CatType::BLACKGREY) {}
};

class OrangeCat : public MeleeCat {
public:
	OrangeCat() 
		: MeleeCat(CatType::ORANGE) {}
	OrangeCat(float xPos, float yPos) 
		: MeleeCat(xPos, yPos, CatType::ORANGE) {}
};

