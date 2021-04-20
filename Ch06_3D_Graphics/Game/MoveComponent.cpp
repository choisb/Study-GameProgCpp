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

	if (!Math::NearZero(mForwardSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;

		// Screen wrapping code only for asteroids
        if (pos.x < -512.0f) { pos.x = 510.0f; }
        else if (pos.x > 512.0f) { pos.x = -510.0f; }
        if (pos.y < -384.0f) { pos.y = 382.0f; }
        else if (pos.y > 384.0f) { pos.y = -382.0f; }
        mOwner->SetPosition(pos);
	}
}