#pragma once
#include <SDL/SDL.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"

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

    // sprite �߰�/����
    void AddSprite(class SpriteComponent* sprite);  
    void RemoveSprite(class SpriteComponent* sprite);

    class Renderer* GetRenderer() { return mRenderer; }

private:
    // ���� ������ ���� ���� �Լ�
    void ProcessInput();
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

    Uint32 mTicksCount;
    bool mIsRunning;        // ������ ��� ����ž� �ϴ����� �Ǵ�.
    bool mUpdatingActors;
};