#pragma once
#include <string>
#include <unordered_map>
#include <SDL/SDL_ttf.h>
#include "Math.h"

class Font
{
public:
    Font(class Game* game);
    ~Font();

    // ���Ϸκ��� ��Ʈ�� �ε� �� ��ε�
    bool Load(const std::string& fileName);
    void Unload();
    // �ش� ��Ʈ�� ���ڿ��� �ؽ�ó�� �׸���
    class Texture* RenderText(const std::string& text, 
                            const Vector3& color = Color::White, 
                            int pointSize = 30);

private:
    // ��Ʈ ũ��� ��Ʈ �����͸� ����
    std::unordered_map<int, TTF_Font*> mFontData;
    class Game* mGame;
};