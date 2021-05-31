#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
{

}
void MoveComponent::Update(float deltaTime)
{
	if(!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mOwner->GetRotation();
        float angle = mAngularSpeed * deltaTime;
        // 회전 증가분을 위한 쿼터니언을 생성
        // (상향축을 기준으로 회전)
        Quaternion inc(Vector3::UnitZ, angle);
        // 이전 쿼터니언과 새 쿼터니언을 연결한다.
        rot = Quaternion::Concatenate(rot, inc);
		mOwner->SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
        // 앞뒤 이동
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
        // 좌우 이동
        pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
        mOwner->SetPosition(pos);
	}
}