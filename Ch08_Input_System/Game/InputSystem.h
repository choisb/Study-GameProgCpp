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
    // InputSystem을 friend로 선언해서 멤버 데이터를 쉽게 갱신하게 한다
    friend class InputSystem;

    // 키의 이진값 true/false를 얻는다
    bool GetKeyValue(SDL_Scancode keyCode) const;

    // 현재/이전 프레임을 토대로 상태를 얻는다
    ButtonState GetKeyState(SDL_Scancode keyCode) const;
private:
    // 현재 상태
    const Uint8* mCurrState;
    // 이전 프레임의 상태
    Uint8 mPrevState[SDL_NUM_SCANCODES];
};

// Helper for mouse input
class MouseState
{
public:
    // InputSystem을 friend로 선언해서 멤버 데이터를 쉽게 갱신하게 한다
    friend class InputSystem;

    // 마우스 위치
    const Vector2& GetPosition() const { return mMousePos; }

    // 버튼
    bool GetButtonValue(int button) const;
    ButtonState GetButtonState(int button) const;
private:
    // 마우스 위치 저장
    Vector2 mMousePos;
    // 마우스 스크롤 움직임 저장
    Vector2 mScrollWheel;
    // 버튼 데이터 저장
    Uint32 mCurrButtons;
    Uint32 mPrevButtons;
    // 마우스 상대모드 상태
    bool mIsRelative;
};

// Helper for controller input
class ControllerState
{

};

// 입력의 현재 상태를 포함하는 래퍼
struct InputState
{
    KeyboardState Keyboard;
    MouseState Mouse;
    ControllerState Controller;
};

class InputSystem
{
public:
    // 현재 상태 버퍼와 이전 상태 버퍼를 초기화한다.
    bool Initialize();
    void Shutdown();

    // SDL_PollEvents 루프 직전에 호출된다.
    // 현재 상태 버퍼를 이전 상태 버퍼로 복사한다.
    void PrepareForUpdate();
    // SDL_PollEvents 루프 직후에 호출된다.
    void Update();
    // SDL_PollEvents 루프에서 호출된다.
    void ProcessEvent(union SDL_Event& event);

    const InputState& GetState()const { return mState; }

    void SetRelativeMouseMode(bool value);
private:
    InputState mState;
    SDL_GameController* mController;
};