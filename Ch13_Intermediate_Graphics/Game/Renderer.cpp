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
        "Game Programming in C++ (Chapter 13)", // ������ ����
        100,                            // �������� ���� ��� x��ǥ
        100,                            // �������� ���� ��� y��ǥ
        static_cast<int>(mScreenWidth), // �������� �ʺ�
        static_cast<int>(mScreenHeight),// �������� ����
        SDL_WINDOW_OPENGL               // �÷��� (0�� ��� �÷��׵� �������� ������ �ǹ�)
    );
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

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



    // ���̴����� �ε��Ѵ�.
    if (!LoadShaders())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }    // ��������Ʈ�� �׸��� ���� �簢�� ����
    CreateSpriteVerts();

    if (!CreateMirrorTarget())
    {
        SDL_Log("Failed to create render target for mirror.");
        return false;
    }

    // G ���� ����
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
    // OpenGL ���ؽ�Ʈ ����
    SDL_GL_DeleteContext(mContext);
    // mWindow ��ü ����.
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
    // �ſ� �ؽ�ó�� �׸���. (����Ʈ �����ϰ�: 0.25)
    Draw3DScene(mMirrorBuffer, mMirrorView, mProjection, 0.25f);
    // G���۷� 3D ����� �׸���.
    Draw3DScene(mGBuffer->GetBufferID(), mView, mProjection, 1.0f, false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //G���ۿ� ����� �ؽ�ó�� ���� ���� ����� �⺻ �����ӹ��ۿ� �׸���.
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

void Renderer::RemoveSprite(SpriteComponent* sprite)
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
    // ��ȯ�� texture�� �ּҰ��� ���� ����
    Texture* tex = nullptr;

    // unordered_map �����̳ʿ� ����Ǿ� �ִ� mTextures���� fileName���� �˻�
    auto iter = mTextures.find(fileName);
    // fileName�� �ش��ϴ� texture�� �����Ѵٸ�
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();

        // ���Ϸκ��� �ε�
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

    // �ſ� �ؽ�ó�� ���� ������ ���� ����
    glGenFramebuffers(1, &mMirrorBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mMirrorBuffer);

    // �������� ���� ����� �ؽ�ó ����
    mMirrorTexture = new Texture();
    mMirrorTexture->CreateForRendering(width, height, GL_RGB);

    // ���� ���� �߰�
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // �ſ� �ؽ�ó�� ������ ������ ��� Ÿ������ ����
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mMirrorTexture->GetTextureID(), 0);

    // �� ������ ���۰� �׸��� ��� ���� ����Ʈ ����
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // ��� ������ ����� �ƴ��� Ȯ��
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // ������ �߻��ߴٸ� ������ ���� ����
        // �ؽ�ó�� �����ϰ� false ����
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
    // ȭ�� �ʺ� 1024x768�� view proj ��ȯ���
    Matrix4 spriteViewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", spriteViewProj);

    mMeshShader = new Shader();
    if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/GBufferWrite.frag"))
    {
        return false;
    }
    mMeshShader->SetActive();
    // 3���� �� ��ȯ ���, MeshShader�� ����
    mView = Matrix4::CreateLookAt(
        Vector3::Zero,  // ī�޶� ��ġ
        Vector3::UnitX, // Ÿ�� ��ġ
        Vector3::UnitZ  // ���� ����
    );
    // 3���� ���� ���, MeshShader�� ����
    mProjection = Matrix4::CreatePerspectiveFOV(
        Math::ToRadians(70.0f), // ���� FOV
        mScreenWidth,           // ���� �ʺ�
        mScreenHeight,          // ���� ����
        10.0f,                  // ����� ������ �Ÿ�
        100000.0f               // �� ������ �Ÿ�
    );
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // ��Ű�� ���̴� ����
    mSkinnedShader = new Shader();
    if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/GBufferWrite.frag"))
    {
        return false;
    }

    mSkinnedShader->SetActive();
    // view projection ����� �Ϲ� �޽� ���̴��� ����
    mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // GBuffer ���̴� ����
    mGGlobalShader = new Shader();
    if (!mGGlobalShader->Load("Shaders/GBufferGlobal.vert", "Shaders/GBufferGlobal.frag"))
    {
        return false;
    }
    // G���� ���̴��� �����ϵ��� �ε��� ������ �� ���÷��� ������Ŵ
    mGGlobalShader->SetActive();
    mGGlobalShader->SetIntUniform("uGDiffuse", 0);
    mGGlobalShader->SetIntUniform("uGNormal", 1);
    mGGlobalShader->SetIntUniform("uGWorldPos", 2);

    // �� ���� ����� �ϳ��� ��������Ʈ�� �׸��� ���� ���
    mGGlobalShader->SetMatrixUniform("uViewPorj", spriteViewProj);
    // ���� ��ȯ�� ȭ�鿡 �°� ��������Ʋ �����ϰ� y���� ���� ��Ŵ
    Matrix4 gbufferWorld = Matrix4::CreateScale(mScreenWidth, -mScreenHeight, 1.0f);
    mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);

    // ������ ���̴� ����
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
        //  x,     y,    z,    x,   y,   z,   u,   v  // ���ؽ� ��ġ(x,y,z) ���� ����(x,y,z) �ؽ�ó ����(u,v)
        -0.5f,  0.5f,  0.f,  0.f, 0.f, 0.f, 0.f, 0.f, // top left
         0.5f,  0.5f,  0.f,  0.f, 0.f, 0.f, 1.f, 0.f, // top right
         0.5f, -0.5f,  0.f,  0.f, 0.f, 0.f, 1.f, 1.f, // bottom right
        -0.5f, -0.5f,  0.f,  0.f, 0.f, 0.f, 0.f, 1.f  // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    // ��������Ʈ�� �׸��� ���� 4���� ��������Ʈ VertexArray ��������
    // ������ ��� sprite���� �� ��������� ����Ѵ�.
    mSpriteVerts = new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
}
void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
    // ī�޶� ��ġ�� �� ����� ����ķ� ���� �� �ִ�.
    Matrix4 invView = view;
    invView.Invert();
    // ���� ī�޶� ��ġ�� CameraPos�� �Է�
    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
    // �ֺ���
    shader->SetVectorUniform("uAmbientLight", mAmbientLight);
    // ���Ɽ (����ü ����� �����ϱ� ���� . ǥ��� ���)
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);

}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
    // ȭ�� ��ǥ�� ��ġ ��ǥ�� ��ȯ (-1 ���� +1 ����)
    Vector3 deviceCoord = screenPoint;
    deviceCoord.x /= (mScreenWidth) * 0.5f;
    deviceCoord.y /= (mScreenHeight) * 0.5f;

    // ���͸� ���������� ��ķ� ��ȯ
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
    // ���� ������ ���۸� ����
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // ����Ʈ �����ϰ����� ����Ʈ ũ�� ����
    glViewport(0, 0,
        static_cast<int>(mScreenWidth * viewPortScale),
        static_cast<int>(mScreenHeight * viewPortScale)
    );

    // ���� ���� / ���� ���� Ŭ����
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // �޽� ������Ʈ�� �׸���
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // mesh ���̴� Ȱ��ȭ
    mMeshShader->SetActive();
    // mesh ���̴��� ��-���� ��� ����
    mMeshShader->SetMatrixUniform("uViewProj", view * proj);
    // ������� ���̴� ���� ����
    if (lit)
    {
        SetLightUniforms(mMeshShader, view);
    }
    // ��� �޽ÿ� ������ ���̴� ���� ��.
    for (auto mc : mMeshComps)
    {
        if (mc->GetVisible())
        {
            mc->Draw(mMeshShader);
        }
    }

    // ��Ű�� ���̴��� Ȱ���� ��Ű�� �޽� �׸���
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
    // ���� �׽�Ʈ ��Ȱ��ȭ
    glDisable(GL_DEPTH_TEST);
    // ���� G ���� ���̴� Ȱ��ȭ
    mGGlobalShader->SetActive();
    // ��������Ʈ�� ���� ���ؽ� �簢�� Ȱ��ȭ
    mSpriteVerts->SetActive();
    // ���ø��Ϸ��� G ���� �ؽ�ó�� Ȱ��ȭ
    mGBuffer->SetTexturesActive();
    // ���� uniform ����
    SetLightUniforms(mGGlobalShader, mView);

    // �簢���� �׸���
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