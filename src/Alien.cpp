#include "Alien.h"
#include "MathVector.h"
#include "Universe.h"
#include "BulletAlien.h"
#include "BulletShip.h"
#include "PhysicalEntity.h"
#include "CollisionTools.h"
#include "Collectable.h"
#include "Configuration.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

Alien::Alien(MathVector _position, Universe & _universe, int _type, MathVector _velocity, int _lifePoints) : Enemy(_universe) {
	alive = true;
	isFollowing = false;	
	// Setting position
	position = _position;
	// Setting velocity;
	velocity = _velocity;
	//velocity.horizontal = _velocity.horizontal;
	//velocity.vertical = _velocity.vertical;
	// Setting angle
	angleVelocity = 0;
	// Setting the type of Alien
	type = _type;
	radius = Configuration::WINDOW_WIDTH;
	angle.horizontal = 1;
	angle.vertical = 1;
	switch(type){
		case Type::CLASSIC:
			sprite.setTexture(Configuration::textures.get(Configuration::Textures::ClassicAlien));
			currentLifePoints = maxLifePoints = 5;
			break;
		case Type::SHOOTER:
			sprite.setTexture(Configuration::textures.get(Configuration::Textures::ShooterAlien));
			currentLifePoints = maxLifePoints = 10;
			break;
		default:
			sprite.setTexture(Configuration::textures.get(Configuration::Textures::ClassicAlien));
			currentLifePoints = maxLifePoints = 5;
	}

	// Setting movement direction
	sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
	sprite.setRotation(0.0);
	timeLastShot = sf::Time::Zero;
}

void Alien::update(sf::Time deltaTime) {
	if (isFollowing) {
		double _angle = std::atan2(toFollow->getPosition().vertical - position.vertical,
							   toFollow->getPosition().horizontal - position.horizontal);
		sprite.setRotation(180 * _angle / M_PI);
	}

	if(timeLastShot > sf::seconds(1)){
		timeLastShot = sf::Time::Zero;
		shot();
	}

	timeLastShot += deltaTime;
	// Updating position
	position.horizontal += velocity.horizontal; //* angle.horizontal; //% Configuration::WINDOW_WIDTH;// * seconds;
	position.vertical += velocity.vertical; //* angle.vertical; //% Configuration::WINDOW_HEIGHT;// seconds;
	sprite.setPosition(position.horizontal, position.vertical);
	
	//Lifebar update
	lifeBar.setPosition(position.horizontal, position.vertical + 30);	
	lifeBar.setSize(sf::Vector2f(currentLifePoints*30/maxLifePoints, 3)); // curLife * BAR_SIZE / MAX_LIFE

	if (currentLifePoints <= 0) alive = false;
}

bool Alien::isColliding(const PhysicalEntity & other) const {
	if (dynamic_cast<const Enemy*>(&other) == nullptr && dynamic_cast<const Collectable*>(&other) == nullptr) {
		if (CollisionTools::circleCollision(*this, other))
			return true;
	}
	return false;
}

void Alien::onCollide(PhysicalEntity & other) {
	bool bullet = false;
	if (dynamic_cast<const BulletShip*>(&other) != nullptr) {
		currentLifePoints -= other.getDamagePoints();
		bullet = true;
	} else if (dynamic_cast<const Player*>(&other) != nullptr) {
		currentLifePoints *= 9;
		currentLifePoints /= 10;
	}

	if(bullet && currentLifePoints <= 0){
		dynamic_cast<const BulletShip*>(&other)->_player->increaseScore(maxLifePoints);
	}
}

bool Alien::isClosing(const PhysicalEntity & other) const {
	float dist = std::sqrt(std::pow(position.horizontal - other.getPosition().horizontal, 2) + 
						   std::pow(position.vertical - other.getPosition().vertical, 2));	
	
	if(dist <= radius){
		return true;	
	}
	else{
		return false;
	}
}

void Alien::onClose(PhysicalEntity & other) {
	if (dynamic_cast<const Player*>(&other) != nullptr) {
		toFollow = dynamic_cast<const Player*>(&other);
		isFollowing = true;
	}
}

void Alien::shot () {
	switch(type) {
		case Type::SHOOTER:
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletAlien {*this, BulletAlien::Type::SIMPLE, BulletAlien::SpawnPoint::LEFT, universe});	
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletAlien {*this, BulletAlien::Type::SIMPLE, BulletAlien::SpawnPoint::RIGHT, universe});	
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletAlien {*this, BulletAlien::Type::SIMPLE, BulletAlien::SpawnPoint::FRONT, universe});	
		break;
		default: return;
	}
}
