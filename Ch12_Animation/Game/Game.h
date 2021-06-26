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
    bool Initialize();  // ���� �ʱ�ȭ
    void RunLoop();     // ������ ������ ������ ���� ���� ����
    void Shutdown();    // ���� ����

    // ���� �߰�/����
    void AddActor(class Actor* actor);   
    void RemoveActor(class Actor* actor);

    class Renderer* GetRenderer() { return mRenderer; }
    class AudioSystem* GetAudioSystem() { return mAudioSystem; }
    class PhysWorld* GetPhysWorld() { return mPhysWorld; }
    class HUD* GetHUD() { return mHUD; }
    // UI stack ���� �Լ�
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
    // ���� ������ ���� ���� �Լ�
    void ProcessInput();
    void HandleKeyPress(int key);
    void UpdateGame();
    void GenerateOutput();
    void LoadData();
    void UnloadData();

    // Ȱ��ȭ�� ����
    std::vector<class Actor*> mActors;

    // UI ����
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class Font*> mFonts;

    // ����ȭ�� ���� Map
    std::unordered_map<std::string, std::string> mText;
    // ��� ���� ����, mActors�� �ݺ��ϴ� ���� �� ���͸� �����ϴ� ��츦 ���ؼ� ��� ���͸� ���� ���� ���.
    std::vector<class Actor*> mPendingActors;

    // ��°� �����ִ� ��ɵ��� ���ִ� Renderer
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