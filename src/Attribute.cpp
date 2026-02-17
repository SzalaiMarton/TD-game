#include "stdafx.hpp"


AttributeBuilder& AttributeBuilder::withTexture(sf::Texture* texture) {
	if (texture) {
		this->components[std::type_index(typeid(SpriteComponent))] = new SpriteComponent(texture);
	}
	else {
		ERROR("ObjectBuilder::withTexture", "Texture was nullptr.");
	}
	return *this;
}

AttributeBuilder& AttributeBuilder::withMouseInteraction(std::initializer_list<MIC::SingleHandler> handlers) {
	this->components[std::type_index(typeid(MIC))] = new MIC(handlers);
	return *this;
}

Attribute* AttributeBuilder::build() const {
	auto resObject = new Attribute();
	resObject->overrideComponents(this->components);

	if (resObject->hasComponent<SpriteComponent>() && resObject->hasComponent<MIC>()) {
		resObject->getComponent<SpriteComponent>()->sprite->setOrigin({ resObject->getComponent<SpriteComponent>()->sprite->getLocalBounds().size.x / 2, resObject->getComponent<SpriteComponent>()->sprite->getLocalBounds().size.y / 2 });
	}
	
	return resObject;
}

SpriteComponent::SpriteComponent(sf::Texture* texture) {
	if (!texture) {
		ERROR("SpriteComponent::SpriteComponent", "texture was nullptr, setting default texture");
		texture = Assets::getTexture("default_texture.png");
	}
	this->sprite = new sf::Sprite(*texture);
	this->originalTextureSize = (sf::Vector2f)texture->getSize();
}

SpriteComponent::SpriteComponent(const SpriteComponent& original) {
	this->sprite = new sf::Sprite(original.sprite->getTexture());
	this->originalTextureSize = original.originalTextureSize;
}

SpriteComponent::~SpriteComponent() {
	delete sprite;
	sprite = nullptr;
}

SpriteComponent* SpriteComponent::copy() const {
	return new SpriteComponent(*this);
}

void SpriteComponent::setSize(float x, float y) {
	this->sprite->setScale({ (x / this->originalTextureSize.x), (y / this->originalTextureSize.y) });
}

void SpriteComponent::setSize(const sf::Vector2f& size) {
	this->sprite->setScale({ (size.x / this->originalTextureSize.x), (size.y / this->originalTextureSize.y) });
}

void SpriteComponent::setPos(float x, float y) {
	if (x < 1) {
		x = Renderer::getWindow()->getSize().x * x;
	}
	if (y < 1) {
		y = Renderer::getWindow()->getSize().y * y;
	}
	this->sprite->setPosition({ x, y });
}

void SpriteComponent::setPos(sf::Vector2f pos) {
	if (pos.x < 1) {
		pos.x = Renderer::getWindow()->getSize().x * pos.x;
	}
	if (pos.y < 1) {
		pos.y = Renderer::getWindow()->getSize().y * pos.y;
	}
	this->sprite->setPosition(pos);
}

sf::Vector2f SpriteComponent::getPos() const {
	return this->sprite->getPosition();
}

Attribute::Attribute(const Attribute& original) {
	this->overrideComponents(original.components);
}

void Attribute::overrideComponents(const std::unordered_map<std::type_index, ObjectComponent*>& components) {
	for (auto& comp : components) {
		this->components.insert({ comp.first, comp.second->copy()});
	}
}

MouseInteractionComponent::MouseInteractionComponent(std::initializer_list<SingleHandler> handlers) {
	for (auto& e : handlers) {
		this->handlers.insert({ e.type, e.handler});
	}
}

MouseInteractionComponent::MouseInteractionComponent(const MouseInteractionComponent& original) {
	for (auto& e : original.handlers) {
		this->handlers.insert({ e.first, std::function<void()>(e.second)});
	}
}

MouseInteractionComponent* MouseInteractionComponent::copy() const {
	return new MouseInteractionComponent(*this);
}

