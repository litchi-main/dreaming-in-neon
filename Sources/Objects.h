#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Defines.h"
#include <iostream>
#include <cmath>
#include <fstream>
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
	void setPoints(float A, float B, float C, float D)
	{
		Xy1.first = A;
		Xy1.second = B;
		Xy2.first = C;
		Xy2.second = D;
	}
	bool Collision(box& other)
	{
		if (this->Xy1.first <= other.Xy2.first &&
			this->Xy2.first >= other.Xy1.first &&
			this->Xy1.second >= other.Xy2.second &&
			this->Xy2.second <= other.Xy1.second)
			return 1;
		else return 0;
	}
	float getXLeft()
	{
		return Xy1.first;
	}
	float getXRight()
	{
		return Xy2.first;
	}
	float getYLeft()
	{
		return Xy1.second;
	}
	float getYRight()
	{
		return Xy2.second;
	}
};

struct PSprite
{
	float x, y,         //from where the sprites begin for an action
		width, height,
		Ox, Oy;         //origins
	int nextWhen;      //when to switch to the next frame
	std::string nextFrame;   //which frame to switch to
};

struct attack
{
	int total, startup, active, recovery,
		hitstun, blockstun;
	float damage,
		pushX, pushY, pushYAir;
	box hitbox;
};

class base
{
protected:
	box CollisionBox;
	sf::Texture* SpriteSheet;
	sf::Sprite Sprite;
	std::map<std::string, PSprite> Frames;
	std::string currentFrame;
	int framesPassed;
	float x, y;
	side Facing;
public:
	base() : x(WINDOW_WIDTH / 2), y(stage), framesPassed(0), Facing(side::none) {}
	base(float X, side Side) : x(X), y(stage), framesPassed(0), Facing(Side) {}
	~base() {}
	
	void loadSprites(sf::Texture* sheet, std::string animData)
	{
		SpriteSheet = sheet;
		Sprite.setTexture(*SpriteSheet);
		std::ifstream fd(animData);
		std::string key;
		while (fd >> key)
		{
			PSprite B;
			fd >> B.x >> B.y >> B.width >> B.height >> B.Ox >> B.Oy >> B.nextFrame >> B.nextWhen;
			Frames[key] = B;
		}
	}
	void setSprite()
	{
		Sprite.setTextureRect(sf::IntRect(Frames[currentFrame].x, Frames[currentFrame].y, Frames[currentFrame].width, Frames[currentFrame].height));
		Sprite.setOrigin(sf::Vector2f(Frames[currentFrame].Ox, Frames[currentFrame].Oy));
		if (Facing == side::right)
			Sprite.setScale(sf::Vector2f(-0.5f, 0.5f));
		else
			Sprite.setScale(sf::Vector2f(0.5f, 0.5f));
		Sprite.setPosition(x, y);

	}
	void setNewSprite(std::string key)
	{
		framesPassed = 0;
		currentFrame = key;
		setSprite();
	}
	void changeFrame()
	{
		if (Frames[currentFrame].nextWhen == framesPassed)
			setNewSprite(Frames[currentFrame].nextFrame);
	}
	sf::Sprite getSprite()
	{
		return Sprite;
	}
	
	virtual void setCoordinates(float xI, float yI)
	{
		x = xI;
		y = yI;
		CollisionBox.setPoints(x - 10.f, y, x + 10.f, y - 80.f);
	}
	float getX()
	{
		return x;
	}
	float getY()
	{
		return y;
	}

	float wallCollision(box& enemyCollisionBox)
	{
		if (CollisionBox.Collision(enemyCollisionBox))
		{
			float temp1 = CollisionBox.getXLeft() - enemyCollisionBox.getXRight();
			float temp2 = CollisionBox.getXRight() - enemyCollisionBox.getXLeft();
			if (abs(temp1) <= abs(temp2))
				return temp1;
			else 
				return temp2;
		}
		return 0.f;
	}
	box* getCollisionBox()
	{
		return &CollisionBox;
	}

	void changeFacing()
	{
		if (Facing == side::left)
			Facing = side::right;
		else
			if (Facing == side::right)
				Facing = side::left;
	}
	bool updateFacing(float enemyX)
	{
		if (Facing == side::left && enemyX < x)
		{
			changeFacing();
			return 1;
		}
		if (Facing == side::right && enemyX > x)
		{
			changeFacing();
			return 1;
		}
		return 0;
	}
};

class character : public base
{
protected:
	state State;
	position Pos;
	float dx, dy;
	float forward_v = 7.f;
	float backward_v = -3.f;
	float jump_v = -12.f;
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
	character() : State(neutral), dx(0.f), dy(0.f), forward_v(0), backward_v(0), jump_v(0), health(0) {}
	character(float X, float fv, float bv, float jv, float hp, side Side) 
		: base(X, Side), State(neutral), dx(0.f), dy(0.f), forward_v(fv), backward_v(bv), jump_v(jv), health(hp) {}
	~character() {}

	void WalkMovement(bool WalkF, bool WalkB)
	{
		if (!WalkB && !WalkF || WalkF && WalkB)
		{
			dx -= dx;
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
				dx = forward_v;
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
					dx = backward_v;
					if (!BWInput)
					{
						setNewSprite("walkb1");
						framesPassed = 0;
					}
					BWInput = 1;
					NInput = 0;
					FWInput = 0;
				}
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
			temp.hitbox.setPoints(X1, Y1, X1 + X2, Y1 + Y2);
			Attacks[key] = temp;
		}
	}
	void Attackcheck(character& enemy)
	{
		if (framesPassed == CurrentAttack.startup)
			IsAttackActive = 1;
		if (framesPassed == CurrentAttack.active)
			IsAttackActive = 0;
		if (IsAttackActive)
		{
			hitbox.setPoints(x + CurrentAttack.hitbox.getXLeft(), y + CurrentAttack.hitbox.getYLeft(), x + CurrentAttack.hitbox.getXRight(), y + CurrentAttack.hitbox.getYRight());
			if (hitbox.Collision(enemy.hurtbox))
			{
				enemy.State = state::hit;
				enemy.stun = CurrentAttack.hitstun;
				enemy.setNewSprite("hit");
			}
		}
	}

	int getStun()
	{
		return stun;
	}
	state getState()
	{
		return State;
	}
	position getPos()
	{
		return Pos;
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

class player : public character
{
protected:
	sf::Keyboard::Key Walkf;
	sf::Keyboard::Key Walkb;
	sf::Keyboard::Key Jump;
	sf::Keyboard::Key A;
	sf::Keyboard::Key B;
public:
	player() : Walkf(sf::Keyboard::Key::D), Walkb(sf::Keyboard::Key::A), Jump(sf::Keyboard::Key::W) {}
	player(float X, float fv, float bv, float jv, float hp, side Side, sf::Keyboard::Key F, sf::Keyboard::Key B, sf::Keyboard::Key J, sf::Keyboard::Key Ain, sf::Keyboard::Key Bin) 
		: character(X, fv, bv, jv, hp, Side), Walkf(F), Walkb(B), Jump(J), A(Ain), B(Bin) {}
	~player() {}
	void Input(player& enemy)
	{
		framesPassed++;
		setDy(getDy() + gravity);
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
};

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
