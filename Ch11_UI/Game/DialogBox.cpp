#include "DialogBox.h"
#include "Game.h"
#include "Renderer.h"
#include <SDL/SDL.h>

DialogBox::DialogBox(Game* game, const std::string& text, 
    std::function<void()> onOk)
    :UIScreen(game)
{
    // 다이얼로그 박스 위치를 조정
    mBGPos = Vector2(0.0f, 0.0f);
    mTitlePos = Vector2(0.0f, 100.0f);
    mNextButtonPos = Vector2(0.0f, 0.0f);
    // 배경 텍스처 설정
    mBackground = mGame->GetRenderer()->GetTexture("../Assets/DialogBG.png");
    SetTitle(text, Vector3::Zero, 30);
    // 버튼을 추가한다
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