#pragma once
#include "SDL/SDL.h"
#include <unordered_map>
#include <string>
#include <vector>

class Game
{
public:
    Game();
    bool Initialize();  // 게임 초기화
    void RunLoop();     // 게임이 끝나기 전까지 게임 루프 실행
    void Shutdown();    // 게임 종료

    // 엑터 추가/삭제
    void AddActor(class Actor* actor);   
    void RemoveActor(class Actor* actor);

    // sprite 추가/삭제
    void AddSprite(class SpriteComponent* sprite);  
    void RemoveSprite(class SpriteComponent* sprite);

    SDL_Texture* GetTexture(const std::string& fileName);

	// Game-specific (add/remove asteroid)
	void AddAsteroid(class Asteroid* ast);
	void RemoveAsteroid(class Asteroid* ast);
	std::vector<class Asteroid*>& GetAsteroids() { return mAsteroids; }

private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    void CreateSpriteVerts();   // 스프라이트를 그리기 위한 버텍스 생성
    void LoadData();
    void UnloadData();
    bool LoadShader();

    // 텍스처 load를 위한 Map
    std::unordered_map<std::string, SDL_Texture*> mTextures;

    // 활성화된 액터
    std::vector<class Actor*> mActors;
    // 대기 중인 액터, mActors를 반복하는 동안 새 액터를 생성하는 경우를 위해서 대기 액터를 위한 벡터 사용.
    std::vector<class Actor*> mPendingActors;

    //sprite를 저장하기 위한 벡터
    std::vector<class SpriteComponent*> mSprites;

    SDL_Window* mWindow;    // SDL이 생성한 window
    bool mIsRunning;        // 게임이 계속 실행돼야 하는지를 판단.
    bool mUpdatingActors;

    SDL_Renderer* mRenderer;

    //OpenGL 변수
    SDL_GLContext mContext; // context는 OpenGL에서 하나의 세계를 뜻한다. (한 프로그램에 복수개의 context도 가능하다.)
    // Sprite vertex array
    class VertexArray* mSpriteVerts;
    // Sprite를 그리기 위한 셰이더
    class Shader* mSpriteShader;

    // 델타 타임 구하기 위한 변수
    Uint32 mTicksCount;

	// Game-specific
	class Ship* mShip; // Player's ship
	std::vector<class Asteroid*> mAsteroids;

};