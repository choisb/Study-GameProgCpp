#pragma once
#include "Math.h"
#include "Component.h"

class PointLightComponent : public Component
{
public:
    PointLightComponent(class Actor* owner);
    ~PointLightComponent();

    // �� ������ ���ϸ� �׸���.
    void Draw(class Shader* shader, class Mesh* mesh);

    // �л걤 ����
    Vector3 mDiffuseColor;
    // ������ ������
    float mInnerRadius;
    float mOuterRadius;
};
