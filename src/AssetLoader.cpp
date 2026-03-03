#include "stdafx.hpp"

extern std::unordered_map<std::string, sf::Texture*> Assets::loadedTextures;
extern std::unordered_map<std::string, sf::Font*> Assets::loadedFonts;

void Assets::loadTextures(const std::filesystem::path& path) {
	if (std::filesystem::exists(path)) {
		for (auto& element : std::filesystem::directory_iterator(path)) {
			if (std::filesystem::is_directory(element)) {
				Assets::loadTextures(element.path());
			}
			else {
				try {
					auto t = new sf::Texture(element.path());
					loadedTextures.insert({ element.path().filename().string(), t });
				}
				catch (sf::Exception e) {
					ERROR("Assets::loadAssets", e.what());
				}
			}
		}
	}
	else {
		ERROR("Assets::loadAssets", "path was not found.");
	}
}

void Assets::loadFonts(const std::filesystem::path& path) {
	if (std::filesystem::exists(path)) {
		for (auto& element : std::filesystem::directory_iterator(path)) {
			try {
				auto t = new sf::Font(element.path());
				loadedFonts.insert({ element.path().filename().string(), t });
			}
			catch (sf::Exception e) {
				ERROR("Assets::loadAssets", e.what());
			}
		}
	}
	else {
		ERROR("Assets::loadAssets", "path was not found.");
	}
}

sf::Texture* Assets::getTexture(std::string_view name) {
	auto it = loadedTextures.find(std::string(name));
	return it != loadedTextures.end() ? it->second : nullptr;
}

sf::Font* Assets::getFont(std::string_view name) {
	auto it = loadedFonts.find(std::string(name));
	return it != loadedFonts.end() ? it->second : nullptr;
}

sf::Image* Assets::getMapMask(MapType type) {
	auto res = new sf::Image();
	auto path = std::string(texturesPath) + std::string(getMaskTextureName(type));
	if (res->loadFromFile(path)) {
		return res;
	}
	return nullptr;
}