void MIC::bindClickHandler(std::function<void()> onC) {
	this->handlers.insert({HandlerType::CLICKHANDLER, onC});
}

void MIC::bindHoverHandler(std::function<void()> onH) {
	this->handlers.insert({ HandlerType::HOVERHANDLER, onH });
}

void MIC::bindDragHandler(std::function<void()> onD) {
	this->handlers.insert({ HandlerType::DRAGHANDLER, onD });
}

void MIC::bindHoverLossHandler(std::function<void()> onHL) {
	this->handlers.insert({ HandlerType::HOVERLOSSHANDLER, onHL });
}

void MIC::bindDragLoss(std::function<void()> onDL) {
	this->handlers.insert({ HandlerType::DRAGLOSSHANDLER, onDL });
}

void MouseInteractionComponent::enableClick() {
	this->isClickable = true;
}

void MouseInteractionComponent::enableDrag() {
	this->isDraggable = true;
}

void MouseInteractionComponent::enableHover() {
	this->isHoverable = true;
}

void MouseInteractionComponent::enableBaseDrag() {
	this->isBaseDrag = true;
}

void MouseInteractionComponent::disableBaseDrag() {
	this->isBaseDrag = false;
}

void MouseInteractionComponent::disableClick() {
	this->isClickable = false;
}

void MouseInteractionComponent::disableDrag() {
	this->isDraggable = false;
}

void MouseInteractionComponent::disableHover() {
	this->isHoverable = false;
}

bool MIC::isHovered(sf::Vector2i& pMouse, sf::FloatRect& bounds) const {
	return bounds.position.x < pMouse.x && bounds.position.y < pMouse.y && bounds.position.x + bounds.size.x >= pMouse.x && bounds.position.y + bounds.size.y >= pMouse.y;
}

void MIC::onHover() const {
	if (this->handlers.find(HandlerType::HOVERHANDLER) != this->handlers.end() && this->isHoverable) {
		this->handlers.at(HandlerType::HOVERHANDLER)();
	}
	else {
		ERROR("MouseInteractionComponent::onHover", "Handler was nullptr.");
	}
}

void MIC::onClick() const {
	if (this->handlers.find(HandlerType::CLICKHANDLER) != this->handlers.end() && this->isClickable) {
		this->handlers.at(HandlerType::CLICKHANDLER)();
	}
	else {
		ERROR("MouseInteractionComponent::onClick", "Handler was nullptr.");
	}
}

void MouseInteractionComponent::onHoverLoss() const {
	if (this->handlers.find(HandlerType::HOVERLOSSHANDLER) != this->handlers.end() && this->isHoverable) {
		this->handlers.at(HandlerType::HOVERLOSSHANDLER)();
	}
	else {
		ERROR("MouseInteractionComponent::onFocusLoss", "Handler was nullptr.");
	}
}

void MouseInteractionComponent::onClickLoss() const {
	if (this->handlers.find(HandlerType::CLICKLOSSHANDLER) != this->handlers.end() && this->isClickable) {
		this->handlers.at(HandlerType::CLICKLOSSHANDLER)();
	}
	else {
		ERROR("MouseInteractionComponent::onClickLoss", "Handler was nullptr.");
	}
}

void MIC::onDrag(const sf::Vector2i& pMouse, BaseShape* shape) const {
	if (this->handlers.find(HandlerType::DRAGHANDLER) != this->handlers.end() && this->isDraggable) {
		this->handlers.at(HandlerType::DRAGHANDLER)();
		if (isBaseDrag) {
			shape->attributes.getComponent<SpriteComponent>()->sprite->setPosition({ (float)pMouse.x ,(float)pMouse.y });
		}
	}
}

void MouseInteractionComponent::onDragLoss() const {
	if (this->handlers.find(HandlerType::DRAGLOSSHANDLER) != this->handlers.end()) {
		this->handlers.at(HandlerType::DRAGLOSSHANDLER)();
	}
	else {
		ERROR("MouseInteractionComponent::onDragLoss", "Handler was nullptr.");
	}
}
