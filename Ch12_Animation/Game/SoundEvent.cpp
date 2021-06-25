#include "SoundEvent.h"
#include "AudioSystem.h"
#include <fmod_studio.hpp>

SoundEvent::SoundEvent(class AudioSystem* system, unsigned int id)
    :mSystem(system)
    ,mID(id)
{
}
SoundEvent::SoundEvent()
    : mSystem(nullptr)
    , mID(0)
{
}

bool SoundEvent::IsValid()
{
    return (mSystem && mSystem->GetEventInstance(mID) != nullptr);
}

void SoundEvent::Restart()
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->start();
    }
}
void SoundEvent::Stop(bool allowFadeOut /* true */)
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_STUDIO_STOP_MODE mode = allowFadeOut ?
            FMOD_STUDIO_STOP_ALLOWFADEOUT :
            FMOD_STUDIO_STOP_IMMEDIATE;
        event->stop(mode);
    }
}
void SoundEvent::SetPaused(bool pause)
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->setPaused(pause);
    }
}
void SoundEvent::SetVolume(float value)
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->setVolume(value);
    }
}

void SoundEvent::SetPitch(float value)
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->setPitch(value);
    }
}
void SoundEvent::SetParameter(const std::string& name, float value)
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->setParameterValue(name.c_str(), value);
    }
}

bool SoundEvent::GetPaused() const
{
    bool retVal = false;
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->getPaused(&retVal);
    }
    return retVal;
}

float SoundEvent::GetVolume() const
{
    float retVal = 0.0f;
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->getVolume(&retVal);
    }
    return retVal;
}

float SoundEvent::GetPitch() const
{
    float retVal = 0.0f;
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->getPitch(&retVal);
    }
    return retVal;
}

float SoundEvent::GetParameter(const std::string& name)
{
    float retVal = 0.0f;
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        event->getParameterValue(name.c_str(), &retVal);
    }
    return retVal;
}

bool SoundEvent::Is3D() const
{
    bool retVal = false;
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        // 이벤트 디스크립션을 얻는다.
        FMOD::Studio::EventDescription* ed = nullptr;
        event->getDescription(&ed);
        if (ed)
        {
            ed->is3D(&retVal);
        }
    }
    return retVal;
}

namespace
{
    FMOD_VECTOR VecToFMOD(const Vector3& in)
    {
        // 게임 좌표 (+x 전방, +y 오른쪽, +z 위쪽)를
        // FMOD (+z 전방, +x 오른쪽, +y 위쪽) 좌표로 변환
        FMOD_VECTOR v;
        v.x = in.y;
        v.y = in.z;
        v.z = in.x;
        return v;
    }
}

void SoundEvent::Set3DAttributes(const Matrix4& worldTrans)
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_3D_ATTRIBUTES attr;
        // Set position, forward, up
        attr.position = VecToFMOD(worldTrans.GetTranslation());
        // 세계 공간에서 첫 번째 행은 전방 벡터
        attr.forward = VecToFMOD(worldTrans.GetXAxis());
        // 세 번째 행은 상향 벡터
        attr.up = VecToFMOD(worldTrans.GetZAxis());
        // 속도를 0으로 설정 (도플러 효과를 사용한다면 수정)
        attr.velocity = { 0.0f, 0.0f, 0.0f };
        event->set3DAttributes(&attr);
    }
}