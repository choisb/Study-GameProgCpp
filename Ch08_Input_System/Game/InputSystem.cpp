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

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
    return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
    if (mPrevButtons[button] == 0)
    {
        if (mCurrButtons[button] == 0)
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
        if (mCurrButtons[button] == 0)
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

    // Controller
    // 컨트롤러 0의 포인터를 얻는다.
    mController = SDL_GameControllerOpen(0);
    mState.Controller.mIsConnected = (mController != nullptr);
    memset(mState.Controller.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
    memset(mState.Controller.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

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

    // Controller
    memcpy(mState.Controller.mPrevButtons, 
        mState.Controller.mCurrButtons, 
        SDL_CONTROLLER_BUTTON_MAX);
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

    // Controller
    // Buttons
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
    {
        mState.Controller.mCurrButtons[i] =
            SDL_GameControllerGetButton(mController,
                SDL_GameControllerButton(i));
    }

    // Triggers
    mState.Controller.mLeftTrigger =
        Filter1D(SDL_GameControllerGetAxis(mController,
			SDL_CONTROLLER_AXIS_TRIGGERRIGHT));
    mState.Controller.mRightTrigger =
        Filter1D(SDL_GameControllerGetAxis(mController,
            SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

    // Sticks
    x = SDL_GameControllerGetAxis(mController,
        SDL_CONTROLLER_AXIS_LEFTX);
    y = -SDL_GameControllerGetAxis(mController,
        SDL_CONTROLLER_AXIS_LEFTY);
    mState.Controller.mLeftStick = Filter2D(x, y);

    x = SDL_GameControllerGetAxis(mController,
        SDL_CONTROLLER_AXIS_RIGHTX);
    y = -SDL_GameControllerGetAxis(mController,
        SDL_CONTROLLER_AXIS_RIGHTY);
    mState.Controller.mRightStick = Filter2D(x, y);
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
float InputSystem::Filter1D(int input)
{
    // 값이 데드 존보다 작으면 0%로 해석한다.
    const int deadZone = 250;
    // 값이 최댓값보다 크면 100%로 해석한다.
    const int maxValue = 30000;

    float retVal = 0.0f;

    // 입력의 절댓값을 얻는다.
    int absValue = input > 0 ? input : -input;
    // 데드 존 내의 입력값은 무시한다.
    if (absValue > deadZone)
    {
        // 데드 존과 최댓값 사이의 분숫값을 계산
        retVal = static_cast<float>(absValue - deadZone) /
            (maxValue - deadZone);

        // 원래값의 부호와 일치시킨다.
        retVal = input > 0 ? retVal : -1.0f * retVal;

        // 값이 -1.0f 와 1.0f 범위를 벗어나지 않게 한다.
        retVal = Math::Clamp(retVal, -1.0f, 1.0f);
    }
    return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
    const float deadZone = 8000.0f;
    const float maxValue = 30000.0f;

    // 2D 벡터를 만든다
    Vector2 dir;
    dir.x = static_cast<float>(inputX);
    dir.y = static_cast<float>(inputY);

    float length = dir.Length();
    // IF 길이가 데드존 보다 작다면 입력 없음으로 처리
    if (length < deadZone)
    {
        dir = Vector2::Zero;
    }
    else
    {
        // 데드존과 최댓값 동심원 사이의 분숫값을 계산
        float f = (length - deadZone) / (maxValue - deadZone);
        // f 값을 0.0f와 1.0f 사이로 한정
        f = Math::Clamp(f, 0.0f, 1.0f);
        // 벡터를 정규화한뒤 분숫값으로
        // 벡터를 스케일
        dir *= f / length;
    }
    return dir;
}