#pragma once
#include "Component.h"
#include "SDL/SDL.h"

class SpriteComponent : public Component
{
public:
    // (�׸��� �������� ���� ���� �� ���ʿ� �׸���.)
    SpriteComponent(class Actor* owner, int drawOrder = 100);
    ~SpriteComponent();
    virtual void Draw(class Shader* renderer);
    virtual void SetTexture(class Texture* shader);

    int GetDrawOrder() const { return mDrawOrder; }
    int GetTexHeight() const { return mTexHeight; }
    int GetTexWidth() const { return mTexWidth; }

protected:
    // �׸� �ؽ�ó
    class Texture* mTexture;
    // ȭ�� �˰��򿡼� ����ϴ� �׸��� ����
    int mDrawOrder;
    // �ؽ�ó�� �ʺ� / ����
    int mTexWidth;
    int mTexHeight;
};