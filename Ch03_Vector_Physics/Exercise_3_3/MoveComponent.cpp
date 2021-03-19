#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mMass(1.0f)
	, mForces(Vector2(0.0f, 0.0f))
	, mAccele(Vector2(0.0f, 0.0f))
	, mVelocity(Vector2(0.0f, 0.0f))
	, GA(Vector2(0.1f, 9.8f))
	, mAirResistance(0.08f)
{

}
void MoveComponent::Update(float deltaTime)
{
	// 매 프레임마다 중력 작용
	AddForce(GA * mMass);
	// 매 프레임마다 공기저항력 작용
	AddForce(mVelocity * mAirResistance * (-1.0f));
	mAccele = mForces * (1/mMass);
	mVelocity += mAccele;
	// 속도벡터로 actor의 회전값 계산.
	float rot = Math::PiOver2 - Math::Atan2(mVelocity.x, -mVelocity.y);
	mOwner->SetRotation(rot);

	
	Vector2 pos = mOwner->GetPosition();
	//SDL의 y좌표계는 아래쪽 방향이 양의방향이기 때문에 y값은 빼준다
	pos.x += mVelocity.x * deltaTime;
	pos.y += mVelocity.y * deltaTime;
	mOwner->SetPosition(pos);

	mForces = Vector2(0.0f, 0.0f);
}