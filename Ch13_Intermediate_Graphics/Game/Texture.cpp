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
        0,              // LOD (mipmap 수동 설정시 mipmap 텍스처의 인덱스)
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

    // Mipmap 자동 생성
    glGenerateMipmap(GL_TEXTURE_2D);

    // 이미지 축소시 삼중 선형 필터링 사용
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // 이미지 축소시 최근접 이웃 맵매핑 사용
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
 
    // 이미지 확대시에는 밉맵이 도움되지 않음. 그대로 이중 선형 필터링 사용
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 그래픽 하드웨어가 이방성 필터링 지원하는지 테스트 후 사용
    if (GLEW_EXT_texture_filter_anisotropic)
    {
        // 이방성 최대값을 얻는다.
        GLfloat largest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
        // 이방성 필터링을 최대값으로 활성화한다.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest);
    }
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
void Texture::CreateForRendering(int width, int height, unsigned int format)
{
    mWidth = width;
    mHeight = height;
    // 텍스처 아이디
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    // 이미지 너비 / 높이 설정, 초기 데이터는 null로 설정
    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, nullptr);

    // 텍스처는 최근접 이웃 필터링만 사용
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
void Texture::SetActive(int index /* = 0*/)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}
