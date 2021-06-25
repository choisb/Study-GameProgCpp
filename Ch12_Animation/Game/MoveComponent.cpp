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

	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
        // �յ� �̵�
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
        // �¿� �̵�
        pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
        mOwner->SetPosition(pos);
	}
}