#pragma once
#include "CameraComponent.h"

class FollowCamera : public CameraComponent
{
public:
    FollowCamera(class Actor* owner);
    void Update(float deltaTime) override;

    void SnapToIdeal();
       
    void SetHorzDist(float dist) { mHorzDist = dist; }
    void SetVertDist(float dist) { mVertDist = dist; }
    void SetTargetDist(float dist) { mTargetDist = dist; }
    void SetSpringConstant(float spring) { mSpringConstant = spring; }

private:
    Vector3 ComputeCameraPos() const;

    // 카메라의 실제 위치
    Vector3 mActualPos;
    // 실제 카메라의 속도
    Vector3 mVelocity;
    // 수평거리
    float mHorzDist;
    // 수직거리
    float mVertDist;
    // 타겟까지 거리
    float mTargetDist;
    // 스프링 상수(높을 수록 복원력이 강하다)
    float mSpringConstant;
};