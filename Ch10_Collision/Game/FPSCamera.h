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
    // 피치의 초당 회전 속도
    float mPitchSpeed;
    // 소유자의 전방 벡터에서 멀어질 수 있는 최대 피치 값
    float mMaxPitch;
    // 현재 피치 값
    float mPitch;
};