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
    // 에러에 대한 로깅을 설정하기 위해 Debug_Initialize 함수 호출
    FMOD::Debug_Initialize(
        FMOD_DEBUG_LEVEL_ERROR, // 로깅 출력 수준: 에러일 경우에만 로그를 남김
        FMOD_DEBUG_MODE_TTY     // 로깅 출력 위치: stdout으로 로그 출력
    );

    // FMOD 스튜디오 시스템의 인스턴스 생성
    FMOD_RESULT result;
    result = FMOD::Studio::System::create(&mSystem);
    // FMOD는 일반적으로 함수의 결과값을 반환하여 정상적으로 시행 되었는지 알려준다.
    if (result != FMOD_OK)  
    {
        SDL_Log("Failed to create FMOD system %s", FMOD_ErrorString(result));
        return false;
    }
   
    // FMOD 시스템의 initialize 함수 호출
    result = mSystem->initialize(
        512,                        // 동시에 출력할 수 있는 사운드의 최대 갯수
        FMOD_STUDIO_INIT_NORMAL,    // 기본 설정 사용
        FMOD_INIT_NORMAL,           // 기본 설정
        nullptr                     // 대부분 nullptr
    );
    if (result != FMOD_OK)
    {
        SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
        return false;
    }

    // 저수준의 시스템 포인터를 얻어와서 mLowLevelSyste에 저장한다.
    result = mSystem->getLowLevelSystem(&mLowLevelSystem);
    if (result != FMOD_OK)
    {
        SDL_Log("Failed to create FMOD system %s", FMOD_ErrorString(result));
        return false;
    }

    mLowLevelSystem->set3DSettings(
        1.f,    // 도플러 스케일, 1 = 정상, 1보다 더 크면 과장된 소리를 낸다
        50.f,   // 게임 단위의 크기 = 1미터 (7장 프로젝트는 50이 적당함)
        1.f     // (도플러와 관계없음, 1로 남겨둔다)
    );


    // 모든 FMOD 스튜디오 프로젝트는 아래의 두 뱅크를 기본 뱅크로 반드시 가지고 있다.
    // 아래의 두 뱅크를 로드하지 않는다면 다른 뱅크나 이벤트에 액세스 할 수 없다.
    // 따라서 초기화 단계에서 아래의 두 뱅크를 로드해주자.
    // 그리고 Master Bank.strings.bank를 먼저 로드하자.
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
    // 두 번 로딩되지 않게 한다.
    if (mBanks.find(name) != mBanks.end())
    {
        return;
    }

    // 뱅크 로드
    FMOD::Studio::Bank* bank = nullptr;
    FMOD_RESULT result = mSystem->loadBankFile(
        name.c_str(),                   // 뱅크의 파일 이름
        FMOD_STUDIO_LOAD_BANK_NORMAL,   // 일반적인 방식으로 로딩
        &bank                           // 뱅크 포인터 저장
    );
    if (result != FMOD_OK)
    {
        SDL_Log("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    }
    const int maxPathLength = 512;
    if (result == FMOD_OK)
    {
        // 뱅크를 맵에 추가
        mBanks.emplace(name, bank);
        // 스트리밍 형식이 아닌 모든 샘플 데이터를 로드
        bank->loadSampleData();
        // 뱅크의 이벤트 수를 얻는다
        int numEvents = 0;
        bank->getEventCount(&numEvents);
        if (numEvents > 0) 
        {
            // 뱅크에서 이벤트 디스크립션 리스트를 얻는다
            // (뱅크의 이벤트 수와 같은 길이의 events vector를 선언)
            std::vector<FMOD::Studio::EventDescription*> events(numEvents);
            bank->getEventList(events.data(), numEvents, &numEvents);
            char eventName[maxPathLength];
            for (auto e : events)
            {
                // event:/Explosion2D 같은 이벤트의 경로를 얻는다.
                e->getPath(eventName, maxPathLength, nullptr);
                // 이벤트를 맵에 추가한다.
                mEvents.emplace(eventName, e);
            }
        }
    }

    // 뱅크에 있는 버스들을 로드
    int numBuses = 0;
    bank->getBusCount(&numBuses);
    if (numBuses > 0)
    {
        // 뱅크에 있는 버스의 리스트를 가져온다
        std::vector<FMOD::Studio::Bus*> buses(numBuses);
        bank->getBusList(buses.data(), numBuses, &numBuses);
        char busName[512];
        for (auto bus : buses)
        {
            // 버스의 경로를 가져온다. (예를 들어 bus:/SFX 와 같은)
            bus->getPath(busName, 512, nullptr);
            // 버스를 맵에 추가한다.
            mBuses.emplace(busName, bus);
        }
    }
}

void AudioSystem::UnloadBank(const std::string& name)
{
    // 로드 되지 않았다면 무시
    auto iter = mBanks.find(name);
    if (iter != mBanks.end())
    {
        return;
    }

    // 먼저 이 뱅크의 모든 이벤트를 삭제한다.
    FMOD::Studio::Bank* bank = iter->second;
    int numEvents = 0;
    bank->getEventCount(&numEvents);
    if (numEvents > 0)
    {
        // 이 뱅크에서 event descriptions들을 가져온다.
        std::vector<FMOD::Studio::EventDescription*> events(numEvents);
        // 이벤트의 리스트를 가져온다.
        bank->getEventList(events.data(), numEvents, &numEvents);
        char eventName[512];
        for (auto e : events)
        {
            // 이 이벤트의 경로를 가져온다
            e->getPath(eventName, 512, nullptr);
            // 이 이벤트를 삭제한다.
            auto eventi = mEvents.find(eventName);
            if (eventi != mEvents.end())
            {
                mEvents.erase(eventi);
            }
        }
    }

    // 이 뱅크의 모든 버스를 삭제한다.
    int numBuses = 0;
    bank->getBusCount(&numBuses);
    if (numBuses > 0)
    {
        // 이 뱅크의 모든 버스 리스트를 가져온다.
        std::vector<FMOD::Studio::Bus*> buses(numBuses);
        bank->getBusList(buses.data(), numBuses, &numBuses);
        char busName[512];
        for (auto bus : buses)
        {
            // 버스의 경로를 가져온다. (예를 들어 bus:/SFX 와 같은)
            bus->getPath(busName, 512, nullptr);
            // 이 버스를 삭제한다.
            auto busi = mBuses.find(busName);
            if (busi != mBuses.end())
            {
                mBuses.erase(busi);
            }
        }
    }

    // 샘플 데이터와 뱅크를 unload
    bank->unloadSampleData();
    bank->unload();
    // banks map에서 bank 삭제
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
    // 이벤트가 존재하는지 확인
    auto iter = mEvents.find(name);
    if (iter != mEvents.end())
    {
        // 이벤트의 인스턴스를 생성한다.
        FMOD::Studio::EventInstance* event = nullptr;
        iter->second->createInstance(&event);
        if (event)
        {
            // 이벤트 인스턴스를 시작한다.
            event->start();
            // 새 아이디를 얻어 맵에 추가한다.
            sNextID++;
            retID = sNextID;
            mEventInstances.emplace(retID, event);

            //// release는 이벤트 인스턴스가 정지할 때 이벤트 소멸자 실행을 예약한다.
            //// 반복하지 않는 이벤트는 자동으로 정지한다.
            //event->release();
        }
    }
    return SoundEvent(this, retID);
}
void AudioSystem::Update(float deltaTime)
{
    // 정지된 이벤트를 찾는다
    std::vector<unsigned int> done;
    for (auto& iter : mEventInstances)
    {
        FMOD::Studio::EventInstance* e = iter.second;
        // 이 이벤트의 상태값을 얻는다.
        FMOD_STUDIO_PLAYBACK_STATE state;
        e->getPlaybackState(&state);
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            // 이벤트를 해제하고 id를 done 맵에 추가한다.
            e->release();
            done.emplace_back(iter.first);
        }
    }

    // 완료된 이벤트 인스턴스를 맵에서 제거
    for (auto id : done)
    {
        mEventInstances.erase(id);
    }
    // FMOD 업데이트
    mSystem->update();
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

void AudioSystem::SetListener(const Matrix4& viewMatrix, const Vector3& velocity)
{
    // 카메라의 위치는 뷰 행렬의 역행렬에서 구할 수 있다.
    Matrix4 invView = viewMatrix;
    invView.Invert();
    FMOD_3D_ATTRIBUTES listener;
    // 리스너의 위치와 전방 벡터, 상향 벡터를 설정
    listener.position = VecToFMOD(invView.GetTranslation());
    // 뷰 행렬의 역행렬에서 세 번째 행은 전방 벡터
    listener.forward = VecToFMOD(invView.GetZAxis());
    // 뷰 행렬의 역행렬에서 두 번째 행은 상향 벡터
    listener.up = VecToFMOD(invView.GetYAxis());
    // 속도를 0으로 설정 (도플러 효과를 사용할 시 수정)
    listener.velocity = VecToFMOD(velocity);
    // FMOD로 보낸다 (0 = 리스너는 하나)
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