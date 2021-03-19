#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
public:
	// ���� ������Ʈ�ǵ��� ���� �������� ����
	MoveComponent(class Actor* owner, int updateOrder = 10);

	void Update(float deltaTime) override;

	// ���������� ���� ����Լ�
	void AddForce(Vector2 force) { mForces += force; }

	float GetMass() const { return mMass; }
	Vector2 GetForces() const { return mForces; }
	Vector2 GetVelocity() const { return mVelocity; }
	float GetairResistance() const { return mAirResistance; }

	void SetMass(float mass) { mMass = mass; }
	void SetForces(Vector2 forces) { mForces = forces; }
	void SetVelocity(Vector2 velocity) { mVelocity = velocity; }


private:
	float mMass;
	Vector2 mForces;
	Vector2 mAccele;
	Vector2 mVelocity;

	// �߷� ���ӵ�(gravitational acceleration)
	const Vector2 GA;
	// ���� ����
	const float mAirResistance;
};