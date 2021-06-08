#include "OrbitActor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "OrbitCamera.h"
#include "MoveComponent.h"
#include "Mesh.h"

OrbitActor::OrbitActor(Game* game)
    :Actor(game)
{
    mMeshComp = new MeshComponent(this);
    mMeshComp->SetMesh(game->GetRenderer()->GetMesh("../Assets/RacingCar.gpmesh"));
    SetPosition(Vector3(0.0f, 0.0f, -100.0f));

    mCameraComp = new OrbitCamera(this);
}

void OrbitActor::ActorInput(const uint8_t* keys)
{
    // 마우스 회전
    // SDL로부터 마우스의 상대좌표를 받음
    int x, y;
    Uint32 buttons = SDL_GetRelativeMouseState(&x, &y);
    // Only apply rotation if right-click is held

    float yawSpeed = 0.0f;
    float pitchSpeed = 0.0f;

    if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
    {
        // 마우스의 최대 이동속도를 500으로 추측
        const int maxMouseSpeed = 500;
        // 초당 최대 회전 속도
        const float maxOrbitSpeed = Math::Pi * 8;

        // yaw 계산
        if (x != 0)
        {
            // [-1.0, 1.0] 범위로 변환
            yawSpeed = static_cast<float>(x) / maxMouseSpeed;
            // 초당 최대 회전 속도를 곱해서 초당 yaw회전 속도 계산
            yawSpeed *= maxOrbitSpeed;
        }

        // pitch 계산
        if (y != 0)
        {
            // [-1.0, 1.0] 범위로 변환
            pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
            pitchSpeed *= maxOrbitSpeed;
        }
    }

    mCameraComp->SetYawSpeed(yawSpeed);
    mCameraComp->SetPitchSpeed(pitchSpeed);
}

void OrbitActor::SetVisible(bool visible)
{
    mMeshComp->SetVisible(visible);
}