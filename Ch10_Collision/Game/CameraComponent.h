#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
    CameraComponent(class Actor* owner, int updateOrder = 200);
protected:
    // CameraComponent�� ���� Component���� ����� �Լ�.
    void SetViewMatrix(const Matrix4& view);
};