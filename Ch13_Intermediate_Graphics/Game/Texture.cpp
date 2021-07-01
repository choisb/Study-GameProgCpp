#include "Texture.h"
#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

Texture::Texture()
    :mTextureID(0)
    ,mWidth(0)
    ,mHeight(0)
{

}

Texture::~Texture()
{

}

bool Texture::Load(const std::string& fileName)
{
    int channels = 0;
    unsigned char* image = SOIL_load_image(
        fileName.c_str(),   // 텍스처 파일 이름
        &mWidth,            // 텍스처 너비 저장
        &mHeight,           // 텍스처 높이 저장
        &channels,          // 채널 수 저장
        SOIL_LOAD_AUTO      // 이미지 파일 타입 또는 자동
    );  // 실패시 nullptr 반환.

    if (image == nullptr)
    {
        SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
        return false;
    }

    int format = GL_RGB;
    if (channels == 4)
    {
        format = GL_RGBA;   // 채널이 4개라면 RGBA를 의미함.
    }
    
    // Texture 오브젝트를 생성해서 mTextureID에 ID값 저장
    glGenTextures(1, &mTextureID);
    // mTextureID에 해당하는 텍스처를 메모리에 바인딩하여 활성화
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // 원본 이미지 데이터를 텍스처 오브젝트에 복사
    glTexImage2D(
        GL_TEXTURE_2D,  // 텍스처 타깃
        0,              // LOD (지금은 0으로 가정)
        format,         // OpenGL이 사용해야 되는 색상 포맷
        mWidth,         // 텍스처의 너비
        mHeight,        // 텍스처의 높이
        0,              // 보더 - (이 값은 0이어야 함) 
        format,         // 입력 데이터의 색상 포맷
        GL_UNSIGNED_BYTE,   // 입력 데이터의 비트 깊이, Unsigned byte는 8비트 채널
        image           // 이미지 데이터의 포인터
    );

    // OpenGL에 이미지 데이터를 복사한 후에는 SOIL에 메모리상의 이미지 데이터 해제를 알린다.
    SOIL_free_image_data(image);

    // 이중 선형 필터링을 활성화시킨다.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void Texture::Unload()
{
    glDeleteTextures(1, &mTextureID);
}
void Texture::CreateFromSurface(SDL_Surface* surface)
{
    mWidth = surface->w;
    mHeight = surface->h;

    // Generate a GL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA,
        GL_UNSIGNED_BYTE, surface->pixels);

    // Use linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void Texture::SetActive()
{
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}
