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
	bool Collision(box*  other)
	{
		if (this->Xy1.first <= other->Xy2.first &&
			this->Xy2.first >= other->Xy1.first &&
			this->Xy1.second >= other->Xy2.second &&
			this->Xy2.second <= other->Xy1.second)
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
};

struct PSprite
{
	float x, y,         //from where the sprites begin for an action
		width, height,
		Ox, Oy;         //origins
	int nextWhen;       //when to switch to the next frame
	std::string nextFrame;   //which frame to switch to
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
	base() : x(WINDOW_WIDTH / 2), y(ground), framesPassed(0), Facing(side::none) {}
	base(float X, side Side) : x(X), y(ground), framesPassed(0), Facing(Side) {}
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
	
	void setCoordinates(float xI, float yI)
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

	float wallCollision(box* enemyCollisionBox)
	{
		if (CollisionBox.Collision(enemyCollisionBox))
		{
			float temp1 = CollisionBox.getXLeft() - enemyCollisionBox->getXRight();
			float temp2 = CollisionBox.getXRight() - enemyCollisionBox->getXLeft();
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
	float dx, dy;
	float forward_v = 4.f;
	float backward_v = -3.f;
	float jump_v = -12.f;
	int air_action = 0;
	bool FInput = 0;
	bool BInput = 0;
	bool NInput = 0;
	bool JInput = 0;
public:
	character() : State(neutral), dx(0.f), dy(0.f) {}
	character(float X, side Side) : base(X,Side), State(neutral), dx(0.f), dy(0.f) {}
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
			else
				framesPassed++;
			NInput = 1;
			FInput = 0;
			BInput = 0;
		}
		else
			if (WalkF)
			{
				dx = forward_v;
				if (!FInput)
				{
					setNewSprite("run1");
					framesPassed = 0;
				}
				else
					framesPassed++;
				FInput = 1;
				NInput = 0;
				BInput = 0;
			}
			else
				if (WalkB)
				{
					dx = backward_v;
					if (!BInput)
					{
						setNewSprite("walkb1");
						framesPassed = 0;
					}
					else framesPassed++;
					BInput = 1;
					NInput = 0;
					FInput = 0;
				}
	}
	void GroundMovement(bool Jump, bool WalkF, bool WalkB)
	{
		WalkMovement(WalkF, WalkB);
		if (Jump && air_action > 0 && !JInput)
		{
			State = state::air;
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
			State = state::air;
			WalkMovement(WalkF, WalkB);
			dy = jump_v / 3 * 2;
			air_action--;
		}
		if (!Jump)
			JInput = 0;
	}

	state getState()
	{
		return State;
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
		if (y >= ground)
		{
			dy = 0.f;
			State = state::neutral;
			y = ground;
			air_action = 2;                
		}
	}
	float pushCollision(box* enemyCollisionBox, float enemyDx)          //so that you push your opponent
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
};

class player : public character
{
protected:
	sf::Keyboard::Key Walkf;
	sf::Keyboard::Key Walkb;
	sf::Keyboard::Key Jump;
public:
	player() : Walkf(sf::Keyboard::Key::D), Walkb(sf::Keyboard::Key::A), Jump(sf::Keyboard::Key::W) {}
	player(float X, side Side, sf::Keyboard::Key F, sf::Keyboard::Key B, sf::Keyboard::Key J) : character(X, Side), Walkf(F), Walkb(B), Jump(J) {}
	~player() {}
	void Input()
	{
		setDy(getDy() + gravity);
		if (getState() == state::neutral)
			GroundMovement(sf::Keyboard::isKeyPressed(Jump), sf::Keyboard::isKeyPressed(Walkf), sf::Keyboard::isKeyPressed(Walkb));
		if (getState() == state::air)
			AirMovement(sf::Keyboard::isKeyPressed(Jump), sf::Keyboard::isKeyPressed(Walkf), sf::Keyboard::isKeyPressed(Walkb));
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
};

