#include <SDL/SDL_log.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <vector>

#include "AudioSystem.h"
#include "SoundEvent.h"

unsigned int AudioSystem::sNextID = 0;

AudioSystem::AudioSystem(Game* game)
    :mGame(game)
    ,mSystem(nullptr)
    ,mLowLevelSystem(nullptr)
{
    
}
AudioSystem::~AudioSystem()
{

}

bool AudioSystem::Initialize()
{
    // ������ ���� �α��� �����ϱ� ���� Debug_Initialize �Լ� ȣ��
    FMOD::Debug_Initialize(
        FMOD_DEBUG_LEVEL_ERROR, // �α� ��� ����: ������ ��쿡�� �α׸� ����
        FMOD_DEBUG_MODE_TTY     // �α� ��� ��ġ: stdout���� �α� ���
    );

    // FMOD ��Ʃ��� �ý����� �ν��Ͻ� ����
    FMOD_RESULT result;
    result = FMOD::Studio::System::create(&mSystem);
    // FMOD�� �Ϲ������� �Լ��� ������� ��ȯ�Ͽ� ���������� ���� �Ǿ����� �˷��ش�.
    if (result != FMOD_OK)  
    {
        SDL_Log("Failed to create FMOD system %s", FMOD_ErrorString(result));
        return false;
    }
   
    // FMOD �ý����� initialize �Լ� ȣ��
    result = mSystem->initialize(
        512,                        // ���ÿ� ����� �� �ִ� ������ �ִ� ����
        FMOD_STUDIO_INIT_NORMAL,    // �⺻ ���� ���
        FMOD_INIT_NORMAL,           // �⺻ ����
        nullptr                     // ��κ� nullptr
    );
    if (result != FMOD_OK)
    {
        SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
        return false;
    }

    // �������� �ý��� �����͸� ���ͼ� mLowLevelSyste�� �����Ѵ�.
    result = mSystem->getLowLevelSystem(&mLowLevelSystem);
    if (result != FMOD_OK)
    {
        SDL_Log("Failed to create FMOD system %s", FMOD_ErrorString(result));
        return false;
    }

    mLowLevelSystem->set3DSettings(
        1.f,    // ���÷� ������, 1 = ����, 1���� �� ũ�� ����� �Ҹ��� ����
        50.f,   // ���� ������ ũ�� = 1���� (7�� ������Ʈ�� 50�� ������)
        1.f     // (���÷��� �������, 1�� ���ܵд�)
    );


    // ��� FMOD ��Ʃ��� ������Ʈ�� �Ʒ��� �� ��ũ�� �⺻ ��ũ�� �ݵ�� ������ �ִ�.
    // �Ʒ��� �� ��ũ�� �ε����� �ʴ´ٸ� �ٸ� ��ũ�� �̺�Ʈ�� �׼��� �� �� ����.
    // ���� �ʱ�ȭ �ܰ迡�� �Ʒ��� �� ��ũ�� �ε�������.
    // �׸��� Master Bank.strings.bank�� ���� �ε�����.
    LoadBank("../Assets/Master Bank.strings.bank");
    LoadBank("../Assets/Master Bank.bank");

    return true;
}

void AudioSystem::Shutdown()
{
    mSystem->release();
}

