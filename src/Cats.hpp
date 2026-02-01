#pragma once

struct Shape;

struct CatStat {
	uint16_t firerate{};
	uint16_t cost{};
	uint8_t range{};

	CatStat() {};
};

class BaseCat : public BaseShape {
protected:
	CatStat stats;
	bool rangeHidden{true};
	sf::CircleShape* range{};

public:
	BaseCat();
	BaseCat(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);

	virtual void fire(BaseShape* shape) = 0;
	virtual void onUpdate() = 0;
	void onDrag();
	void onClick();
	void onClickLoss();
	bool inRange(BaseShape* shape);

	void showRange();
	void hideRange();

	void draw(sf::RenderWindow* window) override;
	void initClass();
};

class BlackGreyCat : public BaseCat {
public:
	BlackGreyCat();
	BlackGreyCat(float xPos, float yPos, float xSize, float ySize);

	void initClass();

	void onUpdate() override;
	void fire(BaseShape* shape) override;
};
