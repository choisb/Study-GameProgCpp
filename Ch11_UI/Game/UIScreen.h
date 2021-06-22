#pragma once
#include "Math.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>
class Button
{
public:
    // 생성자는 이름과 폰트, 콜백 함수
    // 그리고 위치와 너비/높이를 파라미터로 받는다.
    Button(const std::string& name, class Font* font, 
        std::function<void()> onClick,
        const Vector2& pos, const Vector2& dims);
    ~Button();
    // 버튼의 이름을 설정하고 텍스처를 생성한다.
    void SetName(const std::string& name);

    // 점이 버튼 경계 안에 있다면 true를 리턴한다.
    bool ContainsPoint(const Vector2& pt) const;
    // 버튼을 클릭했을 경우 호출된다.
    void OnClick();

    // Getters/setters
    class Texture* GetNameTex() const { return mNameTex; }
    const Vector2& GetPosition() const { return mPosition; }
    bool GetHighlighted() const { return mHighlighted; }
    void SetHighlighted(bool sel) { mHighlighted = sel; }

private:
    std::function<void()> mOnClick;
    std::string mName;
    class Texture* mNameTex;
    class Font* mFont;
    Vector2 mPosition;
    Vector2 mDimensions;
    bool mHighlighted;
};
class UIScreen
{
public:
    UIScreen(class Game* gmae);
    virtual ~UIScreen();
    // UIScreen의 서브클래스는 아래 함수들을 재정의할 수 있다.
    virtual void Update(float deltaTime);
    virtual void Draw(class Shader* shader);
    virtual void ProcessInput(const uint8_t* keys);
    virtual void HandleKeyPress(int key);

    //UI의 활성화 여부를 기록
    enum UIState
    {
        EActive,
        EClosing
    };

    // 상태를 closing으로 설정
    void Close();
    // UI 스크린의 상태를 얻는다.
    UIState GetState() const { return mState; }
    // 제목 텍스트를 변경
    void SetTitle(const std::string& text,
                const Vector3& color = Color::White,
                int pointSize = 40);
    void AddButton(const std::string& name, std::function<void()> onClick);
protected:
    // 텍스처를 그리기 위한 헬퍼 함수
    void DrawTexture(class Shader* shader, class Texture* texture,
        const Vector2& offset = Vector2::Zero, float scale = 1.0f);
    void SetRelativeMouseMode(bool relative);
    class Game* mGame;
    // UI 스크린의 제목 텍스트 렌더링을 위한 정보
    class Font * mFont;
    class Texture* mTitle;
    class Texture* mBackground;
    class Texture* mButtonOn;
    class Texture* mButtonOff;

    Vector2 mTitlePos;
    Vector2 mNextButtonPos;
    Vector2 mBGPos;

    // UI 상태
    UIState mState;
    // 버튼들의 벡터
    std::vector<Button*> mButtons;
};