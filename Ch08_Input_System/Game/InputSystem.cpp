#include "InputSystem.h"
#include <SDL/SDL.h>
#include <cstring>

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
    return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
    if (mPrevState[keyCode] == 0)
    {
        if (mCurrState[keyCode] == 0)
        {
            return ENone;
        }
        else
        {
            return EPressed;
        }
    }
    else // Prev state must be 1
    {
        if (mCurrState[keyCode] == 0)
        {
            return EReleased;
        }
        else
        {
            return EHeld;
        }
    }
}

bool MouseState::GetButtonValue(int button) const
{
    return SDL_BUTTON(button) & mCurrButtons == 1;
}
ButtonState MouseState::GetButtonState(int button) const
{
    int mask = SDL_BUTTON(button);
    if ((mask & mPrevButtons) == 0)
    {
        if ((mask & mCurrButtons) == 0)
        {
            return ENone;
        }
        else
        {
            return EPressed;
        }
    }
    else // prev state must be 1
    {
        if ((mask & mCurrButtons) == 0)
        {
            return EReleased;
        }
        else
        {
            return EHeld;
        }
    }
}

bool InputSystem::Initialize()
{
    // keyboard
    // Assign current state pointer
    mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
    // previous state는 0으로 초기화
    memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

    // Mouse
    mState.Mouse.mCurrButtons = 0;
    mState.Mouse.mPrevButtons = 0;

    //Controller
    // 컨트롤러 0의 포인터를 얻는다.
    mController = SDL_GameControllerOpen(0);

    return true;
}
void InputSystem::Shutdown()
{

}
void InputSystem::PrepareForUpdate()
{
    // 현재 상태 버퍼를 이전 상태 버퍼로 복사
    // keyboard
    memcpy(mState.Keyboard.mPrevState,
        mState.Keyboard.mCurrState,
        SDL_NUM_SCANCODES);

    // mouse
    mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
    mState.Mouse.mScrollWheel = Vector2::Zero;
}
void InputSystem::Update()
{
    int x = 0, y = 0;
    if (mState.Mouse.mIsRelative)
    {
        mState.Mouse.mCurrButtons =
            SDL_GetRelativeMouseState(&x, &y);
    }
    else
    {
        mState.Mouse.mCurrButtons =
            SDL_GetMouseState(&x, &y);
    }

    mState.Mouse.mMousePos.x = static_cast<float>(x);
    mState.Mouse.mMousePos.y = static_cast<float>(y);
}
void InputSystem::ProcessEvent(SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEWHEEL:
        mState.Mouse.mScrollWheel = Vector2(
            static_cast<float>(event.wheel.x),
            static_cast<float>(event.wheel.y));
        break;
    default:
        break;
    }
}
void InputSystem::SetRelativeMouseMode(bool value)
{
    SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
    SDL_SetRelativeMouseMode(set);

    mState.Mouse.mIsRelative = value;
}