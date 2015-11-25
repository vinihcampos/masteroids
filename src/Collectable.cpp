#include "Collectable.h"
#include "Universe.h"
#include "CollisionTools.h"
#include "MathVector.h"

Collectable::Collectable(Collectable::CollectableType _type, MathVector & initialPos, Universe & _universe) : PhysicalEntity(_universe), type {_type} {
	position = initialPos;
	MathVector::Point upperLeft {10,10};
	double spriteWidth = 20;
	double spriteHeigth = 20;
	upperLeft.x = spriteWidth * _type + 10; upperLeft.y = spriteHeigth * _type + 10;
	sprite.setTexture(Configuration::textures.get(Configuration::Textures::Collectables));	
	//sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
	sprite.setTextureRect(sf::IntRect(upperLeft.x, upperLeft.y, spriteWidth, spriteHeigth));
}

void Collectable::update(sf::Time deltaTime) {
	sprite.setPosition(position.horizontal, position.vertical);
}

bool Collectable::isColliding(const PhysicalEntity & other) const {
	if (dynamic_cast<const Player*>(&other) != nullptr) {
		if (CollisionTools::circleCollision(*this, other)) {
			return true;
		}
	}
	return false;
}

void Collectable::onCollide(PhysicalEntity & other) {
	if (dynamic_cast<const Player*>(&other) != nullptr) 
		alive = false;
}

bool Collectable::isClosing(const PhysicalEntity & other) const { 
	return false;  
}

void Collectable::onClose(PhysicalEntity & other) {
	return;
}
