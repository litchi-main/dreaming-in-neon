#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include "box.h"
#include "player.h"
#include "Defines.h"

class swordguy : public player
{
public:
	swordguy(float X, float fv, float bv, float jv, float hp, side Side, sf::Keyboard::Key F, sf::Keyboard::Key B, sf::Keyboard::Key J, sf::Keyboard::Key Ain, sf::Keyboard::Key Bin)
		: player(X, fv, bv, jv, hp, Side, F, B, J, Ain, Bin) {}
	swordguy(const swordguy& temp) : player(temp.x, temp.forward_v, temp.backward_v, temp.jump_v, temp.health, temp.Facing, temp.Walkf, temp.Walkb, temp.Jump, temp.A, temp.B) {}
	~swordguy() {}

	void Attacking(bool A, bool B, bool WalkF, bool Walkb)
	{
		if (State == state::neutral)
		{
			if (Pos == position::ground)
			{
				if (A)
					AssingAttack("5A");
				if (B)
					AssingAttack("5B");
			}
			if (Pos == position::air)
			{
				if (A)
					AssingAttack("j.A");
				if (B)
					AssingAttack("j.B");
			}
		}
	}
};
