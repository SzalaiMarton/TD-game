#pragma once

constexpr sf::Vector2f centerTo(sf::Drawable& parent) {
	if (dynamic_cast<sf::Sprite*>(&parent)) {
		sf::Vector2f s = { dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.x / 2, dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.y / 2 };
		return sf::Vector2f(dynamic_cast<sf::Sprite*>(&parent)->getPosition() + s);
	}
	else if (dynamic_cast<sf::Text*>(&parent)) {
		sf::Vector2f s = { dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.x / 2, dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.y / 2 };
		return sf::Vector2f(dynamic_cast<sf::Text*>(&parent)->getPosition() + s);
	}
	else if (dynamic_cast<sf::Shape*>(&parent)) {
		sf::Vector2f s = { dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.x / 2, dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.y / 2 };
		return sf::Vector2f(dynamic_cast<sf::Shape*>(&parent)->getPosition() + s);
	}
	return {0.f,0.f};
}

constexpr sf::Vector2f topLeft(sf::Drawable& parent) {
	return sf::Vector2f({ 0.f, 0.f });
}

constexpr sf::Vector2f topCenter(sf::Drawable& parent) {
	if (dynamic_cast<sf::Sprite*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.x / 2, 0.f });
	}
	else if (dynamic_cast<sf::Text*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.x / 2, 0.f });
	}
	else if (dynamic_cast<sf::Shape*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.x / 2, 0.f });
	}
	return { 0.f,0.f };
}

constexpr sf::Vector2f topRight(sf::Drawable& parent) {
	if (dynamic_cast<sf::Sprite*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.x, 0.f });
	}
	else if (dynamic_cast<sf::Text*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.x, 0.f });
	}
	else if (dynamic_cast<sf::Shape*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.x, 0.f });
	}
	return { 0.f,0.f };
}

constexpr sf::Vector2f botLeft(sf::Drawable& parent) {
	if (dynamic_cast<sf::Sprite*>(&parent)) {
		return sf::Vector2f({ 0.f, dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.y });
	}
	else if (dynamic_cast<sf::Text*>(&parent)) {
		return sf::Vector2f({ 0.f, dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.y });
	}
	else if (dynamic_cast<sf::Shape*>(&parent)) {
		return sf::Vector2f({ 0.f, dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.y });
	}
	return { 0.f,0.f };
}

constexpr sf::Vector2f botCenter(sf::Drawable& parent) {
	if (dynamic_cast<sf::Sprite*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.x / 2, dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.y });
	}
	else if (dynamic_cast<sf::Text*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.x / 2, dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.y });
	}
	else if (dynamic_cast<sf::Shape*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.x / 2, dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.y });
	}
	return { 0.f,0.f };
}

constexpr sf::Vector2f botRight(sf::Drawable& parent) {
	if (dynamic_cast<sf::Sprite*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.x, dynamic_cast<sf::Sprite*>(&parent)->getGlobalBounds().size.y });
	}
	else if (dynamic_cast<sf::Text*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.x, dynamic_cast<sf::Text*>(&parent)->getGlobalBounds().size.y });
	}
	else if (dynamic_cast<sf::Shape*>(&parent)) {
		return sf::Vector2f({ dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.x, dynamic_cast<sf::Shape*>(&parent)->getGlobalBounds().size.y });
	}
	return { 0.f,0.f };
}

constexpr sf::Vector2f centerTo(sf::RenderWindow& parent) {
	return sf::Vector2f({ (float)parent.getSize().x / 2, (float)parent.getSize().y / 2 });
}

constexpr sf::Vector2f topLeft(sf::RenderWindow& parent) {
	return sf::Vector2f({ 0.f, 0.f });
}

constexpr sf::Vector2f topCenter(sf::RenderWindow& parent) {
	return sf::Vector2f({ (float)parent.getSize().x / 2, 0.f });
}

constexpr sf::Vector2f topRight(sf::RenderWindow& parent) {
	return sf::Vector2f({ (float)parent.getSize().x, 0.f });
}

constexpr sf::Vector2f botLeft(sf::RenderWindow& parent) {
	return sf::Vector2f({ 0.f, (float)parent.getSize().y });
}

constexpr sf::Vector2f botCenter(sf::RenderWindow& parent) {
	return sf::Vector2f({ (float)parent.getSize().x / 2, (float)parent.getSize().y });
}

constexpr sf::Vector2f botRight(sf::RenderWindow& parent) {
	return sf::Vector2f({ (float)parent.getSize().x, (float)parent.getSize().y });
}