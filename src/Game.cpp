#include "stdafx.hpp"

bool Game::wasHolding = false;
bool Game::shuttingdown = false;
bool Game::placingEffectPlayed = false;
TargetGroup* Game::currentGroup{};
GameConfig Game::config{};
BaseShape* Game::beingDraggedShape{};
BaseShape* Game::clickedShape{};
BaseShape* Game::beingHoveredShape{};
GameState Game::currentState = GameState::MAINMENU;
BaseMap* Game::currentMap{};
uint16_t Game::currentWaitTime{};
uint16_t Game::timeWaited{};
uint16_t Game::money{};
Layer* Game::currentLayer{};
std::queue<std::pair<uint8_t, TargetType>> Game::waitingTargets{};
std::vector<CatType> Game::selectedCats{};
std::vector<TargetType> Game::availableTargets{};
std::queue<BaseShape*> Game::toBeDeletedShapes{};

Game::Game() {
	this->config = GameConfig();
}

void Game::init() {
	Assets::loadFonts(Assets::fontsPath);
	Assets::loadTextures(Assets::texturesPath);
	Renderer::init((WindowConfig*)config);

	initLayers();
	initMainMenu();
	initInventory();
	initInGame();
	Game::currentLayer = Renderer::getLayer(MainLayerName::MAINMENU);
}

void Game::run() {
	auto w = Renderer::getWindow();
	while (w->isOpen()) {
		while (const std::optional event = w->pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				w->close();
			}

			if (event->is<sf::Event::MouseButtonPressed>()) {
				handleClick();
				if (Game::shuttingdown) {
					break;
				}
			}

			if (event->is<sf::Event::Resized>()) {
				Renderer::onResize();
			}

			if (event->is<sf::Event::MouseMoved>()) {
				handleHover();
			}
		}

		if (Game::shuttingdown) {
			break;
		}

		#if DEBUG_TREE == 1
		Renderer::getCurrentLayer()->root->draw(Renderer::getCurrentLayer());
		#endif

		handleDrag();
		update();
		Renderer::refreshFrame();
	}
}

void Game::shutdown() {
	Renderer::getWindow()->close();
	delete Button::buttonFont;
	Button::buttonFont = nullptr;
	//delete InvetoryCard::cardFont;
	InvetoryCard::cardFont = nullptr;
	delete Renderer::view;
	Renderer::view = nullptr;
	delete Renderer::window;
	Renderer::window = nullptr;
	for (auto it = Renderer::layers.begin(); it != Renderer::layers.end();) {
		delete it->second;
		it = Renderer::layers.erase(it);
	}
	Renderer::layers.clear();
	Game::currentLayer = nullptr;
	Game::beingDraggedShape = nullptr;
	Game::clickedShape = nullptr;
	Game::beingHoveredShape = nullptr;
	Game::currentMap = nullptr;
	Game::shuttingdown = true;
}

void Game::update() {
	switch (currentState) {
	case GameState::MAINMENU:
		break;
	case GameState::INGAME:
		handleInGame();
		break;
	}
}

void Game::handleClick() {
	auto prev = Game::clickedShape;
	auto mP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
	Game::clickedShape = Renderer::getCurrentLayer()->root->getClicked(mP);
	
	//debug
	//LOG(mP.x << " " << mP.y);
	//debug

	#if DEBUG_TREE == 1
	auto ttt = Renderer::getCurrentLayer()->root->getClickedTree(mP);
	LOG(ttt << " " << ttt->elements.size());
	#endif

	//debug
	//auto tt = Renderer::getCurrentLayer()->root->getClicked(mP);
	//LOG(tt);
	//debug


	if (prev && Game::clickedShape != prev && prev->attributes->hasComponent<MIC>()) {
		prev->getMIC()->onClickLoss();
	}

	if (Game::clickedShape) {
		Game::clickedShape->getMIC()->onClick();
	}
}

