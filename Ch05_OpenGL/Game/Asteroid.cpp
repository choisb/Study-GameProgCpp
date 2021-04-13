#include "Asteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Random.h"
#include "CircleComponent.h"

Asteroid::Asteroid(Game * game)
	:Actor(game)
{
	// �����ϰ� ��ġ�� ������ �ʱ�ȭ�Ѵ�.
    Vector2 randPos = Random::GetVector(Vector2(-512.0f, -384.0f),
        Vector2(512.0f, 384.0f));
	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// ��������Ʈ ������Ʈ�� �����ϰ� �ؽ�ó�� �����Ѵ�.
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("../Assets/Asteroid.png"));
	// �̵� ������Ʈ�� �����ϰ� ���� �ӵ��� �����Ѵ�.
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