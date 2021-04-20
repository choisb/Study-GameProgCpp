#include "Game.h"
#include "Renderer.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"

Game::Game()
    :mRenderer(nullptr)
    ,mIsRunning(true)
    ,mUpdatingActors(false)
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
}

void Game::GenerateOutput()
{
    mRenderer->Draw();
}


void Game::LoadData()
{

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




