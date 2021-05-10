#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"

// FMOD header�� include�ϴ� ���� ���ϱ� ���ؼ� ���漱��
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

    // ��ġ ��� �����
    void SetListener(const Matrix4& viewMatrix, const Vector3& velocity);

    // ���� ��� ���� getter / setter
    float GetBusVolume(const std::string& name) const;
    bool GetBusPaused(const std::string& name) const;
    void SetBusVolume(const std::string& name, float volume);
    void SetBusPaused(const std::string& name, bool pause);

protected:
    // ���� Ŭ������ �̺�Ʈ �ν��Ͻ��� �����ϴ� �� ���� ���� 
    // GetEventInstance �Լ��� SoundEvent������ ���� �� �� �ֵ��� friend �����Ѵ�.
    friend class SoundEvent;
    FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);

private:
    // event instance�� ���� ID�� �ο��ϱ� ���� ���� ����
    // event instance�� ������ �� ���� ID�� 1�� ���Ͽ� event instance�� �ο��Ѵ�.
    static unsigned int sNextID;

    class Game* mGame;
    // FMOD ��Ʃ��� �ý���
    FMOD::Studio::System* mSystem;
    // FMOD ������ �ý��� (���� ������ ���� ����)
    FMOD::System* mLowLevelSystem;

    // �ε�� ��ũ�� �����ϴ� ��
    // key: ��ũ ���� �̸�
    // value: ��ũ ������
    std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;

    // �̺�Ʈ �̸��� EventDescription ��
    // key: FMOD�� �Ҵ��� �̺�Ʈ �̸�
    // value: EventDescription ������
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;

    // EventInstance�� �����ϱ� ���� �̺�Ʈ id ��
    // key: ��ȣ���� ������ id
    // value: EventInstance
    std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;

    // Bus�� �����ϱ� ���� ��
    // key: Bus�� �̸�
    // value: Bus�� ������
    std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};

