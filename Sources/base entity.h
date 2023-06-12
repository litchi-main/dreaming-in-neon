#pragma once
#include "box.h"
#include "SFML/Graphics.hpp"
#include "Defines.h"
#include <iostream>
#include <map>
#include <fstream>

class base
{
protected:
	box CollisionBox;
	sf::Texture* SpriteSheet = nullptr;
	sf::Sprite Sprite;
	std::map<std::string, PSprite> Frames;
	std::string currentFrame;
	int framesPassed;
	float x, y;
	side Facing;
public:
	base() : x(WINDOW_WIDTH / 2), y(stage), framesPassed(0), Facing(side::none), SpriteSheet(nullptr) {}
	base(float X, side Side) : x(X), y(stage), framesPassed(0), Facing(Side), SpriteSheet(nullptr) {}
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
		fd.close();
	}
	void setSprite()
	{
		Sprite.setTextureRect(sf::IntRect((int)Frames[currentFrame].x, (int)Frames[currentFrame].y, (int)Frames[currentFrame].width, (int)Frames[currentFrame].height));
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