void Game::handleDrag() {
	bool holding = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	if (!Game::wasHolding && holding) {
		auto mP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
		auto layer = Game::currentLayer;
		if (layer) {
			Game::beingDraggedShape = layer->root->getClicked(mP);
		}
	}

	if ((Game::wasHolding && holding) || (!Game::wasHolding && holding)) {
		if (Game::beingDraggedShape) {
			Game::beingDraggedShape->getMIC()->onDrag(sf::Mouse::getPosition(*Renderer::getWindow()), Game::beingDraggedShape);
		}
	}
	else {
		if (Game::beingDraggedShape) {
			Game::beingDraggedShape->updateTree();
			Game::beingDraggedShape->getMIC()->onDragLoss();
		}
		Game::beingDraggedShape = nullptr;
	}

	Game::wasHolding = holding;
}

void Game::handleHover() {
	auto mP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
	auto hovered = Renderer::getCurrentLayer()->root->getClicked(mP);
	
	if (Game::beingHoveredShape == hovered) {
		return;
	}
	else {
		if (Game::beingHoveredShape) {
			Game::beingHoveredShape->getMIC()->onHoverLoss();
		}
		Game::beingHoveredShape = hovered;
	}

	if (Game::beingHoveredShape && Game::beingHoveredShape->attributes->hasComponent<MIC>()) {
		Game::beingHoveredShape->getMIC()->onHover();
	}
}

void Game::handleInGame() {
	if (!Game::currentGroup) {
		initGroup();
	}

	if (Game::timeWaited >= Game::currentWaitTime) {
		if (Game::currentGroup && Game::currentGroup->targets.size() > 0) {
			Game::currentGroup->spawnNext(Game::currentLayer);
			Game::timeWaited = 0;
		}
		else {
			Game::currentGroup = nullptr;
		}
	}

	size_t i{};
	auto& shapes = Game::currentLayer->shapes;
	while (i < shapes.size()) {
		shapes[i]->onUpdate();

		size_t eIT{};
		while (eIT < Game::toBeDeletedShapes.size()) {
			auto t = Game::toBeDeletedShapes.front();
			Game::toBeDeletedShapes.pop();
			delete t;
		}

		i += 1;
	}

	if (currentGroup) {
		Game::timeWaited++;
	}
}

void Game::initMainMenu() {
	auto layer = Renderer::getLayer(MainLayerName::MAINMENU);

	auto obj = dynamic_cast<Button*>(layer->addShape(new Button(0.5f, 100.f, 200.f, 100.f, Assets::getTexture("default_texture.png"), "Play")));
	obj->addClickHanlder([]() { switchLayer(Game::currentState, GameState::INGAME); });
	obj->getMIC()->disableDrag();

	obj = dynamic_cast<Button*>(layer->addShape(new Button(0.5f, 280.f, 200.f, 100.f, Assets::getTexture("default_texture.png"), "Inventory")));
	obj->addClickHanlder([]() { switchLayer(Game::currentState, GameState::INVENTORY); });
	obj->getMIC()->disableDrag();

	obj = dynamic_cast<Button*>(layer->addShape(new Button(0.5f, (float)Renderer::getWindow()->getSize().y - 120.f, 200.f, 100.f, Assets::getTexture("default_texture.png"), "Exit")));
	obj->addClickHanlder([]() { shutdown(); });
	obj->getMIC()->disableDrag();
}

void Game::initLayers() {
	float layerPosX = 0.f, layerPosY = 0.f;
	auto layerSize = (sf::Vector2f)Renderer::getWindow()->getSize();

	auto layer = new Layer(layerPosX, layerPosY, layerSize.x, layerSize.y);
	layer->showLayer();
	Renderer::pushLayer(layer, MainLayerName::MAINMENU);

	layer = new Layer(layerPosX, layerPosY, layerSize.x, layerSize.y);
	Renderer::pushLayer(layer, MainLayerName::PAUSEMENU);

	layer = new Layer(layerPosX, layerPosY, layerSize.x, layerSize.y);
	Renderer::pushLayer(layer, MainLayerName::INVENTORY);

	layer = new Layer(layerPosX, layerPosY, layerSize.x, layerSize.y);
	Renderer::pushLayer(layer, MainLayerName::PREGAME);

	layer = new Layer(layerPosX, layerPosY, layerSize.x, layerSize.y);
	Renderer::pushLayer(layer, MainLayerName::BACKGROUNDSCENE);

	layer = new Layer(layerPosX, layerPosY, layerSize.x, layerSize.y);
	Renderer::pushLayer(layer, MainLayerName::INGAME);
}

