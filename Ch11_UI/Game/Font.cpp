#include "Font.h"
#include "Texture.h"
#include <vector>
#include "Game.h"

Font::Font(class Game* game)
    :mGame(game)
{

}

Font::~Font()
{

}

bool Font::Load(const std::string& fileName)
{
    // 해당 폰트 사이즈를 지원
    std::vector<int> fontSizes = {
        8, 9,
        10, 11, 12, 14, 16, 18,
        20, 22, 24, 26, 28,
        30, 32, 34, 36, 38,
        40, 42, 44, 46, 48,
        52, 56,
        60, 64, 68,
        72
    };

    for (auto& size : fontSizes)
    {
        TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
        if (font == nullptr)
        {
            SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
            return false;
        }
        mFontData.emplace(size, font);
    }

    return true;
}