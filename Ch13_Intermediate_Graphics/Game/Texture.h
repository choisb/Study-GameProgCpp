#pragma once
#include <string>
class Texture
{
public:
    Texture();
    ~Texture();

    bool Load(const std::string& fileName);
    void Unload();
    void CreateFromSurface(struct SDL_Surface* surface);
    void CreateForRendering(int width, int height, unsigned int format);

    unsigned int GetTextureID() const { return mTextureID; }

    void SetActive(int index = 0);
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
private:
    // �� �ؽ�ó�� OpenGL ID
    unsigned int mTextureID;
    // �ؽ�ó�� �ʺ�/����
    int mWidth;
    int mHeight;
};