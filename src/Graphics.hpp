#pragma once

class Attribute;
struct Shape;
struct WindowConfig;
enum class MainLayerName;


class Layer {
public:
	std::vector<BaseShape*> shapes{};
	bool isHidden{true};
	QuadTree* root;

	Layer(float xPos, float yPos, float xSize, float ySize);

	void showLayer();
	void hideLayer();
	bool getIsHidden();
	BaseShape* addShape(BaseShape* object, bool dontAddToTree = false);
	void removeShape(BaseShape* object);
	std::vector<BaseShape*>* getShapes();
};

class Renderer {
private:
	static sf::RenderWindow* window;
	static std::map<MainLayerName, Layer*> layers;
	static uint8_t nextFreeIndex;
	static sf::View* view;
	Renderer();

public:
	static void init(WindowConfig* config);
	static sf::RenderWindow* getWindow();
	static void pushLayer(Layer* layer, MainLayerName index);
	static void removeLayer(MainLayerName index, Layer* layer);
	static void refreshFrame();
	static void onResize();
	static Layer* getLayer(MainLayerName index);
	static Layer* getCurrentLayer();
};