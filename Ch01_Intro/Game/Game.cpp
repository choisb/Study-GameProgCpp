#include "Game.h"
#include <iostream>
using namespace std;

Game::Game()
{
    mBallPos.x = 512;
    mBallPos.y = 384;

    mPaddle1_Pos.x = 20;
    mPaddle1_Pos.y = 384;

    mPaddle2_Pos.x = 1004;
    mPaddle2_Pos.y = 384;

    mTicksCount = 0;

    mBallVel.x = -300.0f;
    mBallVel.y = 205.0f;
}

bool Game::Initialize()
{
    int sdResult = SDL_Init(SDL_INIT_VIDEO);    // 성공시 0을 반환
    if (sdResult != 0)
    {
        // SDL_Log는 C의 Printf와 같은 문법 사용
        // SDL_GetError()는 C 스타일의 문자열로 에러 메시지를 반환.
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
    "Game Programming in C++ (Chapter 1)", // 윈도우 제목
        100,    // 윈도우의 좌측 상단 x좌표
        100,    // 윈도우의 좌측 상단 y좌표
        1024,   // 윈도우의 너비
        768,    // 윈도우의 높이
        0       // 플래그 (0은 어떠한 플래그도 설정되지 않음을 의미)
    );
    
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,    // 렌더링을 위해 생성한 윈도우
        -1,         // 사용할 그래픽카드 선택. -1인 경우 SDL이 알아서 선택 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC    // 플래그 값.가속화 렌더러 사용 여부와 수직 동기화의 활성화 여부 선택.
        );

    return true;
}

void Game::Shutdown()
{
    // mWindow 객체 해제.
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
    SDL_Quit();
}

