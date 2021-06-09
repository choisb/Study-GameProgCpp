#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
    CameraComponent(class Actor* owner, int updateOrder = 200);
protected:
    // CameraComponent의 서브 Component에서 사용할 함수.
    void SetViewMatrix(const Matrix4& view);
};