#pragma once

struct BulletStat {
	uint16_t dmg{};
};

class BaseBullet : public BaseShape {
public:
	BaseBullet(float xPos, float yPos, float xSize, float ySize, BulletType type);
	void initClass();

	void onUpdate();

	void move();

	void draw(sf::RenderWindow* window) override;
};
