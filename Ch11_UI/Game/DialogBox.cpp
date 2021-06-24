#include "DialogBox.h"
#include "Game.h"
#include "Renderer.h"
#include <SDL/SDL.h>

DialogBox::DialogBox(Game* game, const std::string& text, 
    std::function<void()> onOk)
    :UIScreen(game)
{
    // ���̾�α� �ڽ� ��ġ�� ����
    mBGPos = Vector2(0.0f, 0.0f);
    mTitlePos = Vector2(0.0f, 100.0f);
    mNextButtonPos = Vector2(0.0f, 0.0f);
    // ��� �ؽ�ó ����
    mBackground = mGame->GetRenderer()->GetTexture("../Assets/DialogBG.png");
    SetTitle(text, Vector3::Zero, 30);
    // ��ư�� �߰��Ѵ�
    AddButton("OKButton", [onOk]() {
        onOk();
    });
    AddButton("CancelButton", [this]() {
        Close();
    });
}

DialogBox::~DialogBox()
{

}