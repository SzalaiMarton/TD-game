#pragma once

class BaseCat;
class Target;

class BaseBullet : public BaseShape {
public:
	Stats::BulletStat* stats{};
	BaseCat* parent{};

	BaseBullet(float xPos, float yPos, float xSize, float ySize, const sf::Vector2f& directionVector, BulletType type, BaseCat* parent);
	~BaseBullet();

	void onUpdate() override;
	void onDeath();
	void draw(sf::RenderWindow* window) override;

	void onCollision(Target* target);
	bool isColliding(Target* target);
	void checkCollision();

	void move();
};
