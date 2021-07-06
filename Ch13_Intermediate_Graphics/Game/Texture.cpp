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
        fileName.c_str(),   // �ؽ�ó ���� �̸�
        &mWidth,            // �ؽ�ó �ʺ� ����
        &mHeight,           // �ؽ�ó ���� ����
        &channels,          // ä�� �� ����
        SOIL_LOAD_AUTO      // �̹��� ���� Ÿ�� �Ǵ� �ڵ�
    );  // ���н� nullptr ��ȯ.

    if (image == nullptr)
    {
        SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
        return false;
    }

    int format = GL_RGB;
    if (channels == 4)
    {
        format = GL_RGBA;   // ä���� 4����� RGBA�� �ǹ���.
    }
    
    // Texture ������Ʈ�� �����ؼ� mTextureID�� ID�� ����
    glGenTextures(1, &mTextureID);
    // mTextureID�� �ش��ϴ� �ؽ�ó�� �޸𸮿� ���ε��Ͽ� Ȱ��ȭ
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // ���� �̹��� �����͸� �ؽ�ó ������Ʈ�� ����
    glTexImage2D(
        GL_TEXTURE_2D,  // �ؽ�ó Ÿ��
        0,              // LOD (mipmap ���� ������ mipmap �ؽ�ó�� �ε���)
        format,         // OpenGL�� ����ؾ� �Ǵ� ���� ����
        mWidth,         // �ؽ�ó�� �ʺ�
        mHeight,        // �ؽ�ó�� ����
        0,              // ���� - (�� ���� 0�̾�� ��) 
        format,         // �Է� �������� ���� ����
        GL_UNSIGNED_BYTE,   // �Է� �������� ��Ʈ ����, Unsigned byte�� 8��Ʈ ä��
        image           // �̹��� �������� ������
    );

    // OpenGL�� �̹��� �����͸� ������ �Ŀ��� SOIL�� �޸𸮻��� �̹��� ������ ������ �˸���.
    SOIL_free_image_data(image);

    // Mipmap �ڵ� ����
    glGenerateMipmap(GL_TEXTURE_2D);

    // �̹��� ��ҽ� ���� ���� ���͸� ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // �̹��� ��ҽ� �ֱ��� �̿� �ʸ��� ���
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
 
    // �̹��� Ȯ��ÿ��� �Ӹ��� ������� ����. �״�� ���� ���� ���͸� ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // �׷��� �ϵ��� �̹漺 ���͸� �����ϴ��� �׽�Ʈ �� ���
    if (GLEW_EXT_texture_filter_anisotropic)
    {
        // �̹漺 �ִ밪�� ��´�.
        GLfloat largest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
        // �̹漺 ���͸��� �ִ밪���� Ȱ��ȭ�Ѵ�.
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
    // �ؽ�ó ���̵�
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    // �̹��� �ʺ� / ���� ����, �ʱ� �����ʹ� null�� ����
    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, nullptr);

    // �ؽ�ó�� �ֱ��� �̿� ���͸��� ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
void Texture::SetActive(int index /* = 0*/)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}
