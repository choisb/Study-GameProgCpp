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
    // ��� ���� ����, mActors�� �ݺ��ϴ� ���� �� ���͸� �����ϴ� ��츦 ���ؼ� ��� ���͸� ���� ���� ���.
    std::vector<class Actor*> mPendingActors;

    // ��°� �����ִ� ��ɵ��� ���ִ� Renderer
    class Renderer* mRenderer;
    class AudioSystem* mAudioSystem;
    class PhysWorld* mPhysWorld;

    Uint32 mTicksCount;
    bool mIsRunning;        // ������ ��� ����ž� �ϴ����� �Ǵ�.
    bool mUpdatingActors;

    // Game-specific code
    std::vector<class PlaneActor*> mPlanes;
    class SpriteComponent* mCrosshair;
    class FPSActor* mFPSActor;

    SoundEvent mMusicEvent;
};