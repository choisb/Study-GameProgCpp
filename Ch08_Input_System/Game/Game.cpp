#include "Game.h"
#include "SDL/SDL_image.h"
#include <GL/glew.h>
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"
#include "VertexArray.h"
#include "Shader.h"
#include "InputSystem.h"

Game::Game()
    : mWindow(nullptr)
    , mRenderer(nullptr)
    , mIsRunning(true)
    , mUpdatingActors(false)
{
}

bool Game::Initialize()
{
    int sdResult = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER);    // 성공시 0을 반환
    if (sdResult != 0)
    {
        // SDL_Log는 C의 Printf와 같은 문법 사용
        // SDL_GetError()는 C 스타일의 문자열로 에러 메시지를 반환.
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    // OpenGL 윈도우 속성 설정 (반드시 윈도우 생성 전에 사용할 것)
    // 함수 호출이 성공하면 0을 반환하고 실패하면 음수를 반환한다.
    // 코어 OpenGL 프로파일 사용
    // 코어(데스크탑용) 외에도 호환성, ES(모바일용) 프로파일이 있다.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,    // profile 속성을 지정한다.
                        SDL_GL_CONTEXT_PROFILE_CORE);   // core profile을 사용한다.
      
    // 3.3 버전으로 지정
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);   // 메이저 버전을 3으로 설정
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);   // 마이너 버전을 3으로 설정

    // RGBA 채널마다 8비트 크기인 색상 버퍼 요청
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);   
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); 
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);  
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); 

    // 더블 버퍼링 활성화
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);   
    // OpenGL이 하드웨어 가속을 사용하도록 강제 (OpenGL 렌더링이 GPU 에서 수행되도록 함)   
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 5)", // 윈도우 제목
        100,    // 윈도우의 좌측 상단 x좌표
        100,    // 윈도우의 좌측 상단 y좌표
        1024,   // 윈도우의 너비
        768,    // 윈도우의 높이
        SDL_WINDOW_OPENGL       // 플래그 (0은 어떠한 플래그도 설정되지 않음을 의미)
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    // input system 초기화
    mInputSystem = new InputSystem();
    if(!mInputSystem->Initialize())
    {
        SDL_Log("Failed to initialize input system");
        mInputSystem->Shutdown();
        delete mInputSystem;
        mInputSystem = nullptr;
    }

    mContext = SDL_GL_CreateContext(mWindow);

    // GLEW 초기화 ( GL/glew.h 헤더 include 해야함)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }
    // 일부 플랫폼에서 GLEW은 에러 코드를 내보낸다.
    // 그러므로 에러값을 제거하자
    glGetError();

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

    // 셰이더들을 로드한다.
    if (!LoadShader())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }    // 스프라이트를 그리기 위한 사각형 생성
    CreateSpriteVerts();

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
    mInputSystem->PrepareForUpdate();

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
        case SDL_MOUSEWHEEL:
            mInputSystem->ProcessEvent(event);
            break;
        default:
            break;
        }
    }

    mInputSystem->Update();
    const InputState& state = mInputSystem->GetState();

    if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == EReleased)
    {
        mIsRunning = false;
    }

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
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
    // 색상을 회색으로 설정
    glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    // 색상 버퍼 초기화
    glClear(GL_COLOR_BUFFER_BIT);

    // Sprite::Draw() 함수에서 glDrawElements()가 실행된다.
    // 이 glDrawElements() 함수를 사용하기 위해서는 
    // "매프레임마다" 활성화된 버텍스 배열 개체와, 셰이더가 반드시 있어야 한다.
    // 그래서 Draw()함수를 호출하기 전에 각각을 활성화 한다.
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    // 알파블렌딩을 활성화
    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,           // srcFactor = srcAlpha
        GL_ONE_MINUS_SRC_ALPHA  // dstFactor = 1 - srcAlpha
    );

    for (auto sprite : mSprites)
    {
        sprite->Draw(mSpriteShader);
    }

    // 버퍼를 스왑해서 장면을 출력한다.
    SDL_GL_SwapWindow(mWindow);
}
void Game::CreateSpriteVerts()
{
    float vertices[] = {
        //  x,     y,    z,    u,    v  // 버텍스 위치(x,y,z) 텍스처 맵핑(u,v)
        -0.5f,  0.5f,  0.f,  0.f,  0.f, // top left
         0.5f,  0.5f,  0.f,  1.f,  0.f, // top right
         0.5f, -0.5f,  0.f,  1.f,  1.f, // bottom right
        -0.5f, -0.5f,  0.f,  0.f,  1.f  // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    // 스프라이트를 그리기 위한 4각형 스프라이트 VertexArray 생성과정
    // 앞으로 모든 sprite들은 이 멤버변수를 사용한다.
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	//mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetRotation(Math::PiOver2);

	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}

}
void Game::UnloadData()
{
    // delete actors
    while (!mActors.empty())
    {
        delete mActors.back();
    }
}
bool Game::LoadShader()
{
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
    {
        return false;
    }
    mSpriteShader->SetActive();
    // 화면 너비가 1024x768인 view proj 변환행렬
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    return true;
}

Texture* Game::GetTexture(const std::string& fileName)
{
    // 반환할 texture의 주소값을 담을 변수
    Texture* tex = nullptr;

    // unordered_map 컨테이너에 저장되어 있는 mTextures에서 fileName으로 검색
    auto iter = mTextures.find(fileName);
    // fileName에 해당하는 texture가 존재한다면
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();

        // 파일로부터 로딩
        if (tex->Load(fileName))
        {
            mTextures.emplace(fileName.c_str(), tex);
        }
        else
        {
            delete tex;
            tex = nullptr;
        }
    }
    return tex;
}

void Game::Shutdown()
{
    delete mSpriteVerts;
    // OpenGL 콘텍스트 제거
    mSpriteShader->Unload();

    // input system 제거
    mInputSystem->Shutdown();
    delete mInputSystem;

    delete mSpriteShader;
    SDL_GL_DeleteContext(mContext);
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