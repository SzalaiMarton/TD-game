#pragma once

class BaseCat;
class Target;

struct BulletStat {
	uint16_t dmg{};
	uint8_t speed{};
	uint8_t penetration{};
	sf::Vector2f directionVector{};

	BulletStat() = default;
	BulletStat(uint16_t dmg, uint8_t speed, uint8_t penetration) : dmg(dmg), speed(speed) {}
};

class BaseBullet : public BaseShape {
public:
	BulletStat* stats{};
	BaseCat* parent{};

	BaseBullet(float xPos, float yPos, float xSize, float ySize, const sf::Vector2f& directionVector, BulletType type, BaseCat* parent);
	~BaseBullet();

	bool onUpdate() override;
	void draw(sf::RenderWindow* window) override;

	void onCollision(Target* target);
	bool isColliding(Target* target);
	bool checkCollision();

	void move();
};

std::pair<sf::Texture*, BulletStat*> createBullet(BulletType type);
std::pair<sf::Texture*, BulletStat*> createBasic();
