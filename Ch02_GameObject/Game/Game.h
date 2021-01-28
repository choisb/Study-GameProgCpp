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

    void AddActor(class Actor* actor);    // 엑터를 추가
    void RemoveActor(class Actor* actor); // 엑터를 삭제

    SDL_Texture* GetTexture(const std::string& fileName);

private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    void LoadData();
    void UnloadData();

    // 텍스처 load를 위한 Map
    std::unordered_map<std::string, SDL_Texture*> mTextures;

    // 활성화된 액터
    std::vector<class Actor*> mActors;
    // 대기 중인 액터, mActors를 반복하는 동안 새 액터를 생성하는 경우를 위해서 대기 액터를 위한 벡터 사용.
    std::vector<class Actor*> mPendingActors;

    SDL_Window* mWindow;    // SDL이 생성한 window
    bool mIsRunning;        // 게임이 계속 실행돼야 하는지를 판단.
    bool mUpdatingActors;

    SDL_Renderer* mRenderer;

    // 델타 타임 구하기 위한 변수
    Uint32 mTicksCount;


};