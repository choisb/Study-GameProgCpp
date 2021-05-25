#pragma once
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_gamecontroller.h>
#include <SDL/SDL_mouse.h>
#include "Math.h"

enum ButtonState
{
    ENone,
    EPressed,
    EReleased,
    EHeld
};
// Helper for keyboard input
class KeyboardState
{
public:
    // InputSystem�� friend�� �����ؼ� ��� �����͸� ���� �����ϰ� �Ѵ�
    friend class InputSystem;

    // Ű�� ������ true/false�� ��´�
    bool GetKeyValue(SDL_Scancode keyCode) const;

    // ����/���� �������� ���� ���¸� ��´�
    ButtonState GetKeyState(SDL_Scancode keyCode) const;
private:
    // ���� ����
    const Uint8* mCurrState;
    // ���� �������� ����
    Uint8 mPrevState[SDL_NUM_SCANCODES];
};

// Helper for mouse input
class MouseState
{
public:
    // InputSystem�� friend�� �����ؼ� ��� �����͸� ���� �����ϰ� �Ѵ�
    friend class InputSystem;

    // ���콺 ��ġ
    const Vector2& GetPosition() const { return mMousePos; }

    // ��ư
    bool GetButtonValue(int button) const;
    ButtonState GetButtonState(int button) const;
private:
    // ���콺 ��ġ ����
    Vector2 mMousePos;
    // ���콺 ��ũ�� ������ ����
    Vector2 mScrollWheel;
    // ��ư ������ ����
    Uint32 mCurrButtons;
    Uint32 mPrevButtons;
    // ���콺 ����� ����
    bool mIsRelative;
};

// Helper for controller input
class ControllerState
{
public:
    friend class InputSystem;

    // ��ư
    bool GetButtonValue(SDL_GameControllerButton button) const;
    ButtonState GetButtonState(SDL_GameControllerButton button) const;

    float GetLeftTrigger() const { return mLeftTrigger; }
    float GetRightTrigger() const { return mRightTrigger; }

    bool GetIsConnected() const { return mIsConnected; }

private:
    // ����/���� ��ư
    Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];
    Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];
    // ���� ������ ���̽�ƽ
    Vector2 mLeftStick;
    Vector2 mRightStick;
    // ���� ������ Ʈ����
    float mLeftTrigger;
    float mRightTrigger;
    // ��Ʈ�ѷ��� ����� �ִ°�?
    bool mIsConnected;
};

// �Է��� ���� ���¸� �����ϴ� ����
struct InputState
{
    KeyboardState Keyboard;
    MouseState Mouse;
    ControllerState Controller;
};

class InputSystem
{
public:
    // ���� ���� ���ۿ� ���� ���� ���۸� �ʱ�ȭ�Ѵ�.
    bool Initialize();
    void Shutdown();

    // SDL_PollEvents ���� ������ ȣ��ȴ�.
    // ���� ���� ���۸� ���� ���� ���۷� �����Ѵ�.
    void PrepareForUpdate();
    // SDL_PollEvents ���� ���Ŀ� ȣ��ȴ�.
    void Update();
    // SDL_PollEvents �������� ȣ��ȴ�.
    void ProcessEvent(union SDL_Event& event);

    const InputState& GetState()const { return mState; }

    void SetRelativeMouseMode(bool value);
private:
    float Filter1D(int input);
    Vector2 Filter2D(int inputX, int inputY);
    InputState mState;
    SDL_GameController* mController;
};