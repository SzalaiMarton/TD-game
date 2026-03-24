#pragma once

struct fVector2 {
	float x;
	float y;
	fVector2() : x(0), y(0) {}
	fVector2(float x, float y) : x(x), y(y) {}
	fVector2(const sf::Vector2f& v) : x(v.x), y(v.y) {}

	fVector2 operator+(const fVector2& other) const {
		return fVector2(x + other.x, y + other.y);
	}
	fVector2 operator-(const fVector2& other) const {
		return fVector2(x - other.x, y - other.y);
	}
	fVector2 operator*(float scalar) const {
		return fVector2(x * scalar, y * scalar);
	}
	fVector2 operator/(float scalar) const {
		return fVector2(x / scalar, y / scalar);
	}

	operator sf::Vector2f() const {
		return sf::Vector2f({x,y});
	}

	bool operator==(const fVector2& other) const {
		return x == other.x && y == other.y;
	}
};

namespace std {
	template<>
	struct hash<fVector2> {
		size_t operator()(const fVector2& p) const noexcept {
			auto x = std::hash<float>{}(p.x);
			auto y = std::hash<float>{}(p.y);
			return x ^ (y << 1);
		}
	};
}