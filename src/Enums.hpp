#pragma once

enum class CatType {
	BLACKGREY,
	ORANGE
};

enum class TargetType {
	NONE,
	BASIC,
};

enum class GameState {
	MENU,
	INGAME,
	PREGAME,
	POSTGAME
};

enum class MapType {
	PLACEHOLDER
};

enum class MainLayerName {
	PAUSEMENU = 1,
	PREGAME = 4,
	INGAME = 5,
	POSTGAME = 6,
	MAINMENU = 7,
	SETTINGS = 8,
	INVENTORY = 9,
	BACKGROUNDSCENE = 20
};

enum class HandlerType {
	CLICKHANDLER,
	DRAGHANDLER,
	HOVERHANDLER,
	HOVERLOSSHANDLER,
	CLICKLOSSHANDLER
};

enum class BulletType {
	BASIC,
};

constexpr std::string mapTypeToFileName(MapType type) {
	switch (type) {
	case MapType::PLACEHOLDER:
		return "place_holder_map.txt";
	default:
		return "none";
	}
}

constexpr std::string catTypeToString(CatType catType) {
	switch (catType) {
	case CatType::BLACKGREY:
		return "black_grey";
	case CatType::ORANGE:
		return "orange";
	default:
		return "none";
	}
}

constexpr MainLayerName gameStateToLayerName(GameState state) {
	switch (state) {
	case GameState::MENU:
		return MainLayerName::MAINMENU;
	case GameState::INGAME:
		return MainLayerName::INGAME;
	case GameState::POSTGAME:
		return MainLayerName::POSTGAME;
	case GameState::PREGAME:
		return MainLayerName::PREGAME;
	}
}