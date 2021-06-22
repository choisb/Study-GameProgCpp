#pragma once
#include "Math.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>
class Button
{
public:
    // �����ڴ� �̸��� ��Ʈ, �ݹ� �Լ�
    // �׸��� ��ġ�� �ʺ�/���̸� �Ķ���ͷ� �޴´�.
    Button(const std::string& name, class Font* font, 
        std::function<void()> onClick,
        const Vector2& pos, const Vector2& dims);
    ~Button();
    // ��ư�� �̸��� �����ϰ� �ؽ�ó�� �����Ѵ�.
    void SetName(const std::string& name);

    // ���� ��ư ��� �ȿ� �ִٸ� true�� �����Ѵ�.
    bool ContainsPoint(const Vector2& pt) const;
    // ��ư�� Ŭ������ ��� ȣ��ȴ�.
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
    // UIScreen�� ����Ŭ������ �Ʒ� �Լ����� �������� �� �ִ�.
    virtual void Update(float deltaTime);
    virtual void Draw(class Shader* shader);
    virtual void ProcessInput(const uint8_t* keys);
    virtual void HandleKeyPress(int key);

    //UI�� Ȱ��ȭ ���θ� ���
    enum UIState
    {
        EActive,
        EClosing
    };

    // ���¸� closing���� ����
    void Close();
    // UI ��ũ���� ���¸� ��´�.
    UIState GetState() const { return mState; }
    // ���� �ؽ�Ʈ�� ����
    void SetTitle(const std::string& text,
                const Vector3& color = Color::White,
                int pointSize = 40);
    void AddButton(const std::string& name, std::function<void()> onClick);
protected:
    // �ؽ�ó�� �׸��� ���� ���� �Լ�
    void DrawTexture(class Shader* shader, class Texture* texture,
        const Vector2& offset = Vector2::Zero, float scale = 1.0f);
    void SetRelativeMouseMode(bool relative);
    class Game* mGame;
    // UI ��ũ���� ���� �ؽ�Ʈ �������� ���� ����
    class Font * mFont;
    class Texture* mTitle;
    class Texture* mBackground;
    class Texture* mButtonOn;
    class Texture* mButtonOff;

    Vector2 mTitlePos;
    Vector2 mNextButtonPos;
    Vector2 mBGPos;

    // UI ����
    UIState mState;
    // ��ư���� ����
    std::vector<Button*> mButtons;
};