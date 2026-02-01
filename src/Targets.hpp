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

	void onUpdate();
	void onDeath();
	void die();

	void retrievePath();
	void draw(sf::RenderWindow* window) override;
	void move();
	void spawn();
};

class TargetGroup {
public:
	std::vector<Target*> targets{};

	TargetGroup(uint8_t amount, TargetType type);

	void spawnNext();
	std::pair<uint8_t, TargetGroup*> initSpawn();
};

TargetStats* createTargetStats(TargetType type);
TargetStats* createPlaceHolder(); 
