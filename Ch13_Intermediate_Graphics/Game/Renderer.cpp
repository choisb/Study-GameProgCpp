#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include <GL/glew.h>
#include "MeshComponent.h"
#include "UIScreen.h"
#include "Game.h"
#include "SkeletalMeshComponent.h"
#include "GBuffer.h"
#include "PointLightComponent.h"

Renderer::Renderer(Game* game)
    :mGame(game)
    ,mSpriteShader(nullptr)
    ,mMeshShader(nullptr)
    ,mSkinnedShader(nullptr)
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
        "Game Programming in C++ (Chapter 13)", // 윈도우 제목
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

    if (!CreateMirrorTarget())
    {
        SDL_Log("Failed to create render target for mirror.");
        return false;
    }

    // G 버퍼 생성
    mGBuffer = new GBuffer();
    int width = static_cast<int>(mScreenWidth);
    int height = static_cast<int>(mScreenHeight);
    if (!mGBuffer->Create(width, height))
    {
        SDL_Log("Failed to create G-buffer.");
        return false;
    }

    mPointLightMesh = GetMesh("../Assets/PointLight.gpmesh");
    return true;
}

void Renderer::Shutdown()
{
    if (mMirrorTexture != nullptr)
    {
        glDeleteFramebuffers(1, &mMirrorBuffer);
        mMirrorTexture->Unload();
        delete mMirrorTexture;
    }

    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;
    mMeshShader->Unload();
    delete mMeshShader;
    // OpenGL 콘텍스트 제거
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
    // 거울 텍스처를 그린다. (뷰포트 스케일값: 0.25)
    Draw3DScene(mMirrorBuffer, mMirrorView, mProjection, 0.25f);
    // G버퍼로 3D 장면을 그린다.
    Draw3DScene(mGBuffer->GetBufferID(), mView, mProjection, 1.0f, false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //G버퍼에 저장된 텍스처로 부터 최종 장면을 기본 프레임버퍼에 그린다.
    DrawFromGBuffer();

    glDisable(GL_DEPTH_TEST);
    // Enable alpha blending on the color buffer
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // Set shader/vao as active
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();
    for (auto sprite : mSprites)
    {
        if (sprite->GetVisible())
        {
            sprite->Draw(mSpriteShader);
        }
    }

    // Draw any UI screens
    for (auto ui : mGame->GetUIStack())
    {
        ui->Draw(mSpriteShader);
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
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
        mSkeletalMeshes.emplace_back(sk);
    }
    else
    {
        mMeshComps.emplace_back(mesh);
    }
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
        auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
        mSkeletalMeshes.erase(iter);
    }
    else
    {
        auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
        mMeshComps.erase(iter);
    }
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
bool Renderer::CreateMirrorTarget()
{
    int width = static_cast<int>(mScreenWidth) / 4;
    int height = static_cast<int>(mScreenHeight) / 4;

    // 거울 텍스처를 위한 프레임 버퍼 생성
    glGenFramebuffers(1, &mMirrorBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mMirrorBuffer);

    // 렌더링을 위해 사용할 텍스처 생성
    mMirrorTexture = new Texture();
    mMirrorTexture->CreateForRendering(width, height, GL_RGB);

    // 깊이 버퍼 추가
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // 거울 텍스처를 프레임 버퍼의 출력 타깃으로 설정
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mMirrorTexture->GetTextureID(), 0);

    // 이 프레임 버퍼가 그리는 출력 버퍼 리스트 설정
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // 모든 설정이 제대로 됐는지 확인
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // 문제가 발생했다면 프레임 버퍼 삭제
        // 텍스처를 삭제하고 false 리턴
        glDeleteFramebuffers(1, &mMirrorBuffer);
        mMirrorTexture->Unload();
        delete mMirrorTexture;
        mMirrorTexture = nullptr;
        return false;
    }
    return true;
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
    Matrix4 spriteViewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", spriteViewProj);

    mMeshShader = new Shader();
    if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/GBufferWrite.frag"))
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
        10.0f,                  // 가까운 평면과의 거리
        100000.0f               // 먼 평면과의 거리
    );
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // 스키닝 셰이더 생성
    mSkinnedShader = new Shader();
    if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/GBufferWrite.frag"))
    {
        return false;
    }

    mSkinnedShader->SetActive();
    // view projection 행렬은 일반 메시 셰이더와 동일
    mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // GBuffer 셰이더 생성
    mGGlobalShader = new Shader();
    if (!mGGlobalShader->Load("Shaders/GBufferGlobal.vert", "Shaders/GBufferGlobal.frag"))
    {
        return false;
    }
    // G버퍼 셰이더에 제공하도록 인덱스 값으로 각 샘플러를 연관시킴
    mGGlobalShader->SetActive();
    mGGlobalShader->SetIntUniform("uGDiffuse", 0);
    mGGlobalShader->SetIntUniform("uGNormal", 1);
    mGGlobalShader->SetIntUniform("uGWorldPos", 2);

    // 뷰 투영 행렬은 하나의 스프라이트를 그리기 위한 행렬
    mGGlobalShader->SetMatrixUniform("uViewPorj", spriteViewProj);
    // 세계 변환은 화면에 맞게 스프라이틀 조정하고 y값을 반전 시킴
    Matrix4 gbufferWorld = Matrix4::CreateScale(mScreenWidth, -mScreenHeight, 1.0f);
    mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);

    // 점광원 셰이더 생성
    mGPointLightShader = new Shader();
    if (!mGPointLightShader->Load("Shaders/BasicMesh.vert",
        "Shaders/GBufferPointLight.frag"))
    {
        return false;
    }
    // Set sampler indices
    mGPointLightShader->SetActive();
    mGPointLightShader->SetIntUniform("uGDiffuse", 0);
    mGPointLightShader->SetIntUniform("uGNormal", 1);
    mGPointLightShader->SetIntUniform("uGWorldPos", 2);
    mGPointLightShader->SetVector2Uniform("uScreenDimensions",
        Vector2(mScreenWidth, mScreenHeight));
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
    mSpriteVerts = new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
}
void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
    // 카메라 위치는 뷰 행렬의 역행렬로 얻을 수 있다.
    Matrix4 invView = view;
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

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
    // 화면 좌표를 장치 좌표로 변환 (-1 에서 +1 범위)
    Vector3 deviceCoord = screenPoint;
    deviceCoord.x /= (mScreenWidth) * 0.5f;
    deviceCoord.y /= (mScreenHeight) * 0.5f;

    // 벡터를 언프로젝션 행렬로 변환
    Matrix4 unprojection = mView * mProjection;
    unprojection.Invert();
    return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
    // Get start point (in center of screen on near plane)
    Vector3 screenPoint(0.0f, 0.0f, 0.0f);
    outStart = Unproject(screenPoint);
    // Get end point (in center of screen, between near and far)
    screenPoint.z = 0.9f;
    Vector3 end = Unproject(screenPoint);
    // Get direction vector
    outDir = end - outStart;
    outDir.Normalize();
}

