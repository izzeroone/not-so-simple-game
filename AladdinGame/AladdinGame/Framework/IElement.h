﻿
#ifndef __ICOMPONENT_H__
#define __ICOMPONENT_H__
#include "define.h"
#include "Sprite.h"
#include <numeric>
#include <map>
using namespace std;
LINK_FRAMEWORK // Link into the framework
//Các thành phần ảnh hưởng đến sự chuyển động của object
class IMovingCompoment
{
public:
	virtual void update(float deltatime) = 0;
};


class Movement : public IMovingCompoment
{
public:
	Movement(GVector2 accel, GVector2 veloc, Sprite* refSprite);
	void update(float deltatime);

	void setAccelerate(GVector2 accel);
	void setVelocity(GVector2 veloc);
	GVector2 getAccelerate();
	GVector2 getVelocity();

private:
	GVector2 _accelerate;
	GVector2 _velocity;
	Sprite* _refSprite;
};

enum eGravityStatus
{
	FALLING__DOWN,
	LANDED
};

class Gravity : public IMovingCompoment
{
public:
	Gravity(GVector2 gravity, Movement *movement);
	void setStatus(eGravityStatus status);
	void update(float deltatime);
	void setGravity(GVector2 gravity);

private:
	GVector2 _gravity;
	GVector2 _additionalVeloc;
	Movement* _refmovement;
	eGravityStatus _status;
	Sprite* _refSprite;
};

#endif // !__ICOMPONENT_H__
