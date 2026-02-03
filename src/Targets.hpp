#pragma once

class TargetGroup;

struct TargetStats {
	uint32_t health{};
	uint8_t speed{};
	sf::Vector2f size{};
	TargetType childType{TargetType::NONE};

	TargetStats() = default;
	TargetStats(uint32_t health, uint8_t speed, const sf::Vector2f& size, TargetType child) : health(health), speed(speed), size(size), childType(child) {};
};

class Target : public BaseShape {
public:
	TargetStats* stats{};
	std::queue<sf::Vector2f> path{};
	bool isSpawned{false};

	Target(TargetType type);
	~Target();

	bool onUpdate() override;
	void draw(sf::RenderWindow* window) override;

	void onDeath();
	bool die();

	void takeDmg(uint16_t dmg);
	void retrievePath();
	void move();
	void spawn(Layer* layer);
};

class TargetGroup {
public:
	std::vector<Target*> targets{};

	TargetGroup(uint8_t amount, TargetType type);

	void spawnNext(Layer* layer);
	std::pair<uint8_t, TargetGroup*> initSpawn();
};

TargetStats* createTargetStats(TargetType type);
TargetStats* createPlaceHolder(); 
