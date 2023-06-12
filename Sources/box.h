#pragma once
#include <iostream>

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
	float x = 0.f, y = 0.f,         //from where the sprites begin for an action
		width = 0.f, height = 0.f,
		Ox = 0.f, Oy = 0.f;         //origins
	int nextWhen = 0;      //when to switch to the next frame
	std::string nextFrame;   //which frame to switch to
};

struct attack
{
	int total = 0, startup = 0, active = 0, recovery = 0,
		hitstun = 0, blockstun = 0;
	float damage = 0.f,
		pushX = 0.f, pushY = 0.f, pushYAir = 0.f;
	box hitbox;
};