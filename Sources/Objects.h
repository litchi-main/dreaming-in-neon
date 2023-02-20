#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Defines.h"
#include <iostream>
#include <string>
#include <algorithm>



struct PSprite
{
	float x, y; //sprite origin
	sf::Texture tex;
};

struct Attack
{
	int startup, active, recovery, hitstun, blockstun, pushback, pushblock;
	float dmg;
	std::pair <float, float> hitbox[2];
	PSprite tStart, tActive, tRecovery;
};

struct Player
{
	float health = 4500;
	float x, y;
	std::pair <float, float> hurtbox[2];
	float width = 30;
	float height = 120;
	float forward_v = 4;
	float backward_v = -3;
	float jump_v = -10;
	float air_action = 0;
	bool jump_input = 1;
	char attack_input = 'n';
	float dy = 0;
	float dx = 0;
	int stun = 0;
	bool cancel = 0;
	Attack move;
	side AtCorner = side::none;
	side side = side::none;
	sf::Keyboard::Key Forward;
	sf::Keyboard::Key Back;
	sf::Keyboard::Key Up;
	sf::Keyboard::Key A;
	sf::Keyboard::Key B;
	Nstate state;
	Astate attack;
	PSprite sprite;
};

