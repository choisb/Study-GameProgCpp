#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include <GL/glew.h>

Renderer::Renderer(Game* game)
    :mGame(game)
    ,mSpriteShader(nullptr)
    ,mMeshShader(nullptr)
{

}

Renderer::~Renderer()
{

}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;

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
        "Game Programming in C++ (Chapter 6)", // 윈도우 제목
        100,                            // 윈도우의 좌측 상단 x좌표
        100,                            // 윈도우의 좌측 상단 y좌표
        static_cast<int>(mScreenWidth), // 윈도우의 너비
        static_cast<int>(mScreenHeight),// 윈도우의 높이
        SDL_WINDOW_OPENGL               // 플래그 (0은 어떠한 플래그도 설정되지 않음을 의미)
    );
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
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



    // 셰이더들을 로드한다.
    if (!LoadShaders())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }    // 스프라이트를 그리기 위한 사각형 생성
    CreateSpriteVerts();

    return true;
}

void Renderer::Shutdown()
{
    delete mSpriteVerts;
    // OpenGL 콘텍스트 제거
    mSpriteShader->Unload();
    delete mSpriteShader;
    SDL_GL_DeleteContext(mContext);
    // mWindow 객체 해제.
    SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    // Destroy meshes
    for (auto i : mMeshes)
    {
        i.second->Unload();
        delete i.second;
    }
    mMeshes.clear();
}

void Renderer::Draw()
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

void Renderer::AddSprite(SpriteComponent* sprite)
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

void Renderer::RemoveSprite(SpriteComponent* sprite)
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

Texture* Renderer::GetTexture(const std::string& fileName)
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

Mesh* Renderer::GetMesh(const std::string & fileName)
{
    Mesh* m = nullptr;
    auto iter = mMeshes.find(fileName);
    if (iter != mMeshes.end())
    {
        m = iter->second;
    }
    else
    {
        m = new Mesh();
        if (m->Load(fileName, this))
        {
            mMeshes.emplace(fileName, m);
        }
        else
        {
            delete m;
            m = nullptr;
        }
    }
    return m;
}
bool Renderer::LoadShaders()
{
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
    {
        return false;
    }
    mSpriteShader->SetActive();
    // 화면 너비가 1024x768인 view proj 변환행렬
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    return true;
}

void Renderer::CreateSpriteVerts()
{
    float vertices[] = {
        //  x,     y,    z,    x,   y,   z,   u,   v  // 버텍스 위치(x,y,z) 법선 벡터(x,y,z) 텍스처 맵핑(u,v)
        -0.5f,  0.5f,  0.f,  0.f, 0.f, 0.f, 0.f, 0.f, // top left
         0.5f,  0.5f,  0.f,  0.f, 0.f, 0.f, 1.f, 0.f, // top right
         0.5f, -0.5f,  0.f,  0.f, 0.f, 0.f, 1.f, 1.f, // bottom right
        -0.5f, -0.5f,  0.f,  0.f, 0.f, 0.f, 0.f, 1.f  // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    // 스프라이트를 그리기 위한 4각형 스프라이트 VertexArray 생성과정
    // 앞으로 모든 sprite들은 이 멤버변수를 사용한다.
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}