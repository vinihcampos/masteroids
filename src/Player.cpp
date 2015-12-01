#include "Player.h"
#include "Asteroid.h"
#include "BulletShip.h"
#include "BulletAlien.h"
#include "Universe.h"
#include "Collectable.h"
#include "CollisionTools.h"
#include <cmath>
#include <iostream>

Player::Player(sf::Texture & _texture, Universe & _universe, double _frameWidth, double _frameHeigth, sf::Time _frameDuration) : AnimatedPhysicalEntity(_texture, _universe, _frameWidth, _frameHeigth, _frameDuration), ActionTarget(Configuration::playerInputs) {
	angleVelocity = 0.0;
	thrusting = false;
	hasShot = false;
	shotLevel = Player::ShotLevel::SIMPLE;
	alive = true;
	isInHyperspace = false;
	radius = 200;
	angleAdjustment = -90;
	indestructible = false;
	slowingAsteroids = false;
	byPassing = false;
	
	position.vertical = 400;
	position.horizontal = 600;

	currentLifePoints = 100;
	maxLifePoints = 100;
	score = 0;
	
	//sprite.setTexture(Configuration::textures.get(Configuration::Textures::Player));
	sprite.setRotation(angleAdjustment);
	sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
	sprite.setScale(.8,.8);

	setLooping(true);
	play();	
	// Thrust
	bind(Configuration::PlayerInputs::Thrust, [this](const sf::Event &) {
		thrusting = true;	
	});
	
	// Turn left	
	bind(Configuration::PlayerInputs::TurnLeft, [this](const sf::Event &) {
		angleVelocity = -2;
	});
	
	// Turn right
	bind(Configuration::PlayerInputs::TurnRight, [this](const sf::Event &) {
		angleVelocity = 2;
	});
	
	// Shoot
	bind(Configuration::PlayerInputs::Shoot, [this](const sf::Event &) {
		if (timeLastShot > sf::seconds(0.3)) {	
			shot();
			timeLastShot = sf::Time::Zero;
		}
	});

	bind(Configuration::PlayerInputs::Hyperspace, [this](const sf::Event &) {
		position.horizontal = rand() % 800;
		position.vertical = rand() % 600;
	});
}

void Player::proccessEvents() {
	thrusting = false;
	angleVelocity = 0.0;
	ActionTarget::proccessEvents();	
}

void Player::update(sf::Time deltaTime) {
	AnimatedPhysicalEntity::update(deltaTime);

	float seconds = deltaTime.asSeconds();
	
	timeLastShot += deltaTime;	

	if (thrusting) {
		angle = getRotationRad(); 
		acceleration.horizontal = std::cos(angle) * .1;
		acceleration.vertical = std::sin(angle) *.1;
	} else {
		acceleration.horizontal = 0;
		acceleration.vertical = 0;
	} 

	if (isUnderEffect()) 
		timeCollectableEffect += deltaTime;

	if (timeCollectableEffect >= sf::seconds(5.0)) {
		setIndestructible(false);
		setSlowingAsteroids(false);
		setByPassing(false);
		timeCollectableEffect = sf::Time::Zero;
	}	

	// Angle update
	sprite.rotate(angleVelocity);

	// Updating velocity
	velocity.horizontal += acceleration.horizontal;
	velocity.vertical += acceleration.vertical;

	// Updating position
	position.horizontal = (position.horizontal + velocity.horizontal); //% Configuration::WINDOW_WIDTH;// * seconds;
	position.vertical = (position.vertical + velocity.vertical); //% Configuration::WINDOW_HEIGHT;// seconds;
	sprite.setPosition(position.horizontal, position.vertical);

	// Setting Euclidean Torus
	if (position.horizontal <= sprite.getGlobalBounds().width / 2) 
		position.horizontal = Configuration::WINDOW_WIDTH - sprite.getGlobalBounds().width / 2; 
	else if (position.horizontal >= Configuration::WINDOW_WIDTH - sprite.getGlobalBounds().width / 2) 
		position.horizontal = sprite.getGlobalBounds().width / 2; 

	if (position.vertical <= sprite.getGlobalBounds().height / 2) 
		position.vertical =  Configuration::WINDOW_HEIGHT - sprite.getGlobalBounds().height / 2; 
	else if (position.vertical >= Configuration::WINDOW_HEIGHT - sprite.getGlobalBounds().height / 2) 
		position.vertical = sprite.getGlobalBounds().height / 2; 
	
	// Decrease velocity
	velocity.horizontal *= .99;
	velocity.vertical *= .99;

}

