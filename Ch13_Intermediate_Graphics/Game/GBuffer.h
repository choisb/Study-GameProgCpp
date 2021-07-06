#pragma once
#include <vector>

class GBuffer
{
public:
    // G ���ۿ� ����Ǵ� ���� Ÿ���� ������
    enum Type
    {
        EDiffuse = 0,
        ENormal,
        EWorldPos,
        NUM_GBUFFER_TEXTURES
    };

    GBuffer();
    ~GBuffer();

    // G ���� ���� �� ����
    bool Create(int width, int height);
    void Destroy();

    // Ư�� ������ Ÿ���� �ؽ�ó�� ��´�.
    class Texture* GetTexture(Type type);
    // ������ ���� ��ü ID�� ��´�
    unsigned int GetBufferID() const { return mBufferID; }
    // ���ø��� ���� ��� G ���� �ؽ�ó�� Ȱ��ȭ�Ѵ�.
    void SetTexturesActive();

private:
    // G���ۿ� ������ �ؽ�ó��
    std::vector<class Texture*> mTextures;
    // ������ ���� ��ü ID
    unsigned int mBufferID;
};