void Game::initInventory() {
	int idx = 0;
	float startingX = 30;
	float startingY = (float)Renderer::getWindow()->getSize().y / 2;
	float gap = 450;
	auto layer = Renderer::getLayer(MainLayerName::INVENTORY);

	for (auto& e : config.availableCats) {
		layer->addShape(new InvetoryCard(startingX + (gap * idx), startingY, 300.f, (float)Renderer::getWindow()->getSize().y - 200.f, e));
		idx++;
	}

	auto backButton = dynamic_cast<Button*>(layer->addShape(new Button(100, 500, 200, 100, Assets::getTexture("default_texture.png"), "Back")));
	backButton->addClickHanlder([]() {switchLayer(Game::currentState, GameState::MAINMENU); });
	backButton->getMIC()->disableDrag();
}

void Game::initInGame() {
	auto layer = Renderer::getLayer(MainLayerName::INGAME);
	Game::selectedCats = {CatType::ORANGE, CatType::BLACKGREY, CatType::ORANGE};

	Game::currentMap = dynamic_cast<BaseMap*>(layer->addShape(new BaseMap(0.f, 0.f, 1.f, 1.f, MapType::ROADMAP), true));
	
	layer->addShape(new InGameCatCardDesk(0, 600, 1100, 100, layer), true);
	layer->addShape(new InGameTargetCardDesk(1000, 0, 100, 800, layer), true);

	Game::queueTarget(10, TargetType::BASIC);

	auto backButton = dynamic_cast<Button*>(layer->addShape(new Button(100, 500, 200, 100, Assets::getTexture("default_texture.png"), "Back")));
	backButton->addClickHanlder([]() {switchLayer(Game::currentState, GameState::MAINMENU); });
	backButton->getMIC()->disableDrag();
}

void Game::isPlacementValid(BaseCat* shape) {
	auto mapSize = Game::currentMap->maskMap->getSize();
	auto mousePos = (sf::Vector2u)sf::Mouse::getPosition(*Renderer::getWindow());

	for (auto& e : Game::currentLayer->shapes) {
		if (dynamic_cast<BaseCat*>(e) && shape != e) {
			auto sp = dynamic_cast<BaseCat*>(e)->getSC()->sprite;
			if (sp->getGlobalBounds().contains((sf::Vector2f)mousePos)) {
				shape->disablePlace();
				return;
			}
		}
	}
	
	sf::Vector2u topLeft = { mousePos.x - Game::config.validPlacementOffset, mousePos.y - Game::config.validPlacementOffset };
	sf::Vector2u topRight = { mousePos.x + Game::config.validPlacementOffset, mousePos.y - Game::config.validPlacementOffset };
	sf::Vector2u botLeft = { mousePos.x - Game::config.validPlacementOffset, mousePos.y + Game::config.validPlacementOffset };
	sf::Vector2u botRight = { mousePos.x + Game::config.validPlacementOffset, mousePos.y + Game::config.validPlacementOffset };

	if (!(topLeft.x > 0 && topLeft.y > 0 && botRight.x < mapSize.x && botRight.y < mapSize.y)) {
		shape->disablePlace();
		return;
	}

	auto topLeftp = Game::currentMap->maskMap->getPixel(topLeft);
	auto topRightp = Game::currentMap->maskMap->getPixel(topRight);
	auto botLeftp = Game::currentMap->maskMap->getPixel(botLeft);
	auto botRightp = Game::currentMap->maskMap->getPixel(botRight);
	if (topLeftp == topRightp && topLeftp == botLeftp && topLeftp == botRightp && topLeftp == Game::currentMap->validPlacementColor) {
		shape->enablePlace();
	}
	else {
		shape->disablePlace();
	}
}

void Game::switchLayer(GameState from, GameState to) {
	Renderer::getLayer(gameStateToLayerName(from))->hideLayer();
	Game::currentLayer = Renderer::getLayer(gameStateToLayerName(to));
	Game::currentLayer->showLayer();
	Game::currentState = to;
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
