#pragma once

namespace Assets {
	extern std::unordered_map<std::string, sf::Texture*> loadedTextures;
	extern std::unordered_map<std::string, sf::Font*> loadedFonts;
	extern std::filesystem::path texturesPath;
	extern std::filesystem::path fontsPath;

	void loadTextures(const std::filesystem::path& path);
	void loadFonts(const std::filesystem::path& path);
	sf::Texture* getTexture(const std::string& name);
	sf::Font* getFont(const std::string& name);
}