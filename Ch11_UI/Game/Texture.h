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
    // �� �ؽ�ó�� OpenGL ID
    unsigned int mTextureID;
    // �ؽ�ó�� �ʺ�/����
    int mWidth;
    int mHeight;
};