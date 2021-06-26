#pragma once
#include "MeshComponent.h"
class SkeletalMeshComponent : public MeshComponent
{
public:
    SkeletalMeshComponent(class Actor* owner);
    // 메시컴포넌트를 그린다.
    void Draw(class Shader* shader) override;
};

