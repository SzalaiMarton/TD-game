#include "stdafx.hpp"

bool Game::wasHolding = false;
BaseShape* Game::beingDraggedShape{};
BaseShape* Game::clickedShape{};
BaseShape* Game::beingHoveredShape{};
BaseMap* Game::currentMap{};
GameConfig Game::config{};
uint16_t Game::currentWaitTime{};
uint16_t Game::timeWaited{};
TargetGroup* Game::currentGroup{};
std::queue<std::pair<uint8_t, TargetType>> Game::waitingTargets{};
GameState Game::currentState = GameState::MENU;

Game::Game() {
	this->config = GameConfig();
}

void Game::init() {
	Assets::loadFonts(Assets::fontsPath);
	Assets::loadTextures(Assets::texturesPath);
	Renderer::init((WindowConfig*)this->config);

	this->initLayers();
	this->initMainMenu();
	this->initInventory();
	this->initInGame();
}

void Game::run() {
	auto w = Renderer::getWindow();
	while (w->isOpen()) {
		while (const std::optional event = w->pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				w->close();
			}

			if (event->is<sf::Event::MouseButtonPressed>()) {
				this->handleClick();
			}

			if (event->is<sf::Event::Resized>()) {
				Renderer::onResize();
			}

			if (event->is<sf::Event::MouseMoved>()) {
				this->handleHover();
			}
		}

		this->update();
		this->handleDrag();
		Renderer::refreshFrame();
	}
}

void Game::update() {
	switch (this->currentState) {
	case GameState::MENU:
		break;
	case GameState::INGAME:
		this->handleInGame();
		break;
	}
}

void Game::handleClick() {
	auto prev = Game::clickedShape;
	auto mP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
	Game::clickedShape = Renderer::getLayer(gameStateToLayerName(Game::currentState))->root->getClicked(mP);

	if (prev && Game::clickedShape != prev && prev->attributes.hasComponent<MIC>()) {
		prev->attributes.getComponent<MIC>()->onClickLoss();
	}

	if (Game::clickedShape) {
		Game::clickedShape->attributes.getComponent<MIC>()->onClick();
	}
}

void Game::handleDrag() {
	bool holding = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	if (!Game::wasHolding && holding) {
		auto mP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
		Game::beingDraggedShape = Renderer::getLayer(gameStateToLayerName(Game::currentState))->root->getClicked(mP);
	}

	if ((Game::wasHolding && holding) || (!Game::wasHolding && holding)) {
		if (Game::beingDraggedShape) {
			Game::beingDraggedShape->attributes.getComponent<MIC>()->onDrag(sf::Mouse::getPosition(*Renderer::getWindow()), Game::beingDraggedShape);
		}
	}
	else {
		Game::beingDraggedShape = nullptr;
	}

	Game::wasHolding = holding;
}

void Game::handleHover() {
	auto mP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
	auto hovered = Renderer::getLayer(gameStateToLayerName(Game::currentState))->root->getClicked(mP);

	if (Game::beingHoveredShape == hovered) {
		return;
	}
	else {
		if (Game::beingHoveredShape) {
			Game::beingHoveredShape->attributes.getComponent<MIC>()->onHoverLoss();
		}
		Game::beingHoveredShape = hovered;
	}

	if (Game::beingHoveredShape && Game::beingHoveredShape->attributes.hasComponent<MIC>()) {
		Game::beingHoveredShape->attributes.getComponent<MIC>()->onHover();
	}
}

void Game::handleInGame() {
	if (!Game::currentGroup) {
		this->initGroup();
	}

	if (Game::timeWaited >= Game::currentWaitTime) {
		if (Game::currentGroup && Game::currentGroup->targets.size() > 0) {
			Game::currentGroup->spawnNext();
			Game::timeWaited = 0;
		}
		else {
			Game::currentGroup = nullptr;
		}
	}
	for (auto& e : Renderer::getLayer(MainLayerName::INGAME)->shapes) {
		if (dynamic_cast<Target*>(e)) {
			dynamic_cast<Target*>(e)->onUpdate();
		}
		else if (dynamic_cast<BaseCat*>(e)) {
			dynamic_cast<BaseCat*>(e)->onUpdate();
		}
	}
	if (currentGroup) {
		Game::timeWaited++;
	}
}

