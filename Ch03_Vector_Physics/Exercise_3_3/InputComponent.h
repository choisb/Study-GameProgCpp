#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent
{
public:
	InputComponent(class Actor* owner);
	void ProcessInput(const uint8_t* keyState) override;
	
	// Getters/setters for private variables���
	Vector2 GetDForcee() const { return mDForce; }
	int GetFireKey() const { return mForceKey; }
	
	void SetDForce(Vector2 force) { mDForce = force; }
	void SetForceKey(int key) { mForceKey = key; }

private:
	// ������(Driving Forc)
	Vector2 mDForce;
	// �������� �Է��ϴ� Ű
	int mForceKey;


};