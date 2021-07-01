#include "Game.h"
#include "Renderer.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Texture.h"
#include "PlaneActor.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "FollowActor.h"
#include "PhysWorld.h"
#include "TargetActor.h"
#include "Font.h"
#include "UIScreen.h"
#include "HUD.h"
#include "PauseMenu.h"
#include "Skeleton.h"
#include "Animation.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>

Game::Game()
    :mRenderer(nullptr)
    ,mGameState(EGameplay)
    ,mUpdatingActors(false)
    ,mAudioSystem(nullptr)
    ,mPhysWorld(nullptr)
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
    // physics world 생성
    mPhysWorld = new PhysWorld(this);

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    LoadData();
    mTicksCount = SDL_GetTicks();
    return true;
}

void Game::RunLoop()
{
    while (mGameState != EQuit)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::AddPlane(PlaneActor* plane)
{
    mPlanes.emplace_back(plane);
}

void Game::RemovePlane(PlaneActor* plane)
{
    auto iter = std::find(mPlanes.begin(), mPlanes.end(), plane);
    mPlanes.erase(iter);
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
            mGameState = EQuit;
            break;

        case SDL_KEYDOWN:
            if (!event.key.repeat)
            {
                if (mGameState == EGameplay)
                {
                    HandleKeyPress(event.key.keysym.sym);
                }
                else if (!mUIStack.empty())
                {
                    mUIStack.back()->
                        HandleKeyPress(event.key.keysym.sym);
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (mGameState == EGameplay)
            {
                HandleKeyPress(event.button.button);
            }
            else if (!mUIStack.empty())
            {
                mUIStack.back()->HandleKeyPress(event.button.button);
            }
            break;
        default:
            break;
        }
    }

    // 키보드 상태 얻기
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (mGameState == EGameplay)
    {
        for (auto actor : mActors)
        {
            if (actor->GetState() == Actor::EActive)
            {
                actor->ProcessInput(keyState);
            }
        }
    }
    else if (!mUIStack.empty())
    {
        mUIStack.back()->ProcessInput(keyState);
    }
}

void Game::HandleKeyPress(int key)
{
    switch (key)
    {
    case SDLK_ESCAPE:
        new PauseMenu(this);
        break;
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
    case '1':
    {
        // Load English text
        LoadText("../Assets/English.gptext");
        break;
    }
    case '2':
    {
        // Load Russian text
        LoadText("../Assets/Russian.gptext");
        break;
    }
    case '3':
    {
        // Load Russian text
        LoadText("../Assets/Korean.gptext");
        break;
    }
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

    if (mGameState == EGameplay)
    {
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
    }
    // Update audio system
    mAudioSystem->Update(deltaTime);

    // Update UI screens
    for (auto ui : mUIStack)
    {
        if (ui->GetState() == UIScreen::EActive)
        {
            ui->Update(deltaTime);
        }
    }
    // Delete any UIScreens that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end())
    {
        if ((*iter)->GetState() == UIScreen::EClosing)
        {
            delete *iter;
            iter = mUIStack.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void Game::GenerateOutput()
{
    mRenderer->Draw();
}


void Game::LoadData()
{
    // Load English text
    LoadText("../Assets/English.gptext");

    // Create actors
    Actor* a = nullptr;
    Quaternion q;
    //MeshComponent* mc = nullptr;

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
    q = Quaternion(Vector3::UnitX, Math::PiOver2);
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

    // Setup lights
    mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
    dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

    // UI elements
    mHUD = new HUD(this);

    // Start music
    mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

    // Enable relative mouse mode for camera look
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // Make an initial call to get relative to clear out
    SDL_GetRelativeMouseState(nullptr, nullptr);

    // Create Player
    mFollowActor = new FollowActor(this);

    // Create target actors
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, 0.0f, 100.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, 0.0f, 400.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, -500.0f, 200.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, 500.0f, 200.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(0.0f, -1450.0f, 200.0f));
    a->SetRotation(Quaternion(Vector3::UnitZ, Math::PiOver2));
    a = new TargetActor(this);
    a->SetPosition(Vector3(0.0f, 1450.0f, 200.0f));
    a->SetRotation(Quaternion(Vector3::UnitZ, -Math::PiOver2));
}

void Game::UnloadData()
{
    // delete actors
    while (!mActors.empty())
    {
        delete mActors.back();
    }
    while (!mUIStack.empty())
    {
        delete mUIStack.back();
        mUIStack.pop_back();
    }
    auto iter = mFonts.begin();
    while(iter != mFonts.end())
    {
        iter->second->Unload();
        delete iter->second;
        ++iter;
    }
    if (mRenderer)
    {
        mRenderer->UnloadData();
    }
}


void Game::Shutdown()
{
    UnloadData();
    delete mPhysWorld;

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

void Game::PushUI(UIScreen* screen)
{
    mUIStack.emplace_back(screen);
}

Font* Game::GetFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }
    else
    {
        Font* font = new Font(this);
        if (font->Load(fileName))
        {
            mFonts.emplace(fileName, font);
        }
        else
        {
            font->Unload();
            delete font;
            font = nullptr;
        }
        return font;
    }
}
void Game::LoadText(const std::string& fileName)
{
    // 기존에 저장되어있던 내용을 정리한다.
    mText.clear();
    // 파일을 연다
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Text file %s not found", fileName.c_str());
        return;
    }
    // 파일의 내용을 모두 string stream로 읽어온다.
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string contents = fileStream.str();
    // 열린 파일은 rapidJSON으로 읽어온다.
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);
    if (!doc.IsObject())
    {
        SDL_Log("Text file %s is not valid JSON", fileName.c_str());
        return;
    }
    // 텍스트 맵을 파싱한다.
    const rapidjson::Value& actions = doc["TextMap"];
    for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
        itr != actions.MemberEnd(); ++itr)
    {
        if (itr->name.IsString() && itr->value.IsString())
        {
            mText.emplace(itr->name.GetString(), itr->value.GetString());
        }
    }
}

const std::string& Game::GetText(const std::string& key)
{
    static std::string errorMsg("**KEY NOT FOUND**");
    // Find this text in the map, if it exists
    auto iter = mText.find(key);
    if (iter != mText.end())
    {
        return iter->second;
    }
    else
    {
        return errorMsg;
    }
}

Skeleton* Game::GetSkeleton(const std::string& fileName)
{
    auto iter = mSkeletons.find(fileName);
    if (iter != mSkeletons.end())
    {
        return iter->second;
    }
    else
    {
        Skeleton* sk = new Skeleton();
        if (sk->Load(fileName))
        {
            mSkeletons.emplace(fileName, sk);
        }
        else
        {
            delete sk;
            sk = nullptr;
        }
        return sk;
    }
}
Animation* Game::GetAnimation(const std::string& fileName)
{
    auto iter = mAnims.find(fileName);
    if (iter != mAnims.end())
    {
        return iter->second;
    }
    else
    {
        Animation* anim = new Animation();
        if (anim->Load(fileName))
        {
            mAnims.emplace(fileName, anim);
        }
        else
        {
            delete anim;
            anim = nullptr;
        }
        return anim;
    }
}