void Game::initMainMenu() {
	auto layer = Renderer::getLayer(MainLayerName::MAINMENU);

	auto obj = dynamic_cast<Button*>(layer->addShape(new Button(400, 20, 200, 100, Assets::getTexture("default_texture.png"), "Play")));
	obj->addClickHanlder([]() { Renderer::getLayer(MainLayerName::INGAME)->showLayer(); Renderer::getLayer(MainLayerName::MAINMENU)->hideLayer(); Game::currentState = GameState::INGAME; });
	obj->attributes.getComponent<MIC>()->disableDrag();

	obj = dynamic_cast<Button*>(layer->addShape(new Button(400, 140, 200, 100, Assets::getTexture("default_texture.png"), "Inventory")));
	obj->addClickHanlder([]() { Renderer::getLayer(MainLayerName::INVENTORY)->showLayer(); Renderer::getLayer(MainLayerName::MAINMENU)->hideLayer(); });
	obj->attributes.getComponent<MIC>()->disableDrag();

	obj = dynamic_cast<Button*>(layer->addShape(new Button(400, (float)Renderer::getWindow()->getSize().y - 120, 200, 100, Assets::getTexture("default_texture.png"), "Exit")));
	obj->addClickHanlder([]() { Renderer::getWindow()->close(); });
	obj->attributes.getComponent<MIC>()->disableDrag();
}

void Game::initLayers() {
	auto layer = new Layer();
	layer->showLayer();
	Renderer::pushLayer(layer, MainLayerName::MAINMENU);

	layer = new Layer();
	Renderer::pushLayer(layer, MainLayerName::PAUSEMENU);

	layer = new Layer();
	Renderer::pushLayer(layer, MainLayerName::INVENTORY);

	layer = new Layer();
	Renderer::pushLayer(layer, MainLayerName::PREGAME);

	layer = new Layer();
	Renderer::pushLayer(layer, MainLayerName::BACKGROUNDSCENE);

	layer = new Layer();
	Renderer::pushLayer(layer, MainLayerName::INGAME);
}

void Game::initInventory() {
	int idx = 0;
	float startingX = 30;
	float startingY = (float)Renderer::getWindow()->getSize().y / 2;
	float gap = 450;
	auto layer = Renderer::getLayer(MainLayerName::INVENTORY);

	for (auto& e : this->config.availableCats) {
		layer->addShape(new InvetoryCard(startingX + (gap * idx), startingY, 300.f, (float)Renderer::getWindow()->getSize().y - 200.f, e));
		idx++;
	}
}

void Game::initInGame() {
	auto layer = Renderer::getLayer(MainLayerName::INGAME);

	Game::currentMap = dynamic_cast<BaseMap*>(layer->addShape(new BaseMap(0.f, 0.f, Renderer::getWindow()->getSize().x, Renderer::getWindow()->getSize().y , Assets::getTexture("place_holder_map.png"), MapType::PLACEHOLDER)));
	this->queueTarget(20, TargetType::BASIC);
	layer->addShape(new BlackGreyCat(400, 400, 60, 40));
}

void Game::queueTarget(uint8_t amount, TargetType type) {
	waitingTargets.push({ amount, type });
}

void Game::initGroup() {
	if (waitingTargets.size() > 0) {
		auto tg = new TargetGroup(waitingTargets.front().first, waitingTargets.front().second);
		auto res = tg->initSpawn();
		Game::currentWaitTime = res.first * 5;
		Game::currentGroup = res.second;
		waitingTargets.pop();
	}
}

GameConfig::GameConfig() {
	this->availableCats = { CatType::BLACKGREY, CatType::ORANGE };
}
