#pragma once

struct Shape;
class Target;

constexpr float pi = 3.14159f;

struct CatStat {
	uint16_t firerate{};
	uint16_t cost{};
	uint8_t range{};

	CatStat() {};
};

class BaseCat : public BaseShape {
protected:
	CatStat stats;
	bool rangeHidden{false};
	sf::CircleShape* range{};
	std::vector<BaseBullet*> bullets{};
	uint16_t cooldown{};

public:
	BaseCat();
	BaseCat(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture);

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
