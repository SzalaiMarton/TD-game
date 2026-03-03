#pragma 

enum class MapType;
struct MapConfig;

static std::string mapPathLocation = "res//map_paths//";
MapConfig parseMapProperties(MapType type);
std::queue<sf::Vector2f> parsePath(std::istream& file);
sf::Vector2f parseSize(std::istream& file);
sf::Color parseValidColor(std::istream& file);
sf::Color parseInvalidColor(std::istream& file);
static std::string terminatorChar = "-";

struct MapConfig {
	sf::Color invalidColor{};
	sf::Color validColor{};
	sf::Vector2f mapSize{};
	std::queue<sf::Vector2f> path{};

	MapConfig() = default;

	void print() {
		LOG("size: " << mapSize.x << " " << mapSize.y);
		//LOG("validColor: " << (std::string)validColor);
		//LOG("invalidColor: " << (std::string)invalidColor);
		LOG("path:");
		auto& temp = path;
		while (!temp.empty()) {
			LOG(temp.front().x << " " << temp.front().y);
			temp.pop();
		}
	}
};

class BaseMap : public BaseShape {
public:
	std::queue<sf::Vector2f> path{};
	sf::Image* maskMap{};
	Shape* maskShape{};
	sf::Color validPlacementColor{};
	sf::Color invalidPlacementColor{};
	sf::Vector2f mapSize{};

	BaseMap(float xPos, float yPos, float xSize, float ySize, MapType type);
	BaseMap(const sf::Vector2f& pos, const sf::Vector2f& size, MapType type);

	void draw(sf::RenderWindow* window) override;
	void onUpdate() override {  }

	void initClass(MapType type);
};