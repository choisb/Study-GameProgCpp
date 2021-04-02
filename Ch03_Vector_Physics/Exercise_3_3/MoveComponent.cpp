#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mMass(1.0f)
	, mForces(Vector2(0.0f, 0.0f))
	, mAccele(Vector2(0.0f, 0.0f))
	, mVelocity(Vector2(0.0f, 0.0f))
	, GA(Vector2(0.0f, 300.0f)) // 중력가속도를 화면 하단 방향으로 300.0f로 임의 설정
	, mAirResistance(0.01f)     // 공기저항 계수를 임의로 0.01f로 설정
{

}
void MoveComponent::Update(float deltaTime)
{
	// 매 프레임마다 중력 작용
	AddForce(GA * mMass);
	// 매 프레임마다 공기저항력 작용
	AddForce(mVelocity * mAirResistance * (-1.0f));
    // 가속도 = 힘의 총량 / 질량
	mAccele = mForces * (1/mMass);
    // 속도 = 가속도 * 시간
	mVelocity += mAccele * deltaTime;
	// 속도벡터로 actor의 회전값 계산. SDL의 y 좌표는 아랫쪽이 양수이기 때문에 y값 반전
	float rot = Math::Atan2(-mVelocity.y, mVelocity.x);
	mOwner->SetRotation(rot);

	
	Vector2 pos = mOwner->GetPosition();
	//SDL의 y좌표계는 아래쪽 방향이 양의방향이기 때문에 y값은 빼준다
	pos.x += mVelocity.x * deltaTime;
	pos.y += mVelocity.y * deltaTime;
	mOwner->SetPosition(pos);

	mForces = Vector2(0.0f, 0.0f);
}