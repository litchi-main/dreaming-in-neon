#pragma once
#include "character.h"
#include "Defines.h"
#include "SFML/Graphics.hpp"
#include <iostream>

class player : public character
{
protected:
	sf::Keyboard::Key Walkf;
	sf::Keyboard::Key Walkb;
	sf::Keyboard::Key Jump;
	sf::Keyboard::Key A;
	sf::Keyboard::Key B;
public:
	player() : Walkf(sf::Keyboard::Key::D), Walkb(sf::Keyboard::Key::A), Jump(sf::Keyboard::Key::W), A(sf::Keyboard::J), B(sf::Keyboard::K) {}
	player(float X, float fv, float bv, float jv, float hp, side Side, sf::Keyboard::Key F, sf::Keyboard::Key B, sf::Keyboard::Key J, sf::Keyboard::Key Ain, sf::Keyboard::Key Bin)
		: character(X, fv, bv, jv, hp, Side), Walkf(F), Walkb(B), Jump(J), A(Ain), B(Bin) {}
	~player() {}
	bool isBlocking()
	{
		return sf::Keyboard::isKeyPressed(Walkb);
	}
	void Input(player& enemy)
	{
		framesPassed++;
		setDy(getDy() + gravity);
		groundFriction();
		if (stun == 0)
		{
			if (State != state::neutral)
			{
				FWInput = 0;
				BWInput = 0;
				NInput = 0;
			}
			State = state::neutral;
			if (getPos() == position::ground)
				GroundMovement(sf::Keyboard::isKeyPressed(Jump), sf::Keyboard::isKeyPressed(Walkf), sf::Keyboard::isKeyPressed(Walkb));
			if (getPos() == position::air)
			{
				AirMovement(sf::Keyboard::isKeyPressed(Jump), sf::Keyboard::isKeyPressed(Walkf), sf::Keyboard::isKeyPressed(Walkb));
				if (dy < -3.f)
					setNewSprite("jumprising");
				else
					if (abs(dy) < 3.f)
						setNewSprite("jumphang");
					else
						if (dy > 3.f)
							setNewSprite("jumpfall");
			}
			Attacking(sf::Keyboard::isKeyPressed(A), sf::Keyboard::isKeyPressed(B), Walkf, Walkb);
		}
		else
		{
			stun--;
			if (State == state::attacking)
				Attackcheck(enemy);
		}
	}
	void swapInputs(bool didFacingChange)
	{
		if (didFacingChange)
		{
			sf::Keyboard::Key temp = Walkf;
			Walkf = Walkb;
			Walkb = temp;
			forward_v *= -1;
			backward_v *= -1;
		}
	}
	void AssingAttack(std::string key)
	{
		State = state::attacking;
		CurrentAttack = Attacks[key];
		setNewSprite(key);
		stun = Attacks[key].total;
	}
	void attackBlocked(character& enemy)
	{
		if (enemy.isBlocking())
		{
			enemy.setState(state::block);
			enemy.setStun(CurrentAttack.blockstun);
			enemy.setHp(enemy.getHp() - CurrentAttack.damage / 5);
			if (enemy.getPos() == position::ground)
				enemy.setNewSprite("block");
			if (enemy.getPos() == position::air)
				enemy.setNewSprite("jumpblock");
		}
	}
	void attackNotBlocked(character& enemy)
	{
		if (!enemy.isBlocking())
		{
			enemy.setState(state::hit);
			enemy.setStun(CurrentAttack.hitstun);
			enemy.setHp(enemy.getHp() - CurrentAttack.damage);
			if (enemy.getPos() == position::ground)
				enemy.setNewSprite("hit");
			if (enemy.getPos() == position::air)
				enemy.setNewSprite("jumphit");
		}
	}
};