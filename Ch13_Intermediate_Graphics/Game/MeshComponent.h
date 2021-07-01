#pragma once
#include "Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* owner, bool isSkeletal);
    ~MeshComponent();
    // ������ ���̴��� �� �޽� ������Ʈ�� �׸���.
    virtual void Draw(class Shader* shader);
    // �޽� ������Ʈ�� ����ϴ� �޽�/�ؽ�ó�� ����
    virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }
    void SetTextureIndex(size_t index) { mTextureIndex = index; }
 
    void SetVisible(bool visible) { mVisible = visible;}
    bool GetVisible() const { return mVisible; }

    bool GetIsSkeletal() const { return mIsSkeletal; }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
    bool mVisible;
    bool mIsSkeletal;
};