void Game::RunLoop()
{
    mIsRunning = true;
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}
void Game::ProcessInput()
{
    SDL_Event event;

    // 큐에 여전히 이벤트가 남아 있는 동안
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // 이벤트가 SDL_QUIT이면 루프를 종료한다.
        case SDL_QUIT:
            mIsRunning = false;
            break;
        }
    }

    // 키보드 상태 얻기
    const Uint8* state = SDL_GetKeyboardState(NULL);
    // 이스케이프 키를 눌렀다면 루프 종료
    if (state[SDL_SCANCODE_ESCAPE])
    {
        mIsRunning = false;
    }

    mPaddle1_Dir = 0;
    if (state[SDL_SCANCODE_W])
    {
        mPaddle1_Dir -= 1;
    }
    if (state[SDL_SCANCODE_S])
    {
        mPaddle1_Dir += 1;
    }

    mPaddle2_Dir = 0;
    if (state[SDL_SCANCODE_UP])
    {
        mPaddle2_Dir -= 1;
    }
    if (state[SDL_SCANCODE_DOWN])
    {
        mPaddle2_Dir += 1;
    }
}
void Game::UpdateGame()
{
    // 마지막 프레임 이후로 16ms가 경과할 때 까지 대기
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    // 델타 시간은 현재 프레임 틱값과 이전 프레임 틱값의 차다.
    // (차를 초 단위로 변환)
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    // 최대 델타 시간값으로 고정
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    if (mPaddle1_Dir != 0)
    {
        mPaddle1_Pos.y += mPaddle1_Dir * 300.0f * deltaTime;
        cout <<"mPaddle1_Dir: "<< mPaddle1_Dir<< " | mPaddle1_Pos.y: " << mPaddle1_Pos.y << endl;
        
        // 패들이 화면 영역을 벗아나는지 검증
        if (mPaddle1_Pos.y < (paddleH / 2.0f + thickness))
        {
            mPaddle1_Pos.y = paddleH / 2.0f + thickness;
        }
        else if (mPaddle1_Pos.y > (768.0f - paddleH / 2.0f - thickness))
        {
            mPaddle1_Pos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    if (mPaddle2_Dir != 0)
    {
        mPaddle2_Pos.y += mPaddle2_Dir * 300.0f * deltaTime;
        cout << "mPaddle2_Dir: " << mPaddle2_Dir << " | mPaddle2_Pos.y: " << mPaddle2_Pos.y << endl;

        // 패들이 화면 영역을 벗아나는지 검증
        if (mPaddle2_Pos.y < (paddleH / 2.0f + thickness))
        {
            mPaddle2_Pos.y = paddleH / 2.0f + thickness;
        }
        else if (mPaddle2_Pos.y > (768.0f - paddleH / 2.0f - thickness))
        {
            mPaddle2_Pos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
    {
        mBallVel.y *= -1;
    }
    else if (mBallPos.y >= 768.0f - thickness && mBallVel.y > 0.0f)
    {
        mBallVel.y *= -1;
    }
    

    float diff;
    if (mBallPos.y > mPaddle1_Pos.y)
        diff = mBallPos.y - mPaddle1_Pos.y;
    else
        diff = mPaddle1_Pos.y - mBallPos.y;
    
    if (
        // y 차가 충분히 작고
        diff <= paddleH / 2.0f &&
        // 공이 올바른 x 값을 갖고 있고
        mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
        // 공이 왼쪽으로 이동하고 있다면
        mBallVel.x < 0.0f)
    {
        mBallVel.x *= -1.0f;
    }
    
    if (mBallPos.y > mPaddle2_Pos.y)
        diff = mBallPos.y - mPaddle2_Pos.y;
    else
        diff = mPaddle2_Pos.y - mBallPos.y;

    if (
        // y 차가 충분히 작고
        diff <= paddleH / 2.0f &&
        // 공이 올바른 x 값을 갖고 있고
        mBallPos.x <= 1024.0f - thickness && mBallPos.x >= 1004.0f - thickness &&
        // 공이 왼쪽으로 이동하고 있다면
        mBallVel.x > 0.0f)
    {
        mBallVel.x *= -1.0f;
    }
    // 다음 프레임을 위해 틱값을 계산
    mTicksCount = SDL_GetTicks();

}
void Game::GenerateOutput()
{
    //mRenderer를 (0,0,255,255) 파랑색으로 지정
    SDL_SetRenderDrawColor(
        mRenderer,
        0,      // R
        0,      // G
        255,    // B
        255     // A
    );

    // 현재 그리기 색상으로 후면 버퍼 지우기
    SDL_RenderClear(mRenderer);

    // 게임 장면을 그리는 코드가 들어갈 곳
    //mRenderer를 (255,255,255,255) 흰색으로 지정
    SDL_SetRenderDrawColor(
        mRenderer,
        0,      // R
        255,    // G
        0,      // B
        255     // A
    );

    // 상단벽
    SDL_Rect tWall{
    0,          // 왼쪽 상단 x
    0,          // 왼쪽 상단 y
    1024,       // 너비
    thickness   // 높이
    };

    // 하단 벽
    SDL_Rect bWall{
        0,
        768 - thickness,
        1024,
        thickness
    };

    // 우측 벽
    SDL_Rect rWall{
        1024 - thickness,
        0,
        thickness,
        768
    };

    // 공
    SDL_Rect ball{
        static_cast<int>(mBallPos.x - thickness/2),
        static_cast<int>(mBallPos.y - thickness/2),
        thickness,
        thickness
    };
    
    // 패들
    SDL_Rect paddle1{
        static_cast<int>(mPaddle1_Pos.x - thickness / 2),
        static_cast<int>(mPaddle1_Pos.y - paddleH / 2),
        thickness,
        paddleH
    };

    SDL_Rect paddle2{
        static_cast<int>(mPaddle2_Pos.x - thickness / 2),
        static_cast<int>(mPaddle2_Pos.y - paddleH / 2),
        thickness,
        paddleH
    };

    SDL_RenderFillRect(mRenderer, &tWall);
    SDL_RenderFillRect(mRenderer, &bWall);
    SDL_RenderFillRect(mRenderer, &ball);
    SDL_RenderFillRect(mRenderer, &paddle1);
    SDL_RenderFillRect(mRenderer, &paddle2);


    /////////////////////////////////

    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(mRenderer);

}