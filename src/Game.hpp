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
	uint8_t validPlacementOffset{10};
	WindowConfig windowConfig{60, 1100, 700, "TD game"};
	std::vector<CatType> availableCats{};
	std::vector<TargetGroupType> availableTargetGroups{};

	GameConfig();

	GameConfig& setFPS(uint16_t value) { this->windowConfig.fps = value; return *this; }

	WindowConfig* getWindowConfig() { return &this->windowConfig; }

	operator WindowConfig*() { return this->getWindowConfig(); }
};

class Game {
public:
	static bool shuttingdown;
	static bool wasHolding;
	static bool placingEffectPlayed;
	static uint16_t targetSpawnDelay;
	static uint16_t targetSpawnTimeElapsed;
	static uint16_t money;
	static GameState currentState;
	static std::vector<CatType> selectedCats;
	static std::queue<Stats::TargetGroupStats*> waitingTargets;
	static std::queue<BaseShape*> toBeDeletedShapes;
	static BaseShape* beingDraggedShape;
	static BaseShape* beingHoveredShape;
	static TargetGroup* currentGroup;
	static BaseShape* clickedShape;
	static BaseMap* currentMap;
	static Layer* currentLayer;
	static GameConfig config;

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

	static void isPlacementValid(BaseCat* shape);
	
	static void switchLayer(GameState from, GameState to);
	static void queueTargetGroup(TargetGroupType type);
	static void initGroup();
};
