#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
public:
	// 먼저 업데이트되도록 갱신 순서값을 낮춤
	MoveComponent(class Actor* owner, int updateOrder = 10);

	void Update(float deltaTime) override;

	// 물리연산을 위한 멤버함수
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

	// 중력 가속도(gravitational acceleration)
	const Vector2 GA;
	// 공기 저항
	const float mAirResistance;
};