void Renderer::Draw3DScene(unsigned int framebuffer,
    const Matrix4& view, const Matrix4& proj, 
    float viewPortScale /*= 1.0f*/, bool lit /*true*/)
{
    // 현재 프레임 버퍼를 설정
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 뷰포트 스케일값으로 뷰포트 크기 설정
    glViewport(0, 0,
        static_cast<int>(mScreenWidth * viewPortScale),
        static_cast<int>(mScreenHeight * viewPortScale)
    );

    // 색상 버퍼 / 깊이 버퍼 클리어
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 메시 컴포넌트를 그린다
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // mesh 셰이더 활성화
    mMeshShader->SetActive();
    // mesh 셰이더에 뷰-투영 행렬 적용
    mMeshShader->SetMatrixUniform("uViewProj", view * proj);
    // 조명관련 셰이더 변수 설정
    if (lit)
    {
        SetLightUniforms(mMeshShader, view);
    }
    // 모든 메시에 동일한 셰이더 적용 중.
    for (auto mc : mMeshComps)
    {
        if (mc->GetVisible())
        {
            mc->Draw(mMeshShader);
        }
    }

    // 스키닝 셰이더를 활용한 스키닝 메시 그리기
    mSkinnedShader->SetActive();
    mSkinnedShader->SetMatrixUniform("uViewProj", view * mProjection);
    if (lit)
    {
        SetLightUniforms(mSkinnedShader, view);
    }
    for (auto sk : mSkeletalMeshes)
    {
        if (sk->GetVisible())
        {
            sk->Draw(mSkinnedShader);
        }
    }
}

void Renderer::DrawFromGBuffer()
{
    // 깊이 테스트 비활성화
    glDisable(GL_DEPTH_TEST);
    // 전역 G 버퍼 셰이더 활성화
    mGGlobalShader->SetActive();
    // 스프라이트를 위한 버텍스 사각형 활성화
    mSpriteVerts->SetActive();
    // 샘플링하려는 G 버퍼 텍스처를 활성화
    mGBuffer->SetTexturesActive();
    // 조명 uniform 설정
    SetLightUniforms(mGGlobalShader, mView);

    // 사각형을 그린다
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Copy depth buffer from G-buffer to default frame buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->GetBufferID());
    int width = static_cast<int>(mScreenWidth);
    int height = static_cast<int>(mScreenHeight);
    glBlitFramebuffer(0, 0, width, height,
        0, 0, width, height,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Enable depth test, but disable writes to depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Set the point light shader and mesh as active
    mGPointLightShader->SetActive();
    mPointLightMesh->GetVertexArray()->SetActive();
    // Set the view-projeciton matrix
    mGPointLightShader->SetMatrixUniform("uViewProj",
        mView * mProjection);
    // Set the G-buffer textures for sampling
    mGBuffer->SetTexturesActive();

    // The point light color should add to existing color
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Draw the point lights
    for (PointLightComponent* p : mPointLights)
    {
        p->Draw(mGPointLightShader, mPointLightMesh);
    }
}
void Renderer::AddPointLight(PointLightComponent* light)
{
    mPointLights.emplace_back(light);
}

void Renderer::RemovePointLight(PointLightComponent* light)
{
    auto iter = std::find(mPointLights.begin(), mPointLights.end(), light);
    mPointLights.erase(iter);
}