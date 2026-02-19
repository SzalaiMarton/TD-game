#include "stdafx.hpp"

BaseMap::BaseMap(float xPos, float yPos, float xSize, float ySize, MapType type) : BaseShape(xPos, yPos, xSize, ySize, Assets::getTexture(mapTypeToTextureName(type))) {
	this->initClass(type);
}

BaseMap::BaseMap(const sf::Vector2f& pos, const sf::Vector2f& size, MapType type) : BaseShape(pos, size, Assets::getTexture(mapTypeToTextureName(type))) {
	this->initClass(type);
}

void BaseMap::draw(sf::RenderWindow* window) {
	window->draw(*this->getSC()->sprite);
	window->draw(*this->maskShape->getSC()->sprite);
}

void BaseMap::initClass(MapType type) {
	auto package = parseMapProperties(type);
	this->getSC()->setSize(package.mapSize);
	this->path = package.path;
	this->maskMap = Assets::getMapMask(type);
	this->maskShape = new Shape(0.f,0.f, package.mapSize.x, package.mapSize.y, new sf::Texture(*this->maskMap));
	this->maskShape->getSC()->sprite->setColor(sf::Color(255,255,255,0));
	this->mapSize = package.mapSize;
	this->validPlacementColor = package.validColor;
	this->invalidPlacementColor = package.invalidColor;
}

MapConfig parseMapProperties(MapType type) {
	std::ifstream file(mapPathLocation + mapTypeToPathName(type));

	if (!file.is_open()) {
		ERROR("parseMapPath", "Could not open file.");
		return MapConfig();
	}

	auto res = MapConfig();

	std::string line{};
	while (getline(file, line)) {
		if (line == "size") {
			res.mapSize = parseSize(file);
		}
		else if (line == "validColor") {
			res.validColor = parseValidColor(file);
		}
		else if (line == "invalidColor") {
			res.invalidColor = parseInvalidColor(file);
		}
		else if (line == "path") {
			res.path = parsePath(file);
		}
	}

	file.close();

	return res;
}

std::queue<sf::Vector2f> parsePath(std::istream& file) {
	std::queue<sf::Vector2f> path{};
	std::string line{};
	while (getline(file, line) && line != terminatorChar) {
		auto space = line.find(" ", 0);
		float x = std::stof(line.substr(0, space));
		float y = std::stof(line.substr(space));

		path.push({ x,y });
	}

	return path;
}

sf::Vector2f parseSize(std::istream& file) {
	sf::Vector2f res{};
	std::string line{};
	while (getline(file, line) && line != terminatorChar) {
		auto space = line.find(" ", 0);
		float x = std::stof(line.substr(0, space));
		float y = std::stof(line.substr(space));

		res = {x,y};
	}

	return res;
}

Color parseValidColor(std::istream& file) {
	Color res{};
	std::string line{};
	while (getline(file, line) && line != terminatorChar) {
		auto space = line.find(" ", 0);
		uint8_t r = (uint8_t)std::stoi(line.substr(0, space));

		auto prevSpace = space + 1;
		space = line.find(" ", prevSpace);
		uint8_t g = (uint8_t)std::stoi(line.substr(prevSpace, space));
		uint8_t b = (uint8_t)std::stoi(line.substr(space));

		res = { r, g, b };
	}
	return res;
}

Color parseInvalidColor(std::istream& file) {
	Color res{};
	std::string line{};
	while (getline(file, line) && line != terminatorChar) {
		auto space = line.find(" ", 0);
		uint8_t r = (uint8_t)std::stoi(line.substr(0, space));

		auto prevSpace = space + 1;
		space = line.find(" ", prevSpace);
		uint8_t g = (uint8_t)std::stoi(line.substr(prevSpace, space));
		uint8_t b = (uint8_t)std::stoi(line.substr(space));

		res = { r, g, b };
	}
	return res;
}
