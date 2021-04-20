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




