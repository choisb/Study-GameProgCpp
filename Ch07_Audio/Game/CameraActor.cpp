#include "CameraActor.h"
#include "MoveComponent.h"
#include "SDL/SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"

CameraActor::CameraActor(Game* game)
    :Actor(game)
{
    mMoveComp = new MoveComponent(this);
}
void CameraActor::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);

    // 카메라의 변화된 위치나 회전에 따라 view 변환 행렬을 새롭게 계산
    Vector3 cameraPos = GetPosition();
    Vector3 target = GetPosition() + GetForward() * 100.f;
    Vector3 up = Vector3::UnitZ;

    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
    GetGame()->GetRenderer()->SetViewMatrix(view);
}

void CameraActor::ActorInput(const uint8_t* keys)
{
    float forwardSpeed = 0.0f;
    float angularSpeed = 0.0f;
    // wasd movement
    if (keys[SDL_SCANCODE_W])
    {
        forwardSpeed += 300.0f;
    }
    if (keys[SDL_SCANCODE_S])
    {
        forwardSpeed -= 300.0f;
    }
    if (keys[SDL_SCANCODE_A])
    {
        angularSpeed -= Math::TwoPi;
    }
    if (keys[SDL_SCANCODE_D])
    {
        angularSpeed += Math::TwoPi;
    }

    mMoveComp->SetForwardSpeed(forwardSpeed);
    mMoveComp->SetAngularSpeed(angularSpeed);
}