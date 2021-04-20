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
    glClear(GL_COLOR_BUFFER_BIT);

    // Sprite::Draw() �Լ����� glDrawElements()�� ����ȴ�.
    // �� glDrawElements() �Լ��� ����ϱ� ���ؼ��� 
    // "�������Ӹ���" Ȱ��ȭ�� ���ؽ� �迭 ��ü��, ���̴��� �ݵ�� �־�� �Ѵ�.
    // �׷��� Draw()�Լ��� ȣ���ϱ� ���� ������ Ȱ��ȭ �Ѵ�.
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    // ���ĺ����� Ȱ��ȭ
    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,           // srcFactor = srcAlpha
        GL_ONE_MINUS_SRC_ALPHA  // dstFactor = 1 - srcAlpha
    );

    for (auto sprite : mSprites)
    {
        sprite->Draw(mSpriteShader);
    }

    // ���۸� �����ؼ� ����� ����Ѵ�.
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