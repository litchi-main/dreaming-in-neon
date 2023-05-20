#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Defines.h"
#include <iostream>
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
};

class base
{
protected:
	box CollisionBox;
	sf::Texture SpriteSheet;
	sf::Sprite Sprite;
	std::map<std::string, PSprite> Frames;
	float x, y;
public:
	base() : x(WINDOW_WIDTH / 2), y(ground) {}
	
	void loadSprites(std::string sheet, std::string animData)
	{
		SpriteSheet.loadFromFile(sheet);
		Sprite.setTexture(SpriteSheet);
		std::ifstream fd(animData);
		std::string key;
		while (fd >> key)
		{
			PSprite B;
			fd >> B.x >> B.y >> B.width >> B.height >> B.Ox >> B.Oy;
			Frames[key] = B;
		}
	}
	void setSprite(std::string key)
	{
		Sprite.setTextureRect(sf::IntRect(Frames[key].x, Frames[key].y, Frames[key].width, Frames[key].height));
		Sprite.setOrigin(sf::Vector2f(Frames[key].Ox, Frames[key].Oy + Frames[key].height));
		Sprite.setPosition(x,y);
	}
	sf::Sprite getSprite()
	{
		return Sprite;
	}
	
	void setCoordinates(float xI, float yI)
	{
		x = xI;
		y = yI;
	}
	float getX()
	{
		return x;
	}
	float getY()
	{
		return y;
	}
};

class character : public base
{
protected:
	float dx, dy;
	float forward_v = 4;
	float backward_v = -3;
	float jump_v = -10;
	float air_action = 0;
	bool jump_input = 1;
public:
	void WalkMovement(bool WalkF, bool WalkB)
	{
		if (WalkF)
			dx = forward_v;
		if (WalkB)
			dx = backward_v;
		if (!WalkB && !WalkF)
			dx -= dx;
	}
	void GroundMovement(bool Jump, bool WalkF, bool WalkB)
	{
		WalkMovement(WalkF, WalkB);
		if (Jump && air_action > 0)
		{
			dy = jump_v;
			air_action--;
		}
	}
	void AirMovement(bool Jump, bool WalkF, bool WalkB)
	{
		if (y >= ground)
		{
			y = ground;
			air_action = 2;                //kiek kartu galima jumpint
		}
		if (Jump && air_action > 0)
		{
			WalkMovement(WalkF, WalkB);
			dy = jump_v;
			air_action--;
		}
	}

	float getDx()
	{
		return dx;
	}
	float getDy()
	{
		return dy;
	}
};

class player : public character
{
public:
	void Input()
	{
		WalkMovement(sf::Keyboard::isKeyPressed(sf::Keyboard::D), sf::Keyboard::isKeyPressed(sf::Keyboard::A));
	}
};

