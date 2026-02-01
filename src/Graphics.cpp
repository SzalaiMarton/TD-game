#include "stdafx.hpp"

sf::RenderWindow* Renderer::window{};
sf::View* Renderer::view{};
std::map<MainLayerName, Layer*> Renderer::layers{};
uint8_t Renderer::nextFreeIndex = 1;

Renderer::Renderer() {}

void Renderer::init(WindowConfig* config = nullptr) {
	if (!window && config) {
		window = new sf::RenderWindow(sf::VideoMode({ (unsigned)config->xSize, (unsigned)config->ySize }), config->name);
		window->setFramerateLimit(config->fps);
		view = new sf::View(sf::FloatRect({ 0.f, 0.f }, { (float)config->xSize, (float)config->ySize }));
		window->setView(*view);
	}
	else {
		throw std::exception();
	}
}

sf::RenderWindow* Renderer::getWindow() {
	if (!window) {
		try {
			Renderer::init();
		}
		catch (std::exception& e) {
			ERROR("Renderer::getWindow", e.what());
			throw std::exception();
		}
	}
	return window;
}

void Renderer::pushLayer(Layer* layer, MainLayerName index) {
	if (layer) {
		layers.insert({index, layer});
	}
	else {
		ERROR("Renderer::pushLayer", "object was nullptr.");
	}
}

void Renderer::removeLayer(MainLayerName index, Layer* layer) {
	if (layer) {
		auto it = std::find(layers.begin(), layers.end(), std::pair(index, layer));
		if (it != layers.end()) {
			layers.erase(it);
		}
		else {
			ERROR("Renderer::removeLayer", "layer was not found.");
		}
	}
	else {
		ERROR("Renderer::removeLayer", "layer was nullptr.");
	}
}

void Renderer::refreshFrame() {
	window->clear();
	for (auto& layer : layers) {
		if (!layer.second->getIsHidden()) {
			for (auto& s : *layer.second->getShapes()) {
				if (s->attributes.hasComponent<SpriteComponent>() && Game::beingDraggedShape != s) {
					s->draw(window);
				}
			}
		}
	}

	if (Game::beingDraggedShape) {
		Game::beingDraggedShape->draw(window);
	}
	
	window->display();
}

void Renderer::onResize() {
	view->setCenter({ (float)(window->getSize().x / 2), (float)(window->getSize().y / 2) });
	view->setSize((sf::Vector2f)window->getSize());
	window->setView(*view);
}

Layer* Renderer::getLayer(MainLayerName index) {
	for (auto& e : layers) {
		if (e.first == index) {
			return e.second;
		}
	}
	return nullptr;
}

Layer::Layer() {
	this->root = new QuadTree(-5000, -5000, Game::config.windowConfig.xSize * 2, Game::config.windowConfig.ySize * 2);
}

void Layer::showLayer() {
	this->isHidden = false;
	for (auto& e : shapes) {
		e->showShape();
	}
}

void Layer::hideLayer() {
	this->isHidden = true;
	for (auto& e : shapes) {
		e->hideShape();
	}
}

bool Layer::getIsHidden() {
	return this->isHidden;
}

BaseShape* Layer::addShape(BaseShape* shape) {
	if (shape) {
		this->shapes.push_back(shape);
		shape->isHidden = this->isHidden;
		this->root->insert(shape);
	}
	return shape;
}

void Layer::removeShape(BaseShape* shape) {
	if (shape) {
		auto it = std::find(this->shapes.begin(), this->shapes.end(), shape);
		if (it != this->shapes.end()) {
			this->shapes.erase(it);
		}
		else {
			ERROR("Layer::removeObject", "shape was not found.");
		}
	}
	else {
		ERROR("Layer::removeObject", "shape was nullptr.");
	}
}

std::vector<BaseShape*>* Layer::getShapes() {
	return &this->shapes;
}
