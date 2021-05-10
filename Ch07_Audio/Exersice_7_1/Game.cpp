#include "Game.h"
#include "Renderer.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Texture.h"
#include "CameraActor.h"
#include "PlaneActor.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "SoundObject.h"
#include <algorithm>

Game::Game()
    :mRenderer(nullptr)
    ,mIsRunning(true)
    ,mUpdatingActors(false)
    ,mCameraActor(nullptr)
    ,mAudioSystem(nullptr)
{
}

bool Game::Initialize()
{
    int sdResult = SDL_Init(SDL_INIT_VIDEO);    // 성공시 0을 반환
    if (sdResult != 0)
    {
        // SDL_Log는 C의 Printf와 같은 문법 사용
        // SDL_GetError()는 C 스타일의 문자열로 에러 메시지를 반환.
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Renderer 생성 및 초기화
    mRenderer = new Renderer(this);
    if (!mRenderer->Initialize(1024.0f, 768.0f))
    {
        SDL_Log("Failed to initialize renderer");
        delete mRenderer;
        mRenderer = nullptr;
        return false;
    }

    // AudioSystem 생성
    mAudioSystem = new AudioSystem(this);
    if (!mAudioSystem->Initialize())
    {
        SDL_Log("Failed to initialize audio system");
        mAudioSystem->Shutdown();
        delete mAudioSystem;
        mAudioSystem = nullptr;
        return false;
    }

    LoadData();
    mTicksCount = SDL_GetTicks();
    return true;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;

    // 큐에 여전히 이벤트가 남아 있는 동안
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // 이벤트가 SDL_QUIT이면 루프를 종료한다.
        case SDL_QUIT:
            mIsRunning = false;
            break;

        case SDL_KEYDOWN:
            if (!event.key.repeat)
            {
                HandleKeyPress(event.key.keysym.sym);
            }
            break;
        default:
            break;
        }
    }

    // 키보드 상태 얻기
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    // 이스케이프 키를 눌렀다면 루프 종료
    if (keyState[SDL_SCANCODE_ESCAPE])
    {
        mIsRunning = false;
    }
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
}

void Game::HandleKeyPress(int key)
{
    switch (key)
    {
    case '-':
    {
        // master volume 감소
        float volume = mAudioSystem->GetBusVolume("bus:/");
        volume = Math::Max(0.0f, volume - 0.1f);
        mAudioSystem->SetBusVolume("bus:/", volume);
        break;
    }
    case '=':
    {
        // master volume 증가
        float volume = mAudioSystem->GetBusVolume("bus:/");
        volume = Math::Min(1.0f, volume + 0.1f);
        mAudioSystem->SetBusVolume("bus:/", volume);
        break;
    }
    case 'e':
        // explosion 재생
        mAudioSystem->PlayEvent("event:/Explosion2D");
        break;
    case 'm':
        // mMusicEvent의 puase 상태 토글
        mMusicEvent.SetPaused(!mMusicEvent.GetPaused());
        break;

    case 'r':
        // 스냅샷을 통해서 SFX 버스의 리버브 DSP를 활성화/비활성화
        if (!mReverbSnap.IsValid())
        {
            mReverbSnap = mAudioSystem->PlayEvent("snapshot:/WithReverb");
        }
        else
        {
            mReverbSnap.Stop();
        }
        break;
    case '1':
        // 기본 footstep surface
        mCameraActor->SetFootstepSurface(0.0f);
        break;
    case '2':
        mCameraActor->SetFootstepSurface(0.5f);
        break;
    default:
        break;
    }
}
void Game::UpdateGame()
{
    // 마지막 프레임 이후로 16ms가 경과할 때 까지 대기
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    // 델타 시간은 현재 프레임 틱값과 이전 프레임 틱값의 차다.
    // (차를 초 단위로 변환)
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    // 최대 델타 시간값으로 고정
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    // 다음 프레임을 위해 현재 시간값 저장.
    mTicksCount = SDL_GetTicks();

    // 모든 액터를 갱신
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    // 대기 중인 액터를 mActors로 이동
    for (auto pending : mPendingActors)
    {
        // 생성된 직후 올바른 세계 변환을 가지기 위해서 변환을 계산한다.
        pending->ComputeWorldTransform();
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // 죽은 액터를 임시 벡터에 추가
    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == Actor::EDead)
        {
            deadActors.emplace_back(actor);
        }
    }

    // 죽은 액터 제거(mActors에서 추려낸 액터들)
    for (auto actor : deadActors)
    {
        delete actor;
    }

    // Update audio system
    mAudioSystem->Update(deltaTime);
}

void Game::GenerateOutput()
{
    mRenderer->Draw();
}


void Game::LoadData()
{
    // Create actors
    Actor* a = new Actor(this);
    a->SetPosition(Vector3(200.0f, 75.0f, 0.0f));
    a->SetScale(100.0f);



    // Setup floor
    const float start = -1250.0f;
    const float size = 250.0f;
    for (int i = 0; i < 10; i++)
    {
	    for (int j = 0; j < 10; j++)
	    {
		    a = new PlaneActor(this);
		    a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
	    }
    }

    // Left/right walls
    Quaternion q = Quaternion(Vector3::UnitX, Math::PiOver2);
    for (int i = 0; i < 10; i++)
    {
	    a = new PlaneActor(this);
	    a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
	    a->SetRotation(q);
	
	    a = new PlaneActor(this);
	    a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
	    a->SetRotation(q);
    }

    q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
    // Forward/back walls
    for (int i = 0; i < 10; i++)
    {
	    a = new PlaneActor(this);
	    a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
	    a->SetRotation(q);

	    a = new PlaneActor(this);
	    a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
	    a->SetRotation(q);
    }
 
    // UI elements
    a = new Actor(this);
    a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
    SpriteComponent* sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("../Assets/HealthBar.png"));

    a = new Actor(this);
    a->SetPosition(Vector3(375.0f, -275.0f, 0.0f));
    a->SetScale(0.75f);
    sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("../Assets/Radar.png"));

    // Camera actor
    mCameraActor = new CameraActor(this);


    // 게임에 존재하는 유일한 방향광을 설정한다. 
    // 실제 게임에서는 다양항 방향광이 존재할 수 있지만 현재 버전에서는 하나의 방향광만 지원한다.
    mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
    dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

    // audio components 재생을 담당할 구 생성
    SoundObject* so;
    so = new SoundObject(this);

    mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

}

void Game::UnloadData()
{
    // delete actors
    while (!mActors.empty())
    {
        delete mActors.back();
    }
    if (mRenderer)
    {
        mRenderer->UnloadData();
    }
}


void Game::Shutdown()
{
    UnloadData();
    if (mRenderer)
    {
        mRenderer->Shutdown();
    }
    if (mAudioSystem)
    {
        mAudioSystem->Shutdown();
    }
    SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
    // 액터를 갱신 중이라면 대기 벡터에 액터를 추가
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    // Actor를 저장한 2개의 vector actor를 탐색 후 존재한다면 삭제
    // mActors vector에 존재할 경우 삭제
    auto iter = find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        std::iter_swap(iter, mActors.end()-1);
        mActors.pop_back();
    }
    // mPendingActors vector에 존재할 경우 삭제
    iter = find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        std::iter_swap(iter, mPendingActors.end()-1);
        mPendingActors.pop_back();
    }
}




