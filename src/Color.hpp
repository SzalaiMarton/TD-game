#pragma once

struct Color {
	uint8_t red{};
	uint8_t green{};
	uint8_t blue{};

	Color() = default;
	Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}

	operator std::string() {
		return std::to_string((int)red) + " " + std::to_string((int)green) + " " + std::to_string((int)blue);
	}

	operator sf::Color() {
		return sf::Color(red,green,blue);
	}
};
