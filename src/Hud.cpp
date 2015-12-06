#include "Hud.h"
#include "Player.h"
#include "Collectable.h"
#include <string>
#include <iostream>

Hud::Hud(){}

Hud::Hud(Player * _player){
	player = _player;

	maxLife.setSize(sf::Vector2f(300, 10));
	maxLife.setPosition(10,10);
	maxLife.setFillColor(sf::Color::Red);

	currentLife.setSize(sf::Vector2f(300, 10));
	currentLife.setPosition(10,10);
	currentLife.setFillColor(sf::Color::Green);

	//font.loadFromFile("media/Colleged.ttf");
	font.loadFromFile("media/Fonts/SFAlienEncountersSolid.ttf");

	scorePoints = player->getScore();
	score.setFont(font);
	score.setString(std::string("Score: " + std::to_string(scorePoints)));
	score.setPosition(350, 5);
	score.setColor(sf::Color::White);
	score.setCharacterSize(20);
	
	curPower.setSize(sf::Vector2f(39, 39));
	curPower.setPosition(640,10);
	curPower.setOutlineThickness(2);
	curPower.setOutlineColor(sf::Color::White);
	curPower.setFillColor(sf::Color::Transparent);
	
	nextPower.setSize(sf::Vector2f(39, 39));
	nextPower.setPosition(690,10);
	nextPower.setOutlineThickness(2);
	nextPower.setOutlineColor(sf::Color::White);
	nextPower.setFillColor(sf::Color::Transparent);

	weaponLevel.setFont(font);
	weaponLevel.setString(std::string(std::to_string(player->getShotLevel())));
	weaponLevel.setPosition(600, 50);
	weaponLevel.setColor(sf::Color::White);
	weaponLevel.setCharacterSize(24);
	
	damageIncrease.setFont(font);
	damageIncrease.setString(std::string(std::to_string(player->getBonusDamage())));
	damageIncrease.setPosition(700, 50);
	damageIncrease.setColor(sf::Color::White);
	damageIncrease.setCharacterSize(24);
	
	powerTime.setFont(font);
	powerTime.setString(std::string(std::to_string(player->getBonusDamage())));
	powerTime.setPosition(638, 50);
	powerTime.setColor(sf::Color::White);
	powerTime.setCharacterSize(16);
}

void Hud::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	target.draw(maxLife, states);
	target.draw(currentLife, states);
	target.draw(score, states);
	//target.draw(weaponLevel, states);
	//target.draw(damageIncrease, states);
	target.draw(curPower, states);
	target.draw(nextPower, states);
	if((player->getTimeCollectableEffect()).asSeconds() > 0) {
		target.draw(powerTime, states);
	}
}

void Hud::update(){ 
	if (player->isAlive()) {
		if (player->isUnderEffect()) 
			nextPower.setOutlineColor(sf::Color::Blue);
		else 
			nextPower.setOutlineColor(sf::Color::White);

		currentLife.setSize(sf::Vector2f(((player->getCurrentLifePoints() * 300) / player->getMaxLifePoints()), 10));
		scorePoints = player->getScore();
		score.setString(std::string("Score: " + std::to_string(scorePoints)));
		weaponLevel.setString(std::string(std::to_string(player->getShotLevel())));
		powerTime.setString(std::string(std::to_string( 5 - (player->getTimeCollectableEffect()).asSeconds())));
		damageIncrease.setString(std::string(std::to_string(player->getBonusDamage())));
		if ((player->powersToUse).size() > 0) {
			((player->powersToUse).front())->setPosition({710, 29});
		}
		if ((player->powersToUse).size() > 1) {
			((player->powersToUse).back())->setPosition({660, 29});
		}
	}
}

