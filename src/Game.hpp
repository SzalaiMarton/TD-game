#pragma once

struct WindowConfig {
	uint16_t fps{};
	int xSize{};
	int ySize{};
	std::string name{};

	WindowConfig(uint16_t fps, int x, int y, std::string_view name) : fps(fps), xSize(x), ySize(y), name(name) {}
};

struct GameConfig {
	// default values are the default configs for the game
	WindowConfig windowConfig{60, 1000, 600, "TD game"};
	std::vector<CatType> availableCats{};

	GameConfig();

	GameConfig& setFPS(uint16_t value) { this->windowConfig.fps = value; return *this; }

	WindowConfig* getWindowConfig() { return &this->windowConfig; }

	operator WindowConfig*() { return this->getWindowConfig(); }
};

class Game {
public:
	static GameConfig config;
	static bool wasHolding;
	static BaseShape* beingDraggedShape;
	static BaseShape* clickedShape;
	static BaseShape* beingHoveredShape;
	static std::queue<std::pair<uint8_t, TargetType>> waitingTargets;
	static BaseMap* currentMap;
	static uint16_t currentWaitTime;
	static uint16_t timeWaited;
	static TargetGroup* currentGroup;
	static GameState currentState;

	Game();

	void init();
	void run();
	void shutdown();
	void update();

	void handleClick();
	void handleDrag();
	void handleHover();

	void handleInGame();

	void initMainMenu();
	void initLayers();
	void initInventory();
	void initInGame();

	void queueTarget(uint8_t amount, TargetType type);
	void initGroup();
};
