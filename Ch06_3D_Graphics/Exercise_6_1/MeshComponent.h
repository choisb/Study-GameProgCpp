#pragma once
#include "Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* owner);
    ~MeshComponent();
    // 제공된 셰이더로 이 메시 컴포턴트를 그린다.
    virtual void Draw(class Shader* shader);
    // 메시 컴포넌트가 사용하는 메시/텍스처를 설정
    virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }
    void SetTextureIndex(size_t index) { mTextureIndex = index; }
    const class Mesh* GetMesh() { return mMesh; }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
};