bool Player::isUnderEffect() const {
	return byPassing || slowingAsteroids || indestructible;
}

bool Player::isColliding(const PhysicalEntity & other) const {
	if (dynamic_cast<const Player*>(&other) == nullptr && 
		dynamic_cast<const BulletShip*>(&other) == nullptr &&
		dynamic_cast<const AnimatedPhysicalEntity*>(&other) == nullptr) {
		if (CollisionTools::circleCollision(*this, other)) {
			std::cout << "ola" << std::endl;
			return true;
		}
	}
	return false;
}

void Player::decreaseShotLevel() {
	if (shotLevel > 0)
		shotLevel--;

}

void Player::onCollide(PhysicalEntity & other) {
	if (dynamic_cast<const Collectable*>(&other) != nullptr) {
		
	} 

	int damage = 0;

	if(dynamic_cast<const BulletAlien*>(&other) != nullptr){
		decreaseShotLevel();
		damage = dynamic_cast<const BulletAlien*>(&other)->getType();
	} else if(dynamic_cast<const Collectable*>(&other) != nullptr) {
	
	} else {			
		if (!byPassing) {
			decreaseShotLevel();
			damage = 30;
		}
	} 
	
	if (!indestructible)
		currentLifePoints -= damage;
	
	if(currentLifePoints <= 0)
		alive = false;
}

bool Player::isClosing(const PhysicalEntity & other) const {
	if (dynamic_cast<const Enemy*>(&other) != nullptr ) {
		float dist = std::sqrt(std::pow(position.horizontal - other.getPosition().horizontal, 2) + 
						  	   std::pow(position.vertical - other.getPosition().vertical, 2));
		if(dist <= radius)
			return true;
	}
	return false;
}

void Player::onClose(PhysicalEntity & other) {
	return;
}

void Player::shot () {
	switch(shotLevel) {
		case ShotLevel::SIMPLE:
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::SIMPLE, BulletShip::SpawnPoint::FRONT, universe});	
		break;
		case ShotLevel::DOUBLE:
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::SIMPLE, BulletShip::SpawnPoint::LEFT, universe});	
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::SIMPLE, BulletShip::SpawnPoint::RIGHT, universe});	
		break;
		case ShotLevel::TRIPLE:
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::SIMPLE, BulletShip::SpawnPoint::LEFT, universe});	
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::SIMPLE, BulletShip::SpawnPoint::RIGHT, universe});	
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::SIMPLE, BulletShip::SpawnPoint::FRONT, universe});	
		break;
		case ShotLevel::POWERFULL:
			//universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::POWERFULL, BulletShip::SpawnPoint::LEFT, universe});	
			//universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::POWERFULL, BulletShip::SpawnPoint::RIGHT, universe});	
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::POWERFULL, BulletShip::SpawnPoint::FRONT, universe});	
		break;
		case ShotLevel::LASER:
			universe.addEntity(PhysicalEntity::EntityType::Bullet, new BulletShip {*this, BulletShip::Type::LASER, BulletShip::SpawnPoint::FRONT, universe});	
		break;
		default: return;
	}
}

int Player::getScore() const{
	return score;
}
void Player::increaseScore(int points){
	score += points;
}

int Player::getBonusDamage() const {
	return bonusDamage;
}

void Player::setBonusDamage(int _bonusDamage) {
	bonusDamage = _bonusDamage;
}

int Player::getBonusPrecision() const {
	return bonusPrecision;
}

void Player::setBonusPrecision(int _bonusPrecision) {
	bonusPrecision = _bonusPrecision;
}

bool Player::isIndestructible() const {
	return indestructible;
}

void Player::setIndestructible(bool _indestructible) {
	indestructible = _indestructible;
}

bool Player::isSlowingAsteroids() const {
	return slowingAsteroids;
}

void Player::setSlowingAsteroids(bool _slowingAsteroids) {
	slowingAsteroids = _slowingAsteroids;
}

bool Player::isByPassing() const {
	return byPassing;
}

void Player::setByPassing(bool _byPassing) {
	byPassing = _byPassing;
}
