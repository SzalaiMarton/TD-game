#pragma once

class TargetGroup;
struct Stats::TargetGroupStats;

class Target : public BaseShape {
public:
	bool isSpawned{false};
	Stats::TargetStats* stats{};
	std::queue<sf::Vector2f> path{};

	Target(TargetType type);
	~Target();

	void onUpdate() override;
	void draw(sf::RenderWindow* window) override;

	void onDeath();
	bool die();

	void takeDmg(uint16_t dmg);
	void retrievePath();
	void move();
	void spawn();
};

class TargetGroup {
public:
	uint8_t targetAmount{};
	uint16_t spawnDelay{};
	TargetType type{};
	std::vector<Target*> targets{};

	TargetGroup(Stats::TargetGroupStats* stats);

	void spawnNext();
	std::pair<uint8_t, TargetGroup*> initSpawn();
};

