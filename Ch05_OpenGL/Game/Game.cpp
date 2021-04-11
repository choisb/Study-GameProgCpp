#include "Game.h"
#include "SDL/SDL_image.h"
#include <GL/glew.h>
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "BGSpriteComponent.h"
#include "Asteroid.h"
#include "VertexArray.h"
#include "Shader.h"

Game::Game()
    : mWindow(nullptr)
    , mRenderer(nullptr)
    , mIsRunning(true)
    , mUpdatingActors(false)
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
    // OpenGL ������ �Ӽ� ���� (�ݵ�� ������ ���� ���� ����� ��)
    // �Լ� ȣ���� �����ϸ� 0�� ��ȯ�ϰ� �����ϸ� ������ ��ȯ�Ѵ�.
    // �ھ� OpenGL �������� ���
    // �ھ�(����ũž��) �ܿ��� ȣȯ��, ES(����Ͽ�) ���������� �ִ�.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,    // profile �Ӽ��� �����Ѵ�.
                        SDL_GL_CONTEXT_PROFILE_CORE);   // core profile�� ����Ѵ�.
      
    // 3.3 �������� ����
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);   // ������ ������ 3���� ����
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);   // ���̳� ������ 3���� ����

    // RGBA ä�θ��� 8��Ʈ ũ���� ���� ���� ��û
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);   
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); 
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);  
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); 

    // ���� ���۸� Ȱ��ȭ
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);   
    // OpenGL�� �ϵ���� ������ ����ϵ��� ���� (OpenGL �������� GPU ���� ����ǵ��� ��)   
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 5)", // ������ ����
        100,    // �������� ���� ��� x��ǥ
        100,    // �������� ���� ��� y��ǥ
        1024,   // �������� �ʺ�
        768,    // �������� ����
        SDL_WINDOW_OPENGL       // �÷��� (0�� ��� �÷��׵� �������� ������ �ǹ�)
    );

    mContext = SDL_GL_CreateContext(mWindow);

    // GLEW �ʱ�ȭ ( GL/glew.h ��� include �ؾ���)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }
    // �Ϻ� �÷������� GLEW�� ���� �ڵ带 ��������.
    // �׷��Ƿ� �������� ��������
    glGetError();

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,    // �������� ���� ������ ������
        -1,         // ����� �׷���ī�� ����. -1�� ��� SDL�� �˾Ƽ� ���� 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC    // �÷��� ��.����ȭ ������ ��� ���ο� ���� ����ȭ�� Ȱ��ȭ ���� ����.
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

    // ���̴����� �ε��Ѵ�.
    if (!LoadShader())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }    // ��������Ʈ�� �׸��� ���� �簢�� ����
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

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
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
    // ������ ȸ������ ����
    glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    // ���� ���� �ʱ�ȭ
    glClear(GL_COLOR_BUFFER_BIT);

    // Sprite::Draw() �Լ����� glDrawElements()�� ����ȴ�.
    // �� glDrawElements() �Լ��� ����ϱ� ���ؼ��� 
    // "�������Ӹ���" Ȱ��ȭ�� ���ؽ� �迭 ��ü��, ���̴��� �ݵ�� �־�� �Ѵ�.
    // �׷��� Draw()�Լ��� ȣ���ϱ� ���� ������ Ȱ��ȭ �Ѵ�.
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();
    for (auto sprite : mSprites)
    {
        sprite->Draw(mSpriteShader);
    }

    // ���۸� �����ؼ� ����� ����Ѵ�.
    SDL_GL_SwapWindow(mWindow);
}
void Game::CreateSpriteVerts()
{

    float vertices[] = {
        -0.5f,  0.5f, 0.f,  // top left
         0.5f,  0.5f, 0.f,  // top right
         0.5f, -0.5f, 0.f,  // bottom right
        -0.5f, -0.5f, 0.f   // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    // ��������Ʈ�� �׸��� ���� 4���� ��������Ʈ VertexArray ��������
    // ������ ��� sprite���� �� ��������� ����Ѵ�.
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
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
    if (!mSpriteShader->Load("Shaders/Basic.vert", "Shaders/basic.frag"))
    {
        return false;
    }
    mSpriteShader->SetActive();
    return true;
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
    // ��ȯ�� texture�� �ּҰ��� ���� ����
    SDL_Texture* tex = nullptr;

    // unordered_map �����̳ʿ� ����Ǿ� �ִ� mTextures���� fileName���� �˻�
    auto iter = mTextures.find(fileName);
    // fileName�� �ش��ϴ� texture�� �����Ѵٸ�
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        // ���Ϸκ��� �ε�
        SDL_Surface* surf = IMG_Load(fileName.c_str());
        if (!surf)
        {
            SDL_Log("Failed to load texture file %s", fileName.c_str());
            return nullptr;
        }

        // �ؽ�ó ����
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
    delete mSpriteVerts;
    // OpenGL ���ؽ�Ʈ ����
    mSpriteShader->Unload();
    delete mSpriteShader;
    SDL_GL_DeleteContext(mContext);
    // mWindow ��ü ����.
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
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

void Game::AddSprite(SpriteComponent* sprite)
{
    // ���ĵ� ���Ϳ��� �����ؾ� �� ��ġ�� ã�´�.
    // (�ڽź��� �׸��� �������� ū ���� ���)
    int myDrawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); ++iter)
    {
        if (myDrawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }
    // �ݺ��� ��ġ �տ� ��Ҹ� �����Ѵ�.
    mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    mSprites.erase(iter);

    // mDrawOrder ������ ���ĵǾ� �ֱ� ������
    // swap(), pop()�� ���ؼ� ���� �� �� ����.

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