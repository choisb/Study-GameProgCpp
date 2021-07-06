#include "GBuffer.h"
#include <GL/glew.h>
#include "Texture.h"

GBuffer::GBuffer()
    :mBufferID(0)
{

}

GBuffer::~GBuffer()
{

}

bool GBuffer::Create(int width, int height)
{
    // ������ ���۸� ����
    glGenFramebuffers(1, &mBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

    // ���� ���۸� �߰�
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
        width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuffer);

    // G ������ �� ����� ���� �ؽ�ó�� ����
    for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
    {
        Texture* tex = new Texture();
        // �� �ؽ�ó�� 32��Ʈ float ��� �ʿ�
        tex->CreateForRendering(width, height, GL_RGB32F);
        mTextures.emplace_back(tex);
        // �� �ؽ�ó�� �ش� ���� ���Կ� ���� �Ͽ� ������ �ؽ�ó�� ���� ������� ���
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
            tex->GetTextureID(), 0);
    }

    // ���� ��� ���� ����
    std::vector<GLenum> attachments;
    for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
    {
        attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
    }

    // �����׸�Ʈ ���̴��� �׸� ���� ����Ʈ ����
    glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());

    // ����� �����ߴ��� ����
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Destroy();
        return false;
    }
    return true;
}

void GBuffer::Destroy()
{
    glDeleteFramebuffers(1, &mBufferID);
    for (Texture* t : mTextures)
    {
        t->Unload();
        delete t;
    }
}

Texture* GBuffer::GetTexture(Type type)
{
    if (mTextures.size() > 0)
    {
        return mTextures[type];
    }
    else
    {
        return nullptr;
    }
}

void GBuffer::SetTexturesActive()
{
    for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
    {
        mTextures[i]->SetActive(i);
    }
}