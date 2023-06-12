#pragma once
#include "base entity.h"
#include "box.h"
#include "Defines.h"
#include "SFML/Graphics.hpp"
#include <iostream>

class character : public base
{
protected:
	state State;
	position Pos;
	float dx, dy;
	float forward_v;
	float backward_v;
	float jump_v;
	float max_v;
	float health;
	int air_action = 0;
	bool FWInput = 0;
	bool BWInput = 0;
	bool NInput = 0;
	bool JInput = 0;
	bool AInput = 0;
	bool BInput = 0;
	bool IsAttackActive = 0;
	attack CurrentAttack;
	std::map<std::string, attack> Attacks;
	int stun = 0;
	box hurtbox, hitbox;
public:
	character() : State(neutral), Pos(ground), dx(0.f), dy(0.f), forward_v(0), backward_v(0), jump_v(0), max_v(9.f), health(0) {}
	character(float X, float fv, float bv, float jv, float hp, side Side)
		: base(X, Side), State(neutral), Pos(ground), dx(0.f), dy(0.f), forward_v(fv), backward_v(bv), jump_v(jv), max_v(9.f), health(hp) {}
	~character() {}

	void WalkMovement(bool WalkF, bool WalkB)
	{
		if (!WalkB && !WalkF || WalkF && WalkB)
		{
			if (!NInput)
			{
				setNewSprite("idle1");
				framesPassed = 0;
			}
			NInput = 1;
			FWInput = 0;
			BWInput = 0;
		}
		else
			if (WalkF)
			{
				dx += forward_v;
				if (!FWInput)
				{
					setNewSprite("run1");
					framesPassed = 0;
				}
				FWInput = 1;
				NInput = 0;
				BWInput = 0;
			}
			else
				if (WalkB)
				{
					dx += backward_v;
					if (!BWInput)
					{
						setNewSprite("walkb1");
						framesPassed = 0;
					}
					BWInput = 1;
					NInput = 0;
					FWInput = 0;
				}
		if (dx > max_v)
			dx = max_v;
		if (dx < -max_v)
			dx = -max_v;
	}
	void GroundMovement(bool Jump, bool WalkF, bool WalkB)
	{
		WalkMovement(WalkF, WalkB);
		if (Jump && air_action > 0 && !JInput)
		{
			Pos = position::air;
			JInput = 1;
			dy = jump_v;
			air_action--;
		}
		if (!Jump)
			JInput = 0;
	}
	void AirMovement(bool Jump, bool WalkF, bool WalkB)
	{
		if (Jump && air_action > 0 && !JInput)
		{
			Pos = position::air;
			WalkMovement(WalkF, WalkB);
			dy = jump_v / 3 * 2;
			air_action--;
		}
		if (!Jump)
			JInput = 0;
	}
	void groundFriction()
	{
		if (Pos == position::ground)
		{
			if (dx > 0)
			{
				dx -= friction;
				if (dx < 0)
					dx = 0;
			}
			if (dx < 0)
			{
				dx += friction;
				if (dx > 0)
					dx = 0;
			}
		}
	}

	virtual void Attacking(bool A, bool B, bool WalkF, bool Walkb) = 0;
	void LoadFrameData(std::string file)
	{
		std::ifstream fd(file);
		std::string key;
		attack temp;
		while (fd >> key)
		{
			fd >> temp.total >> temp.startup >> temp.active >> temp.recovery >> temp.hitstun >> temp.blockstun >> temp.damage >> temp.pushX >> temp.pushY >> temp.pushYAir;
			float X1, Y1, X2, Y2;
			fd >> X1 >> Y1 >> X2 >> Y2;
			temp.hitbox.setPoints(X1, Y1, X2, Y2);
			Attacks[key] = temp;
		}
	}
	virtual void attackBlocked(character& enemy) = 0;
	virtual void attackNotBlocked(character& enemy) = 0;
	virtual bool isBlocking() = 0;
	void Attackcheck(character& enemy)
	{
		if (framesPassed == CurrentAttack.startup)
			IsAttackActive = 1;
		if (framesPassed == CurrentAttack.active)
			IsAttackActive = 0;
		if (IsAttackActive)
		{
			if (Facing == side::left)
				hitbox.setPoints(x + CurrentAttack.hitbox.getXLeft() / 2, y + CurrentAttack.hitbox.getYLeft() / 2, x + CurrentAttack.hitbox.getXRight() / 2, y + CurrentAttack.hitbox.getYRight() / 2);
			if (Facing == side::right)
				hitbox.setPoints(x - CurrentAttack.hitbox.getXRight() / 2, y + CurrentAttack.hitbox.getYLeft() / 2, x - CurrentAttack.hitbox.getXLeft() / 2, y + CurrentAttack.hitbox.getYRight() / 2);
			if (hitbox.Collision(enemy.hurtbox))
			{
				IsAttackActive = 0;
				attackNotBlocked(enemy);
				attackBlocked(enemy);
			}
		}
	}

	int getStun()
	{
		return stun;
	}
	void setStun(int s)
	{
		stun = s;
	}
	state getState()
	{
		return State;
	}
	void setState(state S)
	{
		State = S;
	}
	position getPos()
	{
		return Pos;
	}
	void setPos(position p)
	{
		Pos = p;
	}
	float getHp()
	{
		return health;
	}
	void setHp(float hp)
	{
		health = hp;
	}

	float getDx()
	{
		return dx;
	}
	float getDy()
	{
		return dy;
	}
	void setDx(float dxI)
	{
		dx = dxI;
	}
	void setDy(float dyI)
	{
		dy = dyI;
	}
	void groundCollision()                 //to not go straight through the floor
	{
		if (y >= stage)
		{
			if (Pos != position::ground)
			{
				NInput = 0;
				FWInput = 0;
				BWInput = 0;
				if (State == state::attacking)
				{
					State = neutral;
					stun = 0;
				}
			}
			dy = 0.f;
			Pos = position::ground;
			y = stage;
			air_action = 2;
		}
	}
	float pushCollision(box& enemyCollisionBox, float enemyDx)          //so that you push your opponent
	{
		if (CollisionBox.Collision(enemyCollisionBox))
			if (Facing == side::left && (dx > 0 || enemyDx < 0) ||
				Facing == side::right && (dx < 0 || enemyDx > 0))
			{
				dx = (dx + enemyDx) / 2;
				return dx;
			}
			else
				return enemyDx;
		else
			return enemyDx;
	}
	void setCoordinates(float xI, float yI)
	{
		x = xI;
		y = yI;
		CollisionBox.setPoints(x - 10.f, y, x + 10.f, y - 80.f);
		hurtbox.setPoints(x - 25.f, y, x + 25.f, y - 175.f);
	}
};