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
uint16_t Game::targetSpawnDelay{};
uint16_t Game::targetSpawnTimeElapsed{};
uint16_t Game::money{};
Layer* Game::currentLayer{};
std::queue<Stats::TargetGroupStats*> Game::waitingTargets{};
std::vector<CatType> Game::selectedCats{};
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
#if DEBUG_KEYS == 1
	bool isPaused = false;
	bool jump = false;
#endif

	auto w = Renderer::getWindow();
	while (w->isOpen()) {
		while (const std::optional event = w->pollEvent()) {

#if DEBUG_KEYS == 1
			jump = false;
			if (event->is<sf::Event::KeyPressed>()) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) { // PAUSE
					isPaused = !isPaused;
				}
				if (isPaused && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) { // STEP FRAME
					jump = true;
					break;
				}
			}
			if (isPaused && !jump) { // SKIP UPDATE
				continue;
			}
#endif


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
#if DEBUG_KEYS == 1
		if (isPaused && !jump) { // SKIP UPDATE
			continue;
		}
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

	if (Game::targetSpawnTimeElapsed >= Game::targetSpawnDelay) {
		if (Game::currentGroup && Game::currentGroup->targets.size() > 0) {
			Game::currentGroup->spawnNext();
			Game::targetSpawnTimeElapsed = 0;
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
		Game::targetSpawnTimeElapsed++;
	}
}

void Game::initMainMenu() {
	auto layer = Renderer::getLayer(MainLayerName::MAINMENU);

	auto obj = dynamic_cast<Button*>(layer->addShape(new Button(0.5f, 100.f, 200.f, 100.f, Assets::getTexture(TextureName::defalutTexture), "Play")));
	obj->addClickHanlder([]() { switchLayer(Game::currentState, GameState::INGAME); });
	obj->getMIC()->disableDrag();

	obj = dynamic_cast<Button*>(layer->addShape(new Button(0.5f, 280.f, 200.f, 100.f, Assets::getTexture(TextureName::defalutTexture), "Inventory")));
	obj->addClickHanlder([]() { switchLayer(Game::currentState, GameState::INVENTORY); });
	obj->getMIC()->disableDrag();

	obj = dynamic_cast<Button*>(layer->addShape(new Button(0.5f, (float)Renderer::getWindow()->getSize().y - 120.f, 200.f, 100.f, Assets::getTexture(TextureName::defalutTexture), "Exit")));
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
	auto layer = Renderer::getLayer(MainLayerName::INVENTORY);

	auto showcase = dynamic_cast<ShowCase*>(layer->addShape(new ShowCase(0, 0, 1100, 700, false, false, false, layer)));
	showcase->add(new InvetoryCard(0, 0, 200, 400, CatType::BLACKGREY));
	showcase->add(new InvetoryCard(0, 0, 200, 400, CatType::ORANGE));

	auto backButton = dynamic_cast<Button*>(layer->addShape(new Button(100, 500, 200, 100, Assets::getTexture(TextureName::defalutTexture), "Back")));
	backButton->addClickHanlder([]() {switchLayer(Game::currentState, GameState::MAINMENU); });
	backButton->getMIC()->disableDrag();
}

void Game::initInGame() {
	auto layer = Renderer::getLayer(MainLayerName::INGAME);
	Game::selectedCats = {CatType::ORANGE, CatType::BLACKGREY, CatType::ORANGE};

	Game::currentMap = dynamic_cast<BaseMap*>(layer->addShape(new BaseMap(0.f, 0.f, 1.f, 1.f, MapType::ROADMAP), true));
	
	layer->addShape(new InGameCatCardDesk(0, 600, 1100, 100, layer), true);
	layer->addShape(new InGameTargetCardDesk(1000, 0, 100, 800, layer), true);

	auto backButton = dynamic_cast<Button*>(layer->addShape(new Button(100, 500, 200, 100, Assets::getTexture(TextureName::defalutTexture), "Back")));
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

void Game::queueTargetGroup(TargetGroupType type) {
	waitingTargets.push(Stats::getStatByType(type));
}

void Game::initGroup() {
	if (waitingTargets.size() > 0) {
		auto tg = new TargetGroup(waitingTargets.front());
		auto res = tg->initSpawn();
		Game::targetSpawnDelay = res.first;
		Game::currentGroup = res.second;
		waitingTargets.pop();
	}
}

GameConfig::GameConfig() {
	this->availableCats = { 
		CatType::BLACKGREY, 
		CatType::ORANGE 
	};
	this->availableTargetGroups = {
		TargetGroupType::BEATEN_8
	};
}
