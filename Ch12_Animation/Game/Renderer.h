#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.h"

struct DirectionalLight
{
    // 빛의 방향
    Vector3 mDirection;
    // 난반사 색상
    Vector3 mDiffuseColor;
    // 정반사 색상
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

    void SetViewMatrix(const Matrix4& view) { mView = view; }

    void SetAmbientLight(const Vector3& ambient) { mAmbientLight = ambient; }
    DirectionalLight& GetDirectionalLight() { return mDirLight; }

    Vector3 Unproject(const Vector3& screenPoint) const;
    void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;
    float GetScreenWidth() const { return mScreenWidth; }
    float GetScreenHeight() const { return mScreenHeight; }

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
    std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;

    // Game
    class Game* mGame;

    // Sprite shader
    class Shader* mSpriteShader;
    // Sprite vertex array
    class VertexArray* mSpriteVerts;

    // Mesh shader
    class Shader* mMeshShader;
    class Shader* mSkinnedShader;

    // View/projection for 3D shaders
    Matrix4 mView;
    Matrix4 mProjection;
    // Widht/ height of screen
    float mScreenWidth;
    float mScreenHeight;

    // 조명관련 변수
    Vector3 mAmbientLight;
    DirectionalLight mDirLight;

    // Window
    SDL_Window* mWindow;
    // OpenGL context
    SDL_GLContext mContext;
};