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
// Game이 호출하는 코드
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
// Components들 업데이트 호출
void Actor::UpdateComponents(float deltaTime)
{
    for (auto comp : mComponents)
    {
        comp->Update(deltaTime);
    }
}
void Actor::UpdateActor(float deltaTime)
{
    // actor의 업데이트 코드
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
    
    // 진행 방향이 +X인가?
    if (dot > 0.9999f)
    {
        SetRotation(Quaternion::Identity);
    }
    // 진행 방향이 -X인가?
    else if (dot < -0.9999f)
    {
        SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
    }
    else
    {
        // 외적을 통해 얻은 축을 기준으로 회전하라
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
        // 스케일, 회전, 이동 행렬 순으로 결합해서 세계 변환 행렬 구함
        mWorldTransform = Matrix4::CreateScale(mScale);
        mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
        mWorldTransform *= Matrix4::CreateTranslation(mPosition);
        
        // 컴포넌트에 세계 변환이 갱신됐다고 통지
        for (auto comp : mComponents)
        {
            comp->OnUpdateWorldTransform();
        }
    }
}