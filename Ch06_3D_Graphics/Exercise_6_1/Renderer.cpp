#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include <GL/glew.h>
#include "MeshComponent.h"

Renderer::Renderer(Game* game)
    :mGame(game)
    ,mSpriteShader(nullptr)
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// 3D mesh 컴포넌트를 그릴때는 DEPTH 버퍼를 활성화 / 블랜딩을 비활성
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    
    Shader* meshSader = GetShader("Phong");
	// mesh 셰이더 활성화
    meshSader->SetActive();
	// mesh 셰이더에 뷰-투영 행렬 적용
    meshSader->SetMatrixUniform("uViewProj", mView * mProjection);
    // 조명관련 셰이더 변수 설정
    SetLightUniforms(meshSader);

    // 모든 메시에 동일한 셰이더 적용 중.
	for (auto mc : mMeshComps)
	{
       if(mc->GetMesh()->GetShaderName() == "Phong")
		    mc->Draw(meshSader);
	}

    meshSader = GetShader("Basic");
    // mesh 셰이더 활성화
    meshSader->SetActive();
    // mesh 셰이더에 뷰-투영 행렬 적용
    meshSader->SetMatrixUniform("uViewProj", mView * mProjection);
    // 조명관련 셰이더 변수 설정
    SetLightUniforms(meshSader);

    // 모든 메시에 동일한 셰이더 적용 중.
    for (auto mc : mMeshComps)
    {
        if (mc->GetMesh()->GetShaderName() == "Basic")
            mc->Draw(meshSader);
    }

	// 3D 메시 모두 그리고 UI등 2D 스프라이트 그리기 시작
	// DEPTH 버퍼 비활성화
	glDisable(GL_DEPTH_TEST);
	// 알파값을 사용하기 위한 블렌딩 활성화
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// 스프라이트 셰이더와 스프라이트 버텍스 활성화
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
    // 모든 스프라이트를 그린다.
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// Swap the buffers
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
void Renderer::AddMeshComp(MeshComponent* mesh)
{
    mMeshComps.emplace_back(mesh);
}
void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
    mMeshComps.erase(iter);
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
Shader* Renderer::GetShader(const std::string& fileName)
{
    // 반환할 shader의 주소값을 담을 변수
    Shader* newShader = nullptr;

    auto iter = mMeshShaders.find(fileName);
    // fileName에 해당하는 셰이더가 이미 존재한다면
    if (iter != mMeshShaders.end())
    {
        newShader = iter->second;
    }
    else
    {
        newShader = new Shader();

        if (newShader->Load(fileName))
        {
            mMeshShaders.emplace(fileName.c_str(), newShader);
        }
        else
        {
            delete newShader;
            newShader = nullptr;
        }
    }
    return newShader;
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

    mMeshShader = new Shader();
    if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
    {
        return false;
    }
    mMeshShader->SetActive();
    // 3차원 뷰 변환 행렬, MeshShader에 적용
    mView = Matrix4::CreateLookAt(
        Vector3::Zero,  // 카메라 위치
        Vector3::UnitX, // 타깃 위치
        Vector3::UnitZ  // 상향 벡터
    );
    // 3차원 투영 행렬, MeshShader에 적용
    mProjection = Matrix4::CreatePerspectiveFOV(
        Math::ToRadians(70.0f), // 수평 FOV
        mScreenWidth,           // 뷰의 너비
        mScreenHeight,          // 뷰의 높이
        25.0f,                  // 가까운 평면과의 거리
        100000.0f               // 먼 평면과의 거리
    );
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

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
void Renderer::SetLightUniforms(Shader* shader)
{
    // 카메라 위치는 뷰 행렬의 역행렬로 얻을 수 있다.
    Matrix4 invView = mView;
    invView.Invert();
    // 계산된 카메라 위치를 CameraPos에 입력
    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
    // 주변광
    shader->SetVectorUniform("uAmbientLight", mAmbientLight);
    // 방향광 (구조체 멤버를 참조하기 위해 . 표기법 사용)
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);

}