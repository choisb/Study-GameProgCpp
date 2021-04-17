#include "Asteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Random.h"
#include "CircleComponent.h"

Asteroid::Asteroid(Game * game)
	:Actor(game)
{
	// 랜덤하게 위치와 방향을 초기화한다.
    Vector2 randPos = Random::GetVector(Vector2(-512.0f, -384.0f),
        Vector2(512.0f, 384.0f));
	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// 스프라이트 컴포넌트를 생성하고 텍스처를 설정한다.
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("../Assets/Asteroid.png"));
	// 이동 컴포넌트를 생성하고 전방 속도를 설정한다.
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(150.0f);

	mCircle = new CircleComponent(this);
	mCircle->SetRadius(40.0f);

	GetGame()->AddAsteroid(this);
}
Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}