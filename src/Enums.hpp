#pragma once

namespace TextureName {
	static std::string blackGreyCat = "black_grey.png";
	static std::string orangeCat = "orange.png";
	static std::string defalutTexture = "default_texture.png";
	static std::string inventoryCard = "inventory_card.png";
	static std::string placeHolderMap = "place_holder_map.png";
	static std::string ingameCard = "ingame_card.png";
}

enum class CatType {
	BLACKGREY,
	ORANGE,
	NONE
};

enum class TargetType {
	NONE,
	BASIC,
};

enum class GameState {
	MAINMENU,
	INGAME,
	PREGAME,
	POSTGAME,
	INVENTORY,
	NONE
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
	BACKGROUNDSCENE = 20,
	NONE = 0
};

enum class HandlerType {
	CLICKHANDLER,
	DRAGHANDLER,
	HOVERHANDLER,
	HOVERLOSSHANDLER,
	CLICKLOSSHANDLER,
	DRAGLOSSHANDLER
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

constexpr std::string catTypeToTextureName(CatType catType) {
	switch (catType) {
	case CatType::BLACKGREY:
		return TextureName::blackGreyCat;
	case CatType::ORANGE:
		return TextureName::orangeCat;
	default:
		return TextureName::defalutTexture;
	}
}

constexpr MainLayerName gameStateToLayerName(GameState state) {
	switch (state) {
	case GameState::MAINMENU:
		return MainLayerName::MAINMENU;
	case GameState::INGAME:
		return MainLayerName::INGAME;
	case GameState::POSTGAME:
		return MainLayerName::POSTGAME;
	case GameState::PREGAME:
		return MainLayerName::PREGAME;
	case GameState::INVENTORY:
		return MainLayerName::INVENTORY;
	}
	return MainLayerName::NONE;
}

constexpr GameState layerNameToGameState(MainLayerName layer) {
	switch (layer) {
	case MainLayerName::MAINMENU:
		return GameState::MAINMENU;
	case MainLayerName::INGAME:
		return GameState::INGAME;
	case MainLayerName::POSTGAME:
		return GameState::POSTGAME;
	case MainLayerName::PREGAME:
		return GameState::PREGAME;
	case MainLayerName::INVENTORY:
		return GameState::INVENTORY;
	}
	return GameState::NONE;
}

constexpr std::string targetTypeToTextureName(TargetType type) {
	switch (type) {
	case TargetType::BASIC:
		return TextureName::defalutTexture;
	default:
		return TextureName::defalutTexture;
	}
}
