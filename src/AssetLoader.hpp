#pragma once

namespace Assets {
	static std::unordered_map<std::string, sf::Texture*> loadedTextures{};
	static std::unordered_map<std::string, sf::Font*> loadedFonts{};
	constexpr std::string_view texturesPath = "res//textures//";
	constexpr std::string_view fontsPath = "res//fonts//";

	void loadTextures(const std::filesystem::path& path);
	void loadFonts(const std::filesystem::path& path);
	sf::Texture* getTexture(std::string_view name);
	sf::Font* getFont(std::string_view name);
	sf::Image* getMapMask(MapType type);
}

namespace TextureName {
	constexpr std::string_view blackGreyCat = "black_grey.png";
	constexpr std::string_view orangeCat = "orange.png";
	constexpr std::string_view defalutTexture = "default_texture.png";
	constexpr std::string_view beaten_box = "beaten_box.png";
	constexpr std::string_view inventoryCard = "inventory_card.png";
	constexpr std::string_view placeHolderMap = "place_holder_map.png";
	constexpr std::string_view ingameCard = "ingame_card.png";
	constexpr std::string_view roadMask = "road_mask.png";
	constexpr std::string_view roadMap = "road_map.png";

	constexpr std::string_view placeHolderMapPath = "place_holder_map.txt";
}

constexpr std::string_view enumToTextureName(CatType type) {
	switch (type) {
	case CatType::BLACKGREY:
		return TextureName::blackGreyCat;
	case CatType::ORANGE:
		return TextureName::orangeCat;
	default:
		return TextureName::defalutTexture;
	}
}

constexpr std::string_view enumToTextureName(TargetType type) {
	switch (type) {
	case TargetType::BEATEN:
		return TextureName::beaten_box;
	default:
		return TextureName::defalutTexture;
	}
}

constexpr std::string_view enumToTextureName(BulletType type) {
	switch (type) {
	case BulletType::BASIC:
		return TextureName::defalutTexture;
	default:
		return TextureName::defalutTexture;
	}
}

constexpr std::string_view enumToTextureName(TargetGroupType type) {
	switch (type) {
		case TargetGroupType::BEATEN_8:
			return TextureName::beaten_box;
	default:
		return TextureName::defalutTexture;
	}
}

constexpr std::string_view enumToTextureName(MapType type) {
	switch (type) {
	case MapType::PLACEHOLDER:
		return TextureName::placeHolderMap;
	case MapType::ROADMAP:
		return TextureName::roadMap;
	default:
		return TextureName::placeHolderMap;
	}
}

constexpr std::string_view getMaskTextureName(MapType type) {
	switch (type) {
	case MapType::PLACEHOLDER:
		return TextureName::placeHolderMap;
	case MapType::ROADMAP:
		return TextureName::roadMask;
	default:
		return TextureName::placeHolderMap;
	}
}

constexpr std::string_view mapTypeToPathName(MapType type) {
	switch (type) {
	case MapType::PLACEHOLDER:
		return TextureName::placeHolderMapPath;
	case MapType::ROADMAP:
		return TextureName::placeHolderMapPath;
	default:
		return TextureName::placeHolderMapPath;
	}
}
