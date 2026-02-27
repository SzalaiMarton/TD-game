#pragma once

class BaseCat;
class Target;

struct BulletStat {
	uint16_t dmg{};
	uint8_t speed{};
	uint8_t penetration{};
	sf::Vector2f directionVector{};

	BulletStat() = default;
	BulletStat(uint16_t dmg, uint8_t speed, uint8_t penetration) : dmg(dmg), speed(speed), penetration(penetration) {}
};

class BaseBullet : public BaseShape {
public:
	BulletStat* stats{};
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

std::pair<sf::Texture*, BulletStat*> createBullet(BulletType type);
std::pair<sf::Texture*, BulletStat*> createBasic();
