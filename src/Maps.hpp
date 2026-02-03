#pragma 

enum class MapType;

static std::string mapPathLocation = "res//map_paths//";
std::queue<sf::Vector2f> parseMapPath(MapType type);

class BaseMap : public BaseShape {
public:
	std::queue<sf::Vector2f> path{};

	BaseMap(float xPos, float yPos, float xSize, float ySize, sf::Texture* texture, MapType type);
	BaseMap(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, MapType type);

	void draw(sf::RenderWindow* window) override;
	bool onUpdate() override { return false; }

	void initClass(MapType type);
};