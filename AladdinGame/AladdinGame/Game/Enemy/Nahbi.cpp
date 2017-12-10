﻿#include "Nahbi.h"


void NahbiPhysicsComponent::init()
{
	_movingSpeed = 50;
	auto movement = new Movement(GVector2(10, 0), GVector2(0, 0), this);
	_componentList["Movement"] = movement;
}

GVector2 NahbiPhysicsComponent::getVelocity()
{
	auto move = (Movement*)getComponent("Movement");
	return move->getVelocity();
}


RECT NahbiPhysicsComponent::getBounding()
{
	return _obj->getAnimationComponent()->getBounding();
}

void NahbiAnimationComponent::init()
{
	_sprite = SpriteResource::getInstance()->getSprite(eObjectID::GUARD);
	_sprite->setFrameRect(SpriteResource::getInstance()->getSourceRect(eObjectID::GUARD, "nahbi_running_8"));
	_sprite->setZIndex(0.0f);

	setOrigin(GVector2(0.0f, 0.0f));
	setScale(SCALE_FACTOR);

	//_animations[eStatus::NORMAL] = new Animation(_sprite, 0.07f);
	//_animations[eStatus::NORMAL]->addFrameRect(eObjectID::GUARD, "nahbi_running_8", NULL);

	_animations[eStatus::RUNNING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::RUNNING]->addFrameRect(eObjectID::GUARD, "nahbi_running_8", "nahbi_running_7", "nahbi_running_6", "nahbi_running_5", "nahbi_running_4", "nahbi_running_3", "nahbi_running_2", "nakim_running_1", NULL);

	_animations[eStatus::TAUGHT] = new Animation(_sprite, 0.07f);
	_animations[eStatus::TAUGHT]->addFrameRect(eObjectID::GUARD, "nahbi_taught_6", "nahbi_taught_5", "nahbi_taught_4", "nahbi_taught_3", "nahbi_taught_2", "nahbi_taught_1", NULL);

	_animations[eStatus::SLASH1] = new Animation(_sprite, 0.07f); 
	_animations[eStatus::SLASH1]->addFrameRect(eObjectID::GUARD, "nahbi_slash1_6", "nahbi_slash1_5", "nahbi_slash1_4", "nahbi_slash1_3", "nahbi_slash1_2", "nahbi_slash1_1", NULL);

	_animations[eStatus::SLASH2] = new Animation(_sprite, 0.07f);
	_animations[eStatus::SLASH2]->addFrameRect(eObjectID::GUARD, "nahbi_slash2_5", "nahbi_slash2_4", "nahbi_slash2_3", "nahbi_slash2_2", "nahbi_slash2_1", NULL);

	_animations[eStatus::BEATEN] = new Animation(_sprite, 0.07f);
	_animations[eStatus::BEATEN]->addFrameRect(eObjectID::GUARD, "nahbi_beaten_6", "nahbi_beaten_5", "nahbi_beaten_4", "nahbi_beaten_3", "nahbi_beaten_2", "nahbi_beaten_1", NULL);

	_animations[eStatus::BURNED] = new Animation(_sprite, 0.07f);
	_animations[eStatus::BURNED]->addFrameRect(eObjectID::GUARD, "nahbi_burned_9", "nahbi_burned_8", "nahbi_burned_7", "nahbi_burned_6", "nahbi_burned_5", "nahbi_burned_4", "nahbi_burned_3", "nahbi_burned_2", "nahbi_burned_1", NULL);

	for (auto animate : _animations)
	{
		animate.second->setColorFlash(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void NahbiBehaviorComponent::init()
{
	setStatus(eStatus::TAUGHT);
	_hitpoint = 100;
	_standTime = 0;
	_collisionComponent = new CollisionComponent(eDirection::ALL);
	_collisionComponent->setTargerGameObject(_obj);
}

void NahbiBehaviorComponent::update(float detatime)
{
	if (_hitpoint <= 0)
	{
		setStatus(eStatus::DESTROY);
		return;
	}

	_standTime -= detatime;
	if (_standTime > 0)
	{
		setStatus(eStatus::TAUGHT);
		standing();
		return;
	}
	checkCollision(detatime);

	auto aladdin = SceneManager::getInstance()->getCurrentScene()->getObject(eObjectID::ALADDIN);
	auto aladdinPos = aladdin->getPhysicsComponent()->getPosition();

	float diffirent = _obj->getPhysicsComponent()->getPosition().x - aladdinPos.x;
	RECT bound = _obj->getPhysicsComponent()->getBounding();
	float width = bound.right - bound.left;

	if (diffirent > width && _obj->getPhysicsComponent()->getPositionX() >= _rangeXStart) // aladdin ở bến trái
	{
		setStatus(eStatus::RUNNING);
		moveLeft();
	}
	else if (diffirent < -width && _obj->getPhysicsComponent()->getPositionX() <= _rangeXEnd)
	{
		setStatus(eStatus::RUNNING);
		moveRight();
	}
	else
	{
		standing();
		setStatus(eStatus::TAUGHT);
		slash();
	}


	if (_obj->getAnimationComponent()->isTempAnimationEmpty() == true)
	{
		setWeapon(eStatus::TAUGHT);
	}

}

void NahbiBehaviorComponent::setStatus(eStatus status)
{
	BehaviorComponent::setStatus(status);
	updateAnimation();
}

void NahbiBehaviorComponent::dropHitpoint(int damage)
{
	EnemyBehaviorComponent::dropHitpoint(damage);
	_obj->getAnimationComponent()->setTempAnimation(eStatus::BEATEN, 1);
	_standTime = STAND_TIME;
}

void NahbiBehaviorComponent::checkCollision(float deltatime)
{
	auto active_object = SceneManager::getInstance()->getCurrentScene()->getActiveObject();
	_collisionComponent->reset();
	for (auto obj : active_object)
	{
		eObjectID id = obj->getID();
		switch (id)
		{
		case LAND:
			if (_collisionComponent->checkCollision(obj, deltatime, true))
			{
				if (((LandBehaviorComponent*)obj->getBehaviorComponent())->getLandType() == eLandType::lFLAME)
				{
					_obj->getAnimationComponent()->setTempAnimation(eStatus::BURNED, 1);
				}
			}
			break;
		}
	}
}

void NahbiBehaviorComponent::updateAnimation()
{
	switch (_status)
	{
	case TAUGHT:
		_obj->getAnimationComponent()->setAnimation(eStatus::TAUGHT);
		break;
	case RUNNING:
		_obj->getAnimationComponent()->setAnimation(eStatus::RUNNING);
		break;
	default:
		break;
	}
}


void NahbiBehaviorComponent::faceLeft()
{
	if (_obj->getAnimationComponent()->getScale().x > 0)
	{
		_obj->getAnimationComponent()->setScaleX(_obj->getAnimationComponent()->getScale().x * (-1));

		RECT bound = _obj->getPhysicsComponent()->getBounding();
		float width = bound.right - bound.left;
		_obj->getAnimationComponent()->setTranslateX(width);
	}
	setFacingDirection(eStatus::LEFTFACING);
}

void NahbiBehaviorComponent::faceRight()
{
	if (_obj->getAnimationComponent()->getScale().x < 0)
	{
		_obj->getAnimationComponent()->setScaleX(_obj->getAnimationComponent()->getScale().x * (-1));

		_obj->getAnimationComponent()->setTranslateX(0);
	}
	setFacingDirection(eStatus::RIGHTFACING);
}

void NahbiBehaviorComponent::standing()
{
	auto move = (Movement*)_obj->getPhysicsComponent()->getComponent("Movement");
	move->setVelocity(GVector2(0, 0));
}

void NahbiBehaviorComponent::moveLeft()
{
	faceLeft();
	auto move = (Movement*)_obj->getPhysicsComponent()->getComponent("Movement");
	move->setVelocity(GVector2(-_obj->getPhysicsComponent()->getMovingSpeed(), move->getVelocity().y));
	setFacingDirection(eStatus::LEFTFACING);
}

void NahbiBehaviorComponent::moveRight()
{
	faceRight();
	auto move = (Movement*)_obj->getPhysicsComponent()->getComponent("Movement");
	move->setVelocity(GVector2(_obj->getPhysicsComponent()->getMovingSpeed(), move->getVelocity().y));
	setFacingDirection(eStatus::RIGHTFACING);
}

void NahbiBehaviorComponent::slash()
{
	if (_weapon == eStatus::SLASH1)
		return;
	if (_weapon == eStatus::SLASH2)
		return;

	int random = rand() % 3;
	switch (random)
		{
		case 1:
			setWeapon(eStatus::SLASH1);
			_obj->getAnimationComponent()->setTempAnimation(eStatus::SLASH1, 1);
			break;
		case 2:
			setWeapon(eStatus::SLASH2);
			_obj->getAnimationComponent()->setTempAnimation(eStatus::SLASH2, 1);
			break;
		}
	auto pos = _obj->getPhysicsComponent()->getPosition();
	pos.y += _obj->getAnimationComponent()->getSprite()->getFrameHeight();
	float width = _obj->getAnimationComponent()->getSprite()->getFrameWidth() * 4 / 3;
	float height = _obj->getAnimationComponent()->getSprite()->getFrameHeight();
	if (_facingDirection == eStatus::RIGHTFACING)
	{
		pos += GVector2(_obj->getAnimationComponent()->getSprite()->getFrameWidth(), 0);
	}
	else
	{
		pos -= GVector2(_obj->getAnimationComponent()->getSprite()->getFrameWidth(), 0);
	}
	auto sword = ObjectFactory::getSword(pos, width, height, false);
	addToScene.Emit(sword);

}
