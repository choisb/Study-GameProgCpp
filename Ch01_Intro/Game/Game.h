#pragma once
#include "SDL/SDL.h"

class Game
{
public:
    Game();
    bool Initialize();  // 게임 초기화
    void RunLoop();     // 게임이 끝나기 전까지 게임 루프 실행
    void Shutdown();    // 게임 종료

private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    SDL_Window* mWindow;    // SDL이 생성한 window
    bool mIsRunning;        // 게임이 계속 실행돼야 하는지를 판단.

};