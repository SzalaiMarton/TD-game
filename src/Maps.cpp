#include "stdafx.hpp"

BaseMap::BaseMap(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture, MapType type) : BaseShape(xPos, yPos, xSize, ySize, texture) {
	this->initClass(type);
}

BaseMap::BaseMap(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, MapType type) : BaseShape(pos, size, texture) {
	this->initClass(type);
}

void BaseMap::draw(sf::RenderWindow* window) {
	window->draw(*this->attributes.getComponent<SpriteComponent>()->sprite);
}

void BaseMap::initClass(MapType type) {
	this->path = parseMapPath(type);
}

std::queue<sf::Vector2f> parseMapPath(MapType type) {
	std::ifstream file(mapPathLocation + mapTypeToFileName(type));

	if (!file.is_open()) {
		ERROR("parseMapPath", "Could not open file.");
		return {};
	}

	std::queue<sf::Vector2f> res{};
	std::string line;
	while (std::getline(file, line)) {
		auto spaceIdx = line.find(" ", 1);
		float x = (float)std::stoi(line.substr(0, spaceIdx));
		float y = (float)std::stoi(line.substr(spaceIdx, line.size() - spaceIdx));
		res.push({ x, y });

	}
	return res;
}
