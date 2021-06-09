#pragma once
#include "CameraComponent.h"

class FPSCamera : public CameraComponent
{
public:
    FPSCamera(class Actor* owner);

    void Update(float deltaTime) override;

    float GetPitch() const { return mPitch; }
    float GetPitchSpeed() const { return mPitchSpeed; }
    float GetMaxPitch() const { return mMaxPitch; }

    void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
    void SetMaxPitch(float pitch) { mMaxPitch = pitch; }

private:
    // ��ġ�� �ʴ� ȸ�� �ӵ�
    float mPitchSpeed;
    // �������� ���� ���Ϳ��� �־��� �� �ִ� �ִ� ��ġ ��
    float mMaxPitch;
    // ���� ��ġ ��
    float mPitch;
};