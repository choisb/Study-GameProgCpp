#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mMass(1.0f)
	, mForces(Vector2(0.0f, 0.0f))
	, mAccele(Vector2(0.0f, 0.0f))
	, mVelocity(Vector2(0.0f, 0.0f))
	, GA(Vector2(0.0f, 300.0f)) // �߷°��ӵ��� ȭ�� �ϴ� �������� 300.0f�� ���� ����
	, mAirResistance(0.01f)     // �������� ����� ���Ƿ� 0.01f�� ����
{

}
void MoveComponent::Update(float deltaTime)
{
	// �� �����Ӹ��� �߷� �ۿ�
	AddForce(GA * mMass);
	// �� �����Ӹ��� �������׷� �ۿ�
	AddForce(mVelocity * mAirResistance * (-1.0f));
    // ���ӵ� = ���� �ѷ� / ����
	mAccele = mForces * (1/mMass);
    // �ӵ� = ���ӵ� * �ð�
	mVelocity += mAccele * deltaTime;
	// �ӵ����ͷ� actor�� ȸ���� ���. SDL�� y ��ǥ�� �Ʒ����� ����̱� ������ y�� ����
	float rot = Math::Atan2(-mVelocity.y, mVelocity.x);
	mOwner->SetRotation(rot);

	
	Vector2 pos = mOwner->GetPosition();
	//SDL�� y��ǥ��� �Ʒ��� ������ ���ǹ����̱� ������ y���� ���ش�
	pos.x += mVelocity.x * deltaTime;
	pos.y += mVelocity.y * deltaTime;
	mOwner->SetPosition(pos);

	mForces = Vector2(0.0f, 0.0f);
}