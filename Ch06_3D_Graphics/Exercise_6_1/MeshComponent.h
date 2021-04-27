#pragma once
#include "Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* owner);
    ~MeshComponent();
    // ������ ���̴��� �� �޽� ������Ʈ�� �׸���.
    virtual void Draw(class Shader* shader);
    // �޽� ������Ʈ�� ����ϴ� �޽�/�ؽ�ó�� ����
    virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }
    void SetTextureIndex(size_t index) { mTextureIndex = index; }
    const class Mesh* GetMesh() { return mMesh; }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
};