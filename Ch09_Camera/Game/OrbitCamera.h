#pragma once
#include "CameraComponent.h"

class OrbitCamera : public CameraComponent
{
public:
    OrbitCamera(class Actor* owner);

    void Update(float deltaTime) override;

    float GetPitchSpeed() const { return mPitchSpeed; }
    float GetYawSpeed() const { return mYawSpeed; }

    void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
    void SetYawSpeed(float speed) { mYawSpeed = speed; }

private:
    // ������ ������
    Vector3 mOffset;
    // ī�޶� ���� ����
    Vector3 mUp;
    // ��ġ�� �ʴ� ȸ�� �ӵ�
    float mPitchSpeed;
    // ���� �ʴ� ȸ�� �ӵ�
    float mYawSpeed;

};