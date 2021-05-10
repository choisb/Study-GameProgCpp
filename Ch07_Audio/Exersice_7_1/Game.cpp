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
    int sdResult = SDL_Init(SDL_INIT_VIDEO);    // ������ 0�� ��ȯ
    if (sdResult != 0)
    {
        // SDL_Log�� C�� Printf�� ���� ���� ���
        // SDL_GetError()�� C ��Ÿ���� ���ڿ��� ���� �޽����� ��ȯ.
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Renderer ���� �� �ʱ�ȭ
    mRenderer = new Renderer(this);
    if (!mRenderer->Initialize(1024.0f, 768.0f))
    {
        SDL_Log("Failed to initialize renderer");
        delete mRenderer;
        mRenderer = nullptr;
        return false;
    }

    // AudioSystem ����
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

    // ť�� ������ �̺�Ʈ�� ���� �ִ� ����
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // �̺�Ʈ�� SDL_QUIT�̸� ������ �����Ѵ�.
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

    // Ű���� ���� ���
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    // �̽������� Ű�� �����ٸ� ���� ����
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
        // master volume ����
        float volume = mAudioSystem->GetBusVolume("bus:/");
        volume = Math::Max(0.0f, volume - 0.1f);
        mAudioSystem->SetBusVolume("bus:/", volume);
        break;
    }
    case '=':
    {
        // master volume ����
        float volume = mAudioSystem->GetBusVolume("bus:/");
        volume = Math::Min(1.0f, volume + 0.1f);
        mAudioSystem->SetBusVolume("bus:/", volume);
        break;
    }
    case 'e':
        // explosion ���
        mAudioSystem->PlayEvent("event:/Explosion2D");
        break;
    case 'm':
        // mMusicEvent�� puase ���� ���
        mMusicEvent.SetPaused(!mMusicEvent.GetPaused());
        break;

    case 'r':
        // �������� ���ؼ� SFX ������ ������ DSP�� Ȱ��ȭ/��Ȱ��ȭ
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
        // �⺻ footstep surface
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
    // ������ ������ ���ķ� 16ms�� ����� �� ���� ���
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    // ��Ÿ �ð��� ���� ������ ƽ���� ���� ������ ƽ���� ����.
    // (���� �� ������ ��ȯ)
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    // �ִ� ��Ÿ �ð������� ����
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    // ���� �������� ���� ���� �ð��� ����.
    mTicksCount = SDL_GetTicks();

    // ��� ���͸� ����
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    // ��� ���� ���͸� mActors�� �̵�
    for (auto pending : mPendingActors)
    {
        // ������ ���� �ùٸ� ���� ��ȯ�� ������ ���ؼ� ��ȯ�� ����Ѵ�.
        pending->ComputeWorldTransform();
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // ���� ���͸� �ӽ� ���Ϳ� �߰�
    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == Actor::EDead)
        {
            deadActors.emplace_back(actor);
        }
    }

    // ���� ���� ����(mActors���� �߷��� ���͵�)
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


    // ���ӿ� �����ϴ� ������ ���Ɽ�� �����Ѵ�. 
    // ���� ���ӿ����� �پ��� ���Ɽ�� ������ �� ������ ���� ���������� �ϳ��� ���Ɽ�� �����Ѵ�.
    mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
    dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

    // audio components ����� ����� �� ����
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
    // ���͸� ���� ���̶�� ��� ���Ϳ� ���͸� �߰�
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
    // Actor�� ������ 2���� vector actor�� Ž�� �� �����Ѵٸ� ����
    // mActors vector�� ������ ��� ����
    auto iter = find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        std::iter_swap(iter, mActors.end()-1);
        mActors.pop_back();
    }
    // mPendingActors vector�� ������ ��� ����
    iter = find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        std::iter_swap(iter, mPendingActors.end()-1);
        mPendingActors.pop_back();
    }
}




