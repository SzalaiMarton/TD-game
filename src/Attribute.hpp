#pragma once

class Attribute;
struct BaseShape;

struct ObjectComponent {
	virtual ~ObjectComponent() = default;
	virtual ObjectComponent* copy() const = 0;
};



struct MouseInteractionComponent : public ObjectComponent {
	struct SingleHandler {
		HandlerType type;
		std::function<void()> handler;

		SingleHandler(HandlerType t, std::function<void()> h) : type(t), handler(h) {}
	};

	// handler disablers
	bool isClickable{true};
	bool isHoverable{true};
	bool isDraggable{true};
	bool isBaseDrag{false};

	std::unordered_map<HandlerType, std::function<void()>> handlers;

	MouseInteractionComponent(std::initializer_list<SingleHandler> handlers);
	MouseInteractionComponent(const MouseInteractionComponent& original);
	MouseInteractionComponent* copy() const override;

	void bindClickHandler(std::function<void()> onC);
	void bindHoverHandler(std::function<void()> onH);
	void bindDragHandler(std::function<void()> onD);
	void bindHoverLossHandler(std::function<void()> onLF);
	void bindDragLoss(std::function<void()> onDL);

	void enableClick();
	void enableDrag();
	void enableHover();
	void enableBaseDrag();

	void disableClick();
	void disableDrag();
	void disableHover();
	void disableBaseDrag();

	bool isHovered(sf::Vector2i& pMouse, sf::FloatRect& bounds) const;
	void onHover() const;
	void onClick() const;
	void onHoverLoss() const;
	void onClickLoss() const;
	void onDrag(const sf::Vector2i& pMouse, BaseShape* shape) const;
	void onDragLoss() const;
};
using MIC = MouseInteractionComponent;

struct SpriteComponent : public ObjectComponent {
	sf::Sprite* sprite{};
	sf::Vector2f originalTextureSize{};

	SpriteComponent(sf::Texture* texture);
	SpriteComponent(const SpriteComponent& original);
	~SpriteComponent();
	SpriteComponent* copy() const override;

	void setSize(float x, float y);
	void setSize(const sf::Vector2f& size);
	void setPos(float x, float y);
	void setPos(sf::Vector2f pos);
	sf::Vector2f getPos() const;
};




class AttributeBuilder {
private:
	std::unordered_map<std::type_index, ObjectComponent*> components{};

public:
	AttributeBuilder() {}

	AttributeBuilder& withTexture(sf::Texture* texture);
	AttributeBuilder& withMouseInteraction(std::initializer_list<MIC::SingleHandler> handlers);

	Attribute* build() const;
};

class Attribute {
private:
	std::unordered_map<std::type_index, ObjectComponent*> components{};

public:
	Attribute() {}
	Attribute(const Attribute& original);
	
	void overrideComponents(const std::unordered_map<std::type_index, ObjectComponent*>& components);

	template<typename T>
	void addComponent(T* comp) {
		this->components[std::type_index(typeid(T))] = comp;
	}

	template<typename T>
	bool hasComponent() {
		return this->components.find(std::type_index(typeid(T))) != this->components.end();
	}

	template<typename T>
	T* getComponent() {
		auto it = components.find(std::type_index(typeid(T)));
		return it != components.end() ? dynamic_cast<T*>(it->second) : nullptr;
	}
};