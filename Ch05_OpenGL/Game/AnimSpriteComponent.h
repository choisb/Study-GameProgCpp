#pragma once
#include "SpriteComponent.h"
#include <vector>

class AnimSpriteComponent : public SpriteComponent
{
public:
    AnimSpriteComponent(class Actor* owner, int drawOrder = 100);

    // �ִϸ��̼��� �����Ӹ��� ���� (Component�� Update �޼��带 �������̵�)
    void Update(float deltaTime) override;

    // �ִϸ��̼ǿ� ���Ǵ� �ؽ�ó ����
    void SetAnimTextures(const std::vector<SDL_Texture*>& textures);

    // �ִϸ��̼� FPS�� ��ų� �����ϴ� �Լ�
    float GetAnimFPS() const { return mAnimFPS; }

private:
    std::vector<SDL_Texture*> mAnimTextures; // �ִϸ��̼ǿ� ���Ǵ� ��� �ؽ�ó
    float mCurrFrame;   // ���� ������
    float mAnimFPS;     // �ִϸ��̼� ������ ����Ʈ
};