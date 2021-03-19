#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent
{
public:
	InputComponent(class Actor* owner);
	void ProcessInput(const uint8_t* keyState) override;
	
	// Getters/setters for private variables
	Vector2 GetFireForce() const { return mFireForce; }
	int GetFireKey() const { return mFireKey; }
	
	void SetFireForce(Vector2 force) { mFireForce = force; }
	void SetFireKey(int key) { mFireKey = key; }

private:
	// �߻��Ҷ� ������ ��
	Vector2 mFireForce;
	// �߻縦 ���� Ű
	int mFireKey;
	bool isFired;


};