#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"

// FMOD header를 include하는 것을 피하기 위해서 전방선언
namespace FMOD
{
    class System;
    namespace Studio
    {
        class Bank;
        class EventDescription;
        class EventInstance;
        class System;
        class Bus;
    };
};

class AudioSystem
{
public:
    AudioSystem(class Game* game);
    ~AudioSystem();

    bool Initialize();
    void Shutdown();

    // Load / Unload banks
    void LoadBank(const std::string& name);
    void UnloadBank(const std::string& name);
    void UnloadAllBanks();

    SoundEvent PlayEvent(const std::string& name);

    void Update(float deltaTime);

    // 위치 기반 오디오
    void SetListener(const Matrix4& viewMatrix, const Vector3& velocity);

    // 버스 제어를 위한 getter / setter
    float GetBusVolume(const std::string& name) const;
    bool GetBusPaused(const std::string& name) const;
    void SetBusVolume(const std::string& name, float volume);
    void SetBusPaused(const std::string& name, bool pause);

protected:
    // 여러 클래스가 이벤트 인스턴스에 접근하는 걸 막기 위해 
    // GetEventInstance 함수는 SoundEvent에서만 접근 할 수 있도록 friend 선언한다.
    friend class SoundEvent;
    FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);

private:
    // event instance의 정수 ID를 부여하기 위한 전역 변수
    // event instance가 생성될 때 마다 ID를 1씩 더하여 event instance에 부여한다.
    static unsigned int sNextID;

    class Game* mGame;
    // FMOD 스튜디오 시스템
    FMOD::Studio::System* mSystem;
    // FMOD 저수준 시스템 (거의 접근할 일은 없음)
    FMOD::System* mLowLevelSystem;

    // 로드된 뱅크를 관리하는 맵
    // key: 뱅크 파일 이름
    // value: 뱅크 포인터
    std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;

    // 이벤트 이름과 EventDescription 맵
    // key: FMOD가 할당한 이벤트 이름
    // value: EventDescription 포인터
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;

    // EventInstance를 관리하기 위한 이벤트 id 맵
    // key: 부호없는 정수형 id
    // value: EventInstance
    std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;

    // Bus를 관리하기 위한 맵
    // key: Bus의 이름
    // value: Bus의 포인터
    std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};

