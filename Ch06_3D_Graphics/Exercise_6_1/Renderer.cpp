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
        "Game Programming in C++ (Chapter 6)", // ������ ����
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

    return true;
}

void Renderer::Shutdown()
{
    delete mSpriteVerts;
    // OpenGL ���ؽ�Ʈ ����
    mSpriteShader->Unload();
    delete mSpriteShader;
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
    // ������ ȸ������ ����
    glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    // ���� ���� �ʱ�ȭ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// 3D mesh ������Ʈ�� �׸����� DEPTH ���۸� Ȱ��ȭ / ������ ��Ȱ��
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    
    Shader* meshSader = GetShader("Phong");
	// mesh ���̴� Ȱ��ȭ
    meshSader->SetActive();
	// mesh ���̴��� ��-���� ��� ����
    meshSader->SetMatrixUniform("uViewProj", mView * mProjection);
    // ������� ���̴� ���� ����
    SetLightUniforms(meshSader);

    // ��� �޽ÿ� ������ ���̴� ���� ��.
	for (auto mc : mMeshComps)
	{
       if(mc->GetMesh()->GetShaderName() == "Phong")
		    mc->Draw(meshSader);
	}

    meshSader = GetShader("Basic");
    // mesh ���̴� Ȱ��ȭ
    meshSader->SetActive();
    // mesh ���̴��� ��-���� ��� ����
    meshSader->SetMatrixUniform("uViewProj", mView * mProjection);
    // ������� ���̴� ���� ����
    SetLightUniforms(meshSader);

    // ��� �޽ÿ� ������ ���̴� ���� ��.
    for (auto mc : mMeshComps)
    {
        if (mc->GetMesh()->GetShaderName() == "Basic")
            mc->Draw(meshSader);
    }

	// 3D �޽� ��� �׸��� UI�� 2D ��������Ʈ �׸��� ����
	// DEPTH ���� ��Ȱ��ȭ
	glDisable(GL_DEPTH_TEST);
	// ���İ��� ����ϱ� ���� ���� Ȱ��ȭ
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// ��������Ʈ ���̴��� ��������Ʈ ���ؽ� Ȱ��ȭ
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
    // ��� ��������Ʈ�� �׸���.
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
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
    mMeshComps.emplace_back(mesh);
}
void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
    mMeshComps.erase(iter);
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
Shader* Renderer::GetShader(const std::string& fileName)
{
    // ��ȯ�� shader�� �ּҰ��� ���� ����
    Shader* newShader = nullptr;

    auto iter = mMeshShaders.find(fileName);
    // fileName�� �ش��ϴ� ���̴��� �̹� �����Ѵٸ�
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
    // ȭ�� �ʺ� 1024x768�� view proj ��ȯ���
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    mMeshShader = new Shader();
    if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
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
        25.0f,                  // ����� ������ �Ÿ�
        100000.0f               // �� ������ �Ÿ�
    );
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

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
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}
void Renderer::SetLightUniforms(Shader* shader)
{
    // ī�޶� ��ġ�� �� ����� ����ķ� ���� �� �ִ�.
    Matrix4 invView = mView;
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