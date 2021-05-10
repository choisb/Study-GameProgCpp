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

    // ��ȿ���� ���� 2D event ����
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

    // ��ȿ���� �ʴ� 3D event ����
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
    // �̺�Ʈ�� 3D ���� Ȯ��
    if (e.Is3D())
    {
        mEvents3D.emplace_back(e);
        // �ʱ� 3D �Ӽ� ����
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
    // ��� ���� ����
    for (auto& e : mEvents2D)
    {
        e.Stop();
    }
    for (auto& e : mEvents3D)
    {
        e.Stop();
    }

    // ��� �̺�Ʈ�� �����̳ʿ��� �����Ѵ�.
    mEvents2D.clear();
    mEvents3D.clear();
}