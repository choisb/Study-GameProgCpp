#include "FPSActor.h"
#include "MoveComponent.h"
#include "SDL/SDL_scancode.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Mesh.h"

FPSActor::FPSActor(Game* game)
    :Actor(game)
{
    mMoveComp = new MoveComponent(this);
    mAudioComp = new AudioComponent(this);
    mLastFootstep = 0.0f;
    mFootstep = mAudioComp->PlayEvent("event:/Footstep");
    mFootstep.SetPaused(true);

    mCameraComp = new FPSCamera(this);

    mFPSModel = new Actor(game);
    mFPSModel->SetScale(0.75f);
    mMeshComp = new MeshComponent(mFPSModel);
    mMeshComp->SetMesh(game->GetRenderer()->GetMesh("../Assets/Rifle.gpmesh"));
}

void FPSActor::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);

    // ���� ��ġ�� ���� FPS ���� ������� ��ġ ����
    const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
    Vector3 modelPos = GetPosition();
    modelPos += GetForward() * modelOffset.x;
    modelPos += GetRight() * modelOffset.y;
    modelPos.z += modelOffset.z;
    mFPSModel->SetPosition(modelPos);

    // ���� ȸ�������� ȸ������ �ʱ�ȭ
    Quaternion q = GetRotation();

    // ī�޶� ��ġ ������ ȸ��
    q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
    mFPSModel->SetRotation(q);

}

void FPSActor::ActorInput(const uint8_t* keys)
{
    float forwardSpeed = 0.0f;
    float strafeSpeed = 0.0f;

    //w,a,s,d �̵�
    if (keys[SDL_SCANCODE_W])
    {
        forwardSpeed += 400.0f;
    }
    if (keys[SDL_SCANCODE_S])
    {
        forwardSpeed -= 400.0f;
    }
    if (keys[SDL_SCANCODE_A])
    {
        strafeSpeed -= 400.0f;
    }
    if (keys[SDL_SCANCODE_D])
    {
        strafeSpeed += 400.0f;
    }

    mMoveComp->SetForwardSpeed(forwardSpeed);
    mMoveComp->SetStrafeSpeed(strafeSpeed);

    // ���콺 �̵�
    // SDL�� ���� ������� �̵��� ��´�.
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    // ���콺 �̵��� -500���� 500 ������ ���̶�� ����
    const int maxMouseSpeed = 500;
    // �ʴ� ȸ���� �ִ� �ӵ�
    const float maxAngularSpeed = Math::Pi * 8;
    float angularSpeed = 0.0f;
    if (x != 0)
    {
        // [-1.0, 1.0] ������ ��ȯ
        angularSpeed = static_cast<float>(x) / maxMouseSpeed;
        // �ʴ� ȸ���� ���Ѵ�.
        angularSpeed *= maxAngularSpeed;
    }
    mMoveComp->SetAngularSpeed(angularSpeed);

    // ��ġ ���
    const float maxPitchSpeed = Math::Pi * 8;
    float pitchSpeed = 0.0f;
    if (y != 0)
    {
        pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
        pitchSpeed *= maxPitchSpeed;
    }
    mCameraComp->SetPitchSpeed(pitchSpeed);

}

void FPSActor::SetVisible(bool visible)
{
    mMeshComp->SetVisible(visible);
}