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
	// �� �����Ӹ��� �߷� �ۿ�
	AddForce(GA * mMass);
	// �� �����Ӹ��� �������׷� �ۿ�
	AddForce(mVelocity * mAirResistance * (-1.0f));
	mAccele = mForces * (1/mMass);
	mVelocity += mAccele;
	// �ӵ����ͷ� actor�� ȸ���� ���.
	float rot = Math::PiOver2 - Math::Atan2(mVelocity.x, -mVelocity.y);
	mOwner->SetRotation(rot);

	
	Vector2 pos = mOwner->GetPosition();
	//SDL�� y��ǥ��� �Ʒ��� ������ ���ǹ����̱� ������ y���� ���ش�
	pos.x += mVelocity.x * deltaTime;
	pos.y += mVelocity.y * deltaTime;
	mOwner->SetPosition(pos);

	mForces = Vector2(0.0f, 0.0f);
}