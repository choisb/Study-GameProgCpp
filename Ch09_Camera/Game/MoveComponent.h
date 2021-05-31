#pragma once
#include "Component.h"

class MoveComponent : public Component
{
public:
	// ���� ������Ʈ�ǵ��� ���� �������� ����
	MoveComponent(class Actor* owner, int updateOrder = 10);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
    float GetForwardSpeed() const { return mForwardSpeed; }
    float GetStrafeSpeed() const { return mStrafeSpeed; }
    void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }

private:
	// ȸ�� ���ӵ�(�ʴ� ����)
	float mAngularSpeed;
	// ���� �̵� �ӵ�(�ʴ� ����)
	float mForwardSpeed;
    // �¿� �̵� �ӵ�(�ʴ� ����)
    float mStrafeSpeed;


};