#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.h"

struct DirectionalLight
{
    // ���� ����
    Vector3 mDirection;
    // ���ݻ� ����
    Vector3 mDiffuseColor;
    // ���ݻ� ����
    Vector3 mSpecColor;
};

class Renderer
{
public:
    Renderer(class Game* game);
    ~Renderer();

    bool Initialize(float screeWidth, float screenHeight);
    void Shutdown();
    void UnloadData();

    void Draw();

    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);

    void AddMeshComp(class MeshComponent* mesh);
    void RemoveMeshComp(class MeshComponent* mesh);

    class Texture* GetTexture(const std::string& fileName);
    class Mesh* GetMesh(const std::string& fileName);
    class Shader* GetShader(const std::string& fileName);

    void SetViewMatrix(const Matrix4& view) { mView = view; }

    void SetAmbientLight(const Vector3& ambient) { mAmbientLight = ambient; }
    DirectionalLight& GetDirectionalLight() { return mDirLight; }


private:
    bool LoadShaders();
    void CreateSpriteVerts();
    void SetLightUniforms(class Shader* shader);

    // Map of textures loaded
    std::unordered_map<std::string, class Texture*> mTextures;
    // Map of meshes loaded
    std::unordered_map<std::string, class Mesh*> mMeshes;

    // All the sprite components drawn
    std::vector<class SpriteComponent*> mSprites;

    // All mesh components drawn
    std::vector<class MeshComponent*> mMeshComps;

    // ��� ���̴��鿡 ���� table
    std::unordered_map<std::string, class Shader*> mMeshShaders;

    // Game
    class Game* mGame;

    // Sprite shader
    class Shader* mSpriteShader;
    // Sprite vertex array
    class VertexArray* mSpriteVerts;


    // View/projection for 3D shaders
    Matrix4 mView;
    Matrix4 mProjection;
    // Widht/ height of screen
    float mScreenWidth;
    float mScreenHeight;

    // ������� ����
    Vector3 mAmbientLight;
    DirectionalLight mDirLight;

    // Window
    SDL_Window* mWindow;
    // OpenGL context
    SDL_GLContext mContext;
};