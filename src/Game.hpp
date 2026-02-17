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
	WindowConfig windowConfig{60, 1100, 700, "TD game"};
	std::vector<CatType> availableCats{};

	GameConfig();

	GameConfig& setFPS(uint16_t value) { this->windowConfig.fps = value; return *this; }

	WindowConfig* getWindowConfig() { return &this->windowConfig; }

	operator WindowConfig*() { return this->getWindowConfig(); }
};

class Game {
public:
	static bool shuttingdown;
	static std::queue<std::pair<uint8_t, TargetType>> waitingTargets;
	static std::vector<CatType> selectedCats;
	static std::vector<TargetType> availableTargets;
	static TargetGroup* currentGroup;
	static BaseShape* beingDraggedShape;
	static BaseShape* clickedShape;
	static BaseShape* beingHoveredShape;
	static BaseMap* currentMap;
	static Layer* currentLayer;
	static GameConfig config;
	static GameState currentState;
	static uint16_t currentWaitTime;
	static uint16_t timeWaited;
	static uint16_t money;
	static bool wasHolding;

	Game();

	static void init();
	static void run();
	static void shutdown();
	static void update();

	static void handleClick();
	static void handleDrag();
	static void handleHover();

	static void handleInGame();

	static void initMainMenu();
	static void initLayers();
	static void initInventory();
	static void initInGame();

	static void switchLayer(GameState from, GameState to);
	static void queueTarget(uint8_t amount, TargetType type);
	static void initGroup();
};
