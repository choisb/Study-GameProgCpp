#pragma once
#include <string>
class Texture
{
public:
    Texture();
    ~Texture();

    bool Load(const std::string& fileName);
    void Unload();

    void SetActive();
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
private:
    // 이 텍스처의 OpenGL ID
    unsigned int mTextureID;
    // 텍스처의 너비/높이
    int mWidth;
    int mHeight;
};