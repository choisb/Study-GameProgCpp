#pragma once
#include "MeshComponent.h"
class SkeletalMeshComponent : public MeshComponent
{
public:
    SkeletalMeshComponent(class Actor* owner);
    // �޽�������Ʈ�� �׸���.
    void Draw(class Shader* shader) override;
};

