#include "Actor.h"
#include "Component.h"
#include "Game.h"

Actor::Actor(Game* game)
    :mState(EActive)
    ,mPosition(Vector3::Zero)
    ,mScale(1.0f)
    ,mRotation(Quaternion::Identity)
    ,mGame(game)
    ,mRecomputeWorldTransform(true)
{
    mGame->AddActor(this);
}
Actor::~Actor()
{
    mGame->RemoveActor(this);
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
}
// Game�� ȣ���ϴ� �ڵ�
void Actor::Update(float deltaTime)
{
    if (mState == EActive)
    {
        ComputeWorldTransform();

        UpdateComponents(deltaTime);
        UpdateActor(deltaTime);

        ComputeWorldTransform();
    }
}
// Components�� ������Ʈ ȣ��
void Actor::UpdateComponents(float deltaTime)
{
    for (auto comp : mComponents)
    {
        comp->Update(deltaTime);
    }
}
void Actor::UpdateActor(float deltaTime)
{
    // actor�� ������Ʈ �ڵ�
}
void Actor::AddComponent(Component* component)
{
    auto iter = mComponents.begin();
    for (; iter != mComponents.end(); iter++)
    {
        if ((*iter)->GetUpdateOrder() > component->GetUpdateOrder())
            break;
    }
    mComponents.insert(iter, component);
}
void Actor::RemoveComponent(Component* component)
{
    auto iter = std::find(mComponents.begin(), mComponents.end(), component);
    if (iter != mComponents.end())
    {
        mComponents.erase(iter);
    }
}
void Actor::ProcessInput(const uint8_t* keyState)
{
	if (mState == EActive)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}
		ActorInput(keyState);
	}
}
void Actor::RotateToNewForward(const Vector3& forward)
{
    float dot = Vector3::Dot(Vector3::UnitX, forward);
    float angle = Math::Acos(dot);
    
    // ���� ������ +X�ΰ�?
    if (dot > 0.9999f)
    {
        SetRotation(Quaternion::Identity);
    }
    // ���� ������ -X�ΰ�?
    else if (dot < -0.9999f)
    {
        SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
    }
    else
    {
        // ������ ���� ���� ���� �������� ȸ���϶�
        Vector3 axis = Vector3::Cross(Vector3::UnitX, forward);
        axis.Normalize();
        SetRotation(Quaternion(axis, angle));
    }
}
void Actor::ComputeWorldTransform()
{
    if (mRecomputeWorldTransform)
    {
        mRecomputeWorldTransform = false;
        // ������, ȸ��, �̵� ��� ������ �����ؼ� ���� ��ȯ ��� ����
        mWorldTransform = Matrix4::CreateScale(mScale);
        mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
        mWorldTransform *= Matrix4::CreateTranslation(mPosition);
        
        // ������Ʈ�� ���� ��ȯ�� ���ŵƴٰ� ����
        for (auto comp : mComponents)
        {
            comp->OnUpdateWorldTransform();
        }
    }
}