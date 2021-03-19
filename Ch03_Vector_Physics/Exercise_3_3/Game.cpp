#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"


Game::Game()
    : mWindow(nullptr)
    , mRenderer(nullptr)
    , mIsRunning(true)
    , mUpdatingActors(false)
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

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 3)", // 윈도우 제목
        100,    // 윈도우의 좌측 상단 x좌표
        100,    // 윈도우의 좌측 상단 y좌표
        1024,   // 윈도우의 너비
        768,    // 윈도우의 높이
        0       // 플래그 (0은 어떠한 플래그도 설정되지 않음을 의미)
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,    // 렌더링을 위해 생성한 윈도우
        -1,         // 사용할 그래픽카드 선택. -1인 경우 SDL이 알아서 선택 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC    // 플래그 값.가속화 렌더러 사용 여부와 수직 동기화의 활성화 여부 선택.
    );
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }


    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
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

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
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
    //mRenderer를 (0,0,255,255) 파랑색으로 지정
    SDL_SetRenderDrawColor(
        mRenderer,
        210,    // R
        210,    // G
        210,    // B
        255     // A
    );

    // 현재 그리기 색상으로 후면 버퍼 지우기
    SDL_RenderClear(mRenderer);

    for (auto sprite : mSprites)
    {
        sprite->Draw(mRenderer);
    }

    /////////////////////////////////

    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(mRenderer);
}
void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(10.0f, 384.0f));

}
void Game::UnloadData()
{
    // delete actors
    while (!mActors.empty())
    {
        delete mActors.back();
    }
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
    // 반환할 texture의 주소값을 담을 변수
    SDL_Texture* tex = nullptr;

    // unordered_map 컨테이너에 저장되어 있는 mTextures에서 fileName으로 검색
    auto iter = mTextures.find(fileName);
    // fileName에 해당하는 texture가 존재한다면
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        // 파일로부터 로딩
        SDL_Surface* surf = IMG_Load(fileName.c_str());
        if (!surf)
        {
            SDL_Log("Failed to load texture file %s", fileName.c_str());
            return nullptr;
        }

        // 텍스처 생성
        tex = SDL_CreateTextureFromSurface(mRenderer, surf);
        SDL_FreeSurface(surf);
        if (!tex)
        {
            SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
            return nullptr;
        }

        mTextures.emplace(fileName.c_str(), tex);
    }
    return tex;
}

void Game::Shutdown()
{

    // mWindow 객체 해제.
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
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

void Game::AddSprite(SpriteComponent* sprite)
{
    // 정렬된 벡터에서 사입해야 할 위치를 찾는다.
    // (자신보다 그리기 순서값이 큰 최초 요소)
    int myDrawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); ++iter)
    {
        if (myDrawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }
    // 반복자 위치 앞에 요소를 삽입한다.
    mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    mSprites.erase(iter);

    // mDrawOrder 순으로 정렬되어 있기 때문에
    // swap(), pop()을 통해서 삭제 할 수 없다.

    //if (iter != mSprites.end())
    //{
    //    std::iter_swap(iter, mSprites.end() - 1);
    //    mSprites.pop_back();
    //}
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(),
		mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}