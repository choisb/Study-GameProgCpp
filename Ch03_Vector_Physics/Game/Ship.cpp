#pragma once
#include "Ship.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Laser.h"

Ship::Ship(Game* game)
    : Actor(game)
{
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("../Assets/Ship.png"));

	// Create InputComponent and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);
}

void Ship::UpdateActor(float deltaTime)
{
	mLaserCooldown -= deltaTime;
}

void Ship::ActorInput(const uint8_t* keyState)
{
	// �����̽��ٰ� ��������, cool down �ð��� �����ٸ� laser ����
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// �������� �����ϰ� �������� ��ġ�� ȸ������ ����
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// �������� cool down �ð� �缳��
		   
		mLaserCooldown = 0.5f;
	}
}
