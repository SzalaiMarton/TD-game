#pragma once

class Attribute;
struct Shape;
struct WindowConfig;
enum class MainLayerName;

class Layer {
public:
	bool isHidden{true};
	std::vector<BaseShape*> shapes{};
	QuadTree* root;

	Layer(float xPos, float yPos, float xSize, float ySize);
	~Layer();

	void showLayer();
	void hideLayer();
	bool getIsHidden();
	BaseShape* addShape(BaseShape* object, bool dontAddToTree = false);
	void removeShape(BaseShape* object);
	std::vector<BaseShape*>* getShapes();
};

class Renderer {
public:
	static sf::RenderWindow* window;
	static uint8_t nextFreeIndex;
	static sf::View* view;
	static std::map<MainLayerName, Layer*> layers;

	static void init(WindowConfig* config);
	static void pushLayer(Layer* layer, MainLayerName index);
	static void removeLayer(MainLayerName index, Layer* layer);
	static void refreshFrame();
	static void onResize();
	static Layer* getLayer(MainLayerName index);
	static Layer* getCurrentLayer();
	static sf::RenderWindow* getWindow();
};