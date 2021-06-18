#pragma once
#include "Component.h"
#include "Collision.h"
class BoxComponent : public Component
{
public:
    BoxComponent(class Actor* owner, int updateOrder = 100);
    ~BoxComponent();
    void OnUpdateWorldTransform() override;
    void SetObjectBox(const AABB& model) { mObjectBox = model; }
    const AABB& GetWorldBox() const { return mWorldBox; }
    void SetShouldRotate(bool value) { mShouldRotate = value; }

private:
    // 오브젝트 공간 경계를 위한 AABB
    AABB mObjectBox;
    // 세계 공간 경계를 위한 AABB
    AABB mWorldBox;

    bool mShouldRotate;
};