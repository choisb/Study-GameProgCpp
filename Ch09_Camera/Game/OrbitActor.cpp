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
    // ���콺 ȸ��
    // SDL�κ��� ���콺�� �����ǥ�� ����
    int x, y;
    Uint32 buttons = SDL_GetRelativeMouseState(&x, &y);
    // Only apply rotation if right-click is held

    float yawSpeed = 0.0f;
    float pitchSpeed = 0.0f;

    if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
    {
        // ���콺�� �ִ� �̵��ӵ��� 500���� ����
        const int maxMouseSpeed = 500;
        // �ʴ� �ִ� ȸ�� �ӵ�
        const float maxOrbitSpeed = Math::Pi * 8;

        // yaw ���
        if (x != 0)
        {
            // [-1.0, 1.0] ������ ��ȯ
            yawSpeed = static_cast<float>(x) / maxMouseSpeed;
            // �ʴ� �ִ� ȸ�� �ӵ��� ���ؼ� �ʴ� yawȸ�� �ӵ� ���
            yawSpeed *= maxOrbitSpeed;
        }

        // pitch ���
        if (y != 0)
        {
            // [-1.0, 1.0] ������ ��ȯ
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