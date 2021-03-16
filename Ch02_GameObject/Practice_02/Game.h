#pragma once
#include "SDL/SDL.h"
#include <unordered_map>
#include <string>
#include <vector>

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

    SDL_Texture* GetTexture(const std::string& fileName);

private:
    // ���� ������ ���� ���� �Լ�
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    void LoadData();
    void UnloadData();

    // �ؽ�ó load�� ���� Map
    std::unordered_map<std::string, SDL_Texture*> mTextures;

    // Ȱ��ȭ�� ����
    std::vector<class Actor*> mActors;
    // ��� ���� ����, mActors�� �ݺ��ϴ� ���� �� ���͸� �����ϴ� ��츦 ���ؼ� ��� ���͸� ���� ���� ���.
    std::vector<class Actor*> mPendingActors;

    //sprite�� �����ϱ� ���� ����
    std::vector<class SpriteComponent*> mSprites;

    SDL_Window* mWindow;    // SDL�� ������ window
    bool mIsRunning;        // ������ ��� ����ž� �ϴ����� �Ǵ�.
    bool mUpdatingActors;

    SDL_Renderer* mRenderer;

    // ��Ÿ Ÿ�� ���ϱ� ���� ����
    Uint32 mTicksCount;

    // �÷��̾� ship
    class Ship* mShip;


};