#include "Laser.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Laser::Laser(Game* game)
	:Actor(game)
	, mDeathTimer(1.0f)
{
	// sprite component 생성
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("../Assets/Laser.png"));

	// move component 생성 및 forward speed 설정
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(800.0f);

	// 출돌검사를 위한 circle component 생성
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(11.0f);
}

void Laser::UpdateActor(float deltaTime)
{
	// time out 발생시 leaser는 dead 상태로
	mDeathTimer -= deltaTime;
	if (mDeathTimer <= 0.0f)
	{
		SetState(EDead);
	}
	else
	{
		// 운석과 교차여부 검사
		for (auto ast : GetGame()->GetAsteroids())
		{
			if (Intersect(*mCircle, *(ast->GetCircle())))
			{
				// 레이저와 운석이 교차한다면
				// 레이저와 운석은 dead 상태로 설정
				SetState(EDead);
				ast->SetState(EDead);
				break;
			}
		}
	}
}