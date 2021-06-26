#pragma once
#include <SDL/SDL.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "SoundEvent.h"

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

    class Renderer* GetRenderer() { return mRenderer; }
    class AudioSystem* GetAudioSystem() { return mAudioSystem; }
    class PhysWorld* GetPhysWorld() { return mPhysWorld; }
    class HUD* GetHUD() { return mHUD; }
    // UI stack 관리 함수
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }
    void PushUI(class UIScreen* screen);

    class FollowActor* GetPlayer() { return mFollowActor; }

    enum GameState
    {
        EGameplay,
        EPaused,
        EQuit
    };

    GameState GetState() const { return mGameState; }
    void SetState(GameState state) { mGameState = state; }

    class Font* GetFont(const std::string& fileName);
    
    void LoadText(const std::string& fileName);
    const std::string& GetText(const std::string& key);

    // Game-specific
    void AddPlane(class PlaneActor* plane);
    void RemovePlane(class PlaneActor* plane);
    std::vector<class PlaneActor*>& GetPlanes() { return mPlanes; }

private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void HandleKeyPress(int key);
    void UpdateGame();
    void GenerateOutput();
    void LoadData();
    void UnloadData();

    // 활성화된 액터
    std::vector<class Actor*> mActors;

    // UI 관련
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class Font*> mFonts;

    // 현지화를 위한 Map
    std::unordered_map<std::string, std::string> mText;
    // 대기 중인 액터, mActors를 반복하는 동안 새 액터를 생성하는 경우를 위해서 대기 액터를 위한 벡터 사용.
    std::vector<class Actor*> mPendingActors;

    // 출력과 관련있는 기능들이 모여있는 Renderer
    class Renderer* mRenderer;
    class AudioSystem* mAudioSystem;
    class PhysWorld* mPhysWorld;
    class HUD* mHUD;

    Uint32 mTicksCount;
    GameState mGameState;
    bool mUpdatingActors;

    // Game-specific code
    std::vector<class PlaneActor*> mPlanes;
    class FollowActor* mFollowActor;

    SoundEvent mMusicEvent;
};