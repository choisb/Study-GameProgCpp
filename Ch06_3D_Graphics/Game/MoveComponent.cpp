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
        // ȸ�� �������� ���� ���ʹϾ��� ����
        // (�������� �������� ȸ��)
        Quaternion inc(Vector3::UnitZ, angle);
        // ���� ���ʹϾ�� �� ���ʹϾ��� �����Ѵ�.
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