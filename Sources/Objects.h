#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Defines.h"
#include <iostream>
#include <string>
#include <algorithm>

class box
{
private:
	std::pair<float, float> Xy1, Xy2;
	//Xy1 - left lowest point;
	//Xy2 - right highest point;
public:
	box() : Xy1(0, 0), Xy2(0, 0) {}
	box(float A, float B, float C, float D) : Xy1(A, B), Xy2(C, D) {}
	~box() {}
	bool Collision(box* other)
	{
		if (this->Xy1.first <= other->Xy2.first &&
			this->Xy2.first >= other->Xy1.first &&
			this->Xy1.second <= other->Xy2.second &&
			this->Xy2.second >= other->Xy1.second)
			return 1;
		else return 0;
	}
};

struct PSprite
{
	float x, y,         //from where the sprites begin for an action
		width, height,
		Ox, Oy;         //origins
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

