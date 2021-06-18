#include "BoxComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"

BoxComponent::BoxComponent(Actor* owner, int updateOrder)
    :Component(owner, updateOrder)
    , mObjectBox(Vector3::Zero, Vector3::Zero)
    , mWorldBox(Vector3::Zero, Vector3::Zero)
    , mShouldRotate(true)
{
    mOwner->GetGame()->GetPhysWorld()->AddBox(this);
}
void BoxComponent::OnUpdateWorldTransform()
{
    // ������Ʈ ������ �ٿ�� �ڽ��� ����
    mWorldBox = mObjectBox;
    // ������
    mWorldBox.mMin *= mOwner->GetScale();
    mWorldBox.mMax *= mOwner->GetScale();
    // ȸ��
    if (mShouldRotate)
    {
        mWorldBox.Rotate(mOwner->GetRotation());
    }
    // �̵�
    mWorldBox.mMin += mOwner->GetPosition();
    mWorldBox.mMax += mOwner->GetPosition();
}
BoxComponent::~BoxComponent()
{
    mOwner->GetGame()->GetPhysWorld()->RemoveBox(this);
}