void AudioSystem::LoadBank(const std::string& name)
{
    // �� �� �ε����� �ʰ� �Ѵ�.
    if (mBanks.find(name) != mBanks.end())
    {
        return;
    }

    // ��ũ �ε�
    FMOD::Studio::Bank* bank = nullptr;
    FMOD_RESULT result = mSystem->loadBankFile(
        name.c_str(),                   // ��ũ�� ���� �̸�
        FMOD_STUDIO_LOAD_BANK_NORMAL,   // �Ϲ����� ������� �ε�
        &bank                           // ��ũ ������ ����
    );
    if (result != FMOD_OK)
    {
        SDL_Log("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    }
    const int maxPathLength = 512;
    if (result == FMOD_OK)
    {
        // ��ũ�� �ʿ� �߰�
        mBanks.emplace(name, bank);
        // ��Ʈ���� ������ �ƴ� ��� ���� �����͸� �ε�
        bank->loadSampleData();
        // ��ũ�� �̺�Ʈ ���� ��´�
        int numEvents = 0;
        bank->getEventCount(&numEvents);
        if (numEvents > 0) 
        {
            // ��ũ���� �̺�Ʈ ��ũ���� ����Ʈ�� ��´�
            // (��ũ�� �̺�Ʈ ���� ���� ������ events vector�� ����)
            std::vector<FMOD::Studio::EventDescription*> events(numEvents);
            bank->getEventList(events.data(), numEvents, &numEvents);
            char eventName[maxPathLength];
            for (auto e : events)
            {
                // event:/Explosion2D ���� �̺�Ʈ�� ��θ� ��´�.
                e->getPath(eventName, maxPathLength, nullptr);
                // �̺�Ʈ�� �ʿ� �߰��Ѵ�.
                mEvents.emplace(eventName, e);
            }
        }
    }

    // ��ũ�� �ִ� �������� �ε�
    int numBuses = 0;
    bank->getBusCount(&numBuses);
    if (numBuses > 0)
    {
        // ��ũ�� �ִ� ������ ����Ʈ�� �����´�
        std::vector<FMOD::Studio::Bus*> buses(numBuses);
        bank->getBusList(buses.data(), numBuses, &numBuses);
        char busName[512];
        for (auto bus : buses)
        {
            // ������ ��θ� �����´�. (���� ��� bus:/SFX �� ����)
            bus->getPath(busName, 512, nullptr);
            // ������ �ʿ� �߰��Ѵ�.
            mBuses.emplace(busName, bus);
        }
    }
}

void AudioSystem::UnloadBank(const std::string& name)
{
    // �ε� ���� �ʾҴٸ� ����
    auto iter = mBanks.find(name);
    if (iter != mBanks.end())
    {
        return;
    }

    // ���� �� ��ũ�� ��� �̺�Ʈ�� �����Ѵ�.
    FMOD::Studio::Bank* bank = iter->second;
    int numEvents = 0;
    bank->getEventCount(&numEvents);
    if (numEvents > 0)
    {
        // �� ��ũ���� event descriptions���� �����´�.
        std::vector<FMOD::Studio::EventDescription*> events(numEvents);
        // �̺�Ʈ�� ����Ʈ�� �����´�.
        bank->getEventList(events.data(), numEvents, &numEvents);
        char eventName[512];
        for (auto e : events)
        {
            // �� �̺�Ʈ�� ��θ� �����´�
            e->getPath(eventName, 512, nullptr);
            // �� �̺�Ʈ�� �����Ѵ�.
            auto eventi = mEvents.find(eventName);
            if (eventi != mEvents.end())
            {
                mEvents.erase(eventi);
            }
        }
    }

    // �� ��ũ�� ��� ������ �����Ѵ�.
    int numBuses = 0;
    bank->getBusCount(&numBuses);
    if (numBuses > 0)
    {
        // �� ��ũ�� ��� ���� ����Ʈ�� �����´�.
        std::vector<FMOD::Studio::Bus*> buses(numBuses);
        bank->getBusList(buses.data(), numBuses, &numBuses);
        char busName[512];
        for (auto bus : buses)
        {
            // ������ ��θ� �����´�. (���� ��� bus:/SFX �� ����)
            bus->getPath(busName, 512, nullptr);
            // �� ������ �����Ѵ�.
            auto busi = mBuses.find(busName);
            if (busi != mBuses.end())
            {
                mBuses.erase(busi);
            }
        }
    }

    // ���� �����Ϳ� ��ũ�� unload
    bank->unloadSampleData();
    bank->unload();
    // banks map���� bank ����
    mBanks.erase(iter);
}
void AudioSystem::UnloadAllBanks()
{
    for (auto& iter : mBanks)
    {
        iter.second->unloadSampleData();
        iter.second->unload();
    }
    mBanks.clear();
    mEvents.clear();
}
SoundEvent AudioSystem::PlayEvent(const std::string& name)
{
    unsigned int retID = 0;
    // �̺�Ʈ�� �����ϴ��� Ȯ��
    auto iter = mEvents.find(name);
    if (iter != mEvents.end())
    {
        // �̺�Ʈ�� �ν��Ͻ��� �����Ѵ�.
        FMOD::Studio::EventInstance* event = nullptr;
        iter->second->createInstance(&event);
        if (event)
        {
            // �̺�Ʈ �ν��Ͻ��� �����Ѵ�.
            event->start();
            // �� ���̵� ��� �ʿ� �߰��Ѵ�.
            sNextID++;
            retID = sNextID;
            mEventInstances.emplace(retID, event);

            //// release�� �̺�Ʈ �ν��Ͻ��� ������ �� �̺�Ʈ �Ҹ��� ������ �����Ѵ�.
            //// �ݺ����� �ʴ� �̺�Ʈ�� �ڵ����� �����Ѵ�.
            //event->release();
        }
    }
    return SoundEvent(this, retID);
}
void AudioSystem::Update(float deltaTime)
{
    // ������ �̺�Ʈ�� ã�´�
    std::vector<unsigned int> done;
    for (auto& iter : mEventInstances)
    {
        FMOD::Studio::EventInstance* e = iter.second;
        // �� �̺�Ʈ�� ���°��� ��´�.
        FMOD_STUDIO_PLAYBACK_STATE state;
        e->getPlaybackState(&state);
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            // �̺�Ʈ�� �����ϰ� id�� done �ʿ� �߰��Ѵ�.
            e->release();
            done.emplace_back(iter.first);
        }
    }

    // �Ϸ�� �̺�Ʈ �ν��Ͻ��� �ʿ��� ����
    for (auto id : done)
    {
        mEventInstances.erase(id);
    }
    // FMOD ������Ʈ
    mSystem->update();
}

namespace
{
    FMOD_VECTOR VecToFMOD(const Vector3& in)
    {
        // ���� ��ǥ (+x ����, +y ������, +z ����)��
        // FMOD (+z ����, +x ������, +y ����) ��ǥ�� ��ȯ
        FMOD_VECTOR v;
        v.x = in.y;
        v.y = in.z;
        v.z = in.x;
        return v;
    }
}

void AudioSystem::SetListener(const Matrix4& viewMatrix, const Vector3& velocity)
{
    // ī�޶��� ��ġ�� �� ����� ����Ŀ��� ���� �� �ִ�.
    Matrix4 invView = viewMatrix;
    invView.Invert();
    FMOD_3D_ATTRIBUTES listener;
    // �������� ��ġ�� ���� ����, ���� ���͸� ����
    listener.position = VecToFMOD(invView.GetTranslation());
    // �� ����� ����Ŀ��� �� ��° ���� ���� ����
    listener.forward = VecToFMOD(invView.GetZAxis());
    // �� ����� ����Ŀ��� �� ��° ���� ���� ����
    listener.up = VecToFMOD(invView.GetYAxis());
    // �ӵ��� 0���� ���� (���÷� ȿ���� ����� �� ����)
    listener.velocity = VecToFMOD(velocity);
    // FMOD�� ������ (0 = �����ʴ� �ϳ�)
    mSystem->setListenerAttributes(0, &listener);
}
float AudioSystem::GetBusVolume(const std::string& name) const
{
    float retVal = 0.f;
    const auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->getVolume(&retVal);
    }
    return retVal;
}
bool AudioSystem::GetBusPaused(const std::string & name) const
{
    bool retVal = false;
    const auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->getPaused(&retVal);
    }
    return retVal;
}

void AudioSystem::SetBusVolume(const std::string& name, float volume)
{
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->setVolume(volume);
    }
}

void AudioSystem::SetBusPaused(const std::string & name, bool pause)
{
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->setPaused(pause);
    }
}

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id)
{
    FMOD::Studio::EventInstance* event = nullptr;
    auto iter = mEventInstances.find(id);
    if (iter != mEventInstances.end())
    {
        event = iter->second;
    }
    return event;
}