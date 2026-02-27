#pragma once

class TargetGroup;

constexpr uint16_t getPriceByGroupType(TargetGroupType type);
constexpr uint8_t getAmountByType(TargetType type);

class Target : public BaseShape {
public:
	bool isSpawned{false};
	std::queue<sf::Vector2f> path{};
	Stats::TargetStats* stats{};

	Target(TargetType type);
	~Target();

	void onUpdate() override;
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
	uint8_t targetAmount{};
	uint16_t spawnDelay{};
	TargetType type{};
	std::vector<Target*> targets{};

	TargetGroup(TargetGroupStats* stats);

	void spawnNext(Layer* layer);
	std::pair<uint8_t, TargetGroup*> initSpawn();
};

TargetGroupStats* getGroupStats(TargetGroupType type);
