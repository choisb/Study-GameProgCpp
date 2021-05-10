#include "AudioComponent.h"
#include "Actor.h"
#include "Game.h"
#include "AudioSystem.h"
#include <SDL/SDL.h>

AudioComponent::AudioComponent(Actor* owner, int updateOrder)
    :Component(owner, updateOrder)
{

}

AudioComponent::~AudioComponent()
{
    StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
    Component::Update(deltaTime);

    // 유효하지 않은 2D event 삭제
    auto iter = mEvents2D.begin();
    while (iter != mEvents2D.end())
    {
        if (!iter->IsValid())
        {
            iter = mEvents2D.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    // 유효하지 않는 3D event 삭제
    iter = mEvents3D.begin();
    while (iter != mEvents3D.end())
    {
        if (!iter->IsValid())
        {
            iter = mEvents3D.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void AudioComponent::OnUpdateWorldTransform()
{
    Matrix4 world = mOwner->GetWorldTransform();
    for (auto& event : mEvents3D)
    {
        if (event.IsValid())
        {
            event.Set3DAttributes(world, mOwner->GetForward() * mOwner->GetForwardSpeed());
        }
    }
}

SoundEvent AudioComponent::PlayEvent(const std::string& name)
{
    SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
    // 이벤트의 3D 여부 확인
    if (e.Is3D())
    {
        mEvents3D.emplace_back(e);
        // 초기 3D 속성 설정
        e.Set3DAttributes(mOwner->GetWorldTransform(), mOwner->GetForward() * mOwner->GetForwardSpeed());
    }
    else
    {
        mEvents2D.emplace_back(e);
    }

    return e;
}

void AudioComponent::StopAllEvents()
{
    // 모든 사운드 정지
    for (auto& e : mEvents2D)
    {
        e.Stop();
    }
    for (auto& e : mEvents3D)
    {
        e.Stop();
    }

    // 모든 이벤트를 컨테이너에서 삭제한다.
    mEvents2D.clear();
    mEvents3D.clear();
}