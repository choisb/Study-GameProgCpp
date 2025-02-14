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

    // 충돌 검사를 위해서 CircleComponent 추가와 반지름 설정
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(30.0f);
}

void Ship::UpdateActor(float deltaTime)
{
	mLaserCooldown -= deltaTime;
	mRespawnTime -= deltaTime;
	mInvincibleTime -= deltaTime;

    // 리스폰 시간이 남아있을 경우 Active 상태가 될 수 없다.
	if (mRespawnTime >= 0)
		return;

	SetState(EActive);

	// 부활후 무적시간이 남아있을 경우 운석 교차 검사 안함
	if (mInvincibleTime >= 0)
		return;

	// 운석과 교차여부 검사
	for (auto ast : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// 우주선과 운석이 교차한다면
			// 우주선은 paused 상태로 설정
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

	// 스페이스바가 눌러졌고, cool down 시간이 지났다면 laser 생성
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// 레이저를 생성하고 레이저의 위치와 회전값을 설정
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// 레이저의 cool down 시간 재설정
		   
		mLaserCooldown = 0.5f;
	}
}
