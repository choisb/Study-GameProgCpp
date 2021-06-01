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

    // ī�޶��� ���� ��ġ
    Vector3 mActualPos;
    // ���� ī�޶��� �ӵ�
    Vector3 mVelocity;
    // ����Ÿ�
    float mHorzDist;
    // �����Ÿ�
    float mVertDist;
    // Ÿ�ٱ��� �Ÿ�
    float mTargetDist;
    // ������ ���(���� ���� �������� ���ϴ�)
    float mSpringConstant;
};