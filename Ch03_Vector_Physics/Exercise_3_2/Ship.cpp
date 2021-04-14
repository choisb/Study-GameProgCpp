#pragma once
#include "Ship.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Laser.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Ship::Ship(Game* game)
    : Actor(game)
{
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("../Assets/ShipWithThrust.png"));

	// Create InputComponent and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);

    // �浹 �˻縦 ���ؼ� CircleComponent �߰��� ������ ����
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(30.0f);
}

void Ship::UpdateActor(float deltaTime)
{
	mLaserCooldown -= deltaTime;
	mRespawnTime -= deltaTime;
	mInvincibleTime -= deltaTime;

    // ������ �ð��� �������� ��� Active ���°� �� �� ����.
	if (mRespawnTime >= 0)
		return;

	SetState(EActive);

	// ��Ȱ�� �����ð��� �������� ��� � ���� �˻� ����
	if (mInvincibleTime >= 0)
		return;

	// ��� �������� �˻�
	for (auto ast : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// ���ּ��� ��� �����Ѵٸ�
			// ���ּ��� paused ���·� ����
			SetState(EPaused);
			SetPosition(Vector2(512.0f, 386.0f));
            SetRotation(0.0f);
			mRespawnTime = 1.5f;
			mInvincibleTime = 3.0f;
			break;
		}
	}
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