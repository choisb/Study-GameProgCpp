#pragma once
#include "Math.h"
#include "Component.h"

class PointLightComponent : public Component
{
public:
    PointLightComponent(class Actor* owner);
    ~PointLightComponent();

    // 이 점광의 기하를 그린다.
    void Draw(class Shader* shader, class Mesh* mesh);

    // 분산광 색상
    Vector3 mDiffuseColor;
    // 광원의 반지름
    float mInnerRadius;
    float mOuterRadius;
};
