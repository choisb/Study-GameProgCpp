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
#include "BoxComponent.h"
#include "PlaneActor.h"
#include "BallActor.h"

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
    mMeshComp = new MeshComponent(mFPSModel, false);
    mMeshComp->SetMesh(game->GetRenderer()->GetMesh("../Assets/Rifle.gpmesh"));

    // Add a box component
    mBoxComp = new BoxComponent(this);
    AABB myBox(Vector3(-25.0f, -25.0f, -87.5f),
        Vector3(25.0f, 25.0f, 87.5f));
    mBoxComp->SetObjectBox(myBox);
    mBoxComp->SetShouldRotate(false);
}

void FPSActor::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);

    FixCollisions();

    mLastFootstep -= deltaTime;
    if ((!Math::NearZero(mMoveComp->GetForwardSpeed()) ||
        !Math::NearZero(mMoveComp->GetStrafeSpeed())) &&
        mLastFootstep <= 0.0f)
    {
        mFootstep.SetPaused(false);
        mFootstep.Restart();
        mLastFootstep = 0.5f;
    }

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
void FPSActor::SetFootstepSurface(float value)
{
    // Pause here because the way I setup the parameter in FMOD
    // changing it will play a footstep
    mFootstep.SetPaused(true);
    mFootstep.SetParameter("Surface", value);
}
void FPSActor::SetVisible(bool visible)
{
    mMeshComp->SetVisible(visible);
}
void FPSActor::Shoot()
{
    // Get start point (in center of screen on near plane)
    Vector3 screenPoint(0.0f, 0.0f, 0.0f);
    Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
    // Get end point (in center of screen, between near and far)
    screenPoint.z = 0.9f;
    Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
    // Get direction vector
    Vector3 dir = end - start;
    dir.Normalize();
    // Spawn a ball
    BallActor* ball = new BallActor(GetGame());
    ball->SetPlayer(this);
    ball->SetPosition(start + dir * 20.0f);
    // Rotate the ball to face new direction
    ball->RotateToNewForward(dir);
    // Play shooting sound
    mAudioComp->PlayEvent("event:/Shot");
}

void FPSActor::FixCollisions()
{
    // �ڽ��� �����ϱ� ���� �ڽ��� ���� ��ȯ ������ �ʿ�
    ComputeWorldTransform();

    const AABB& playerBox = mBoxComp->GetWorldBox();
    Vector3 pos = GetPosition();

    auto& planes = GetGame()->GetPlanes();
    for (auto pa : planes)
    {
        // �� planeActor�� �浹�ϴ°�?
        const AABB& planeBox = pa->GetBox()->GetWorldBox();
        if (Intersect(playerBox, planeBox))
        {
            // �� ������ ��ħ���� ���
            float dx1 = planeBox.mMin.x - playerBox.mMax.x;
            float dx2 = planeBox.mMax.x - playerBox.mMin.x;
            float dy1 = planeBox.mMin.y - playerBox.mMax.y;
            float dy2 = planeBox.mMax.y - playerBox.mMin.y;
            float dz1 = planeBox.mMin.z - playerBox.mMax.z;
            float dz2 = planeBox.mMax.z - playerBox.mMin.z;

            // dx1 / dx2 ���� ������ ���� ���� ���� dx�� ����
            float dx = (Math::Abs(dx1) < Math::Abs(dx2)) ? dx1 : dx2;
            // ���� ������ �Ȱ��� dy�� ����
            float dy = (Math::Abs(dy1) < Math::Abs(dy2)) ? dy1 : dy2;
            // ���� ������ �Ȱ��� dz�� ����
            float dz = (Math::Abs(dz1) < Math::Abs(dz2)) ? dz1 : dz2;

            // �ּ� ��ħ�� ���� ���� ������ x/y/z ��ġ�� �����Ѵ�
            // dx�� ���� ���� ���
            if (Math::Abs(dx) <= Math::Abs(dy) &&
                Math::Abs(dx) <= Math::Abs(dz))
            {
                pos.x += dx;
            }
            // dy�� ���� ���� ���
            else if (Math::Abs(dy) <= Math::Abs(dx) &&
                     Math::Abs(dy) <= Math::Abs(dz))
            {
                pos.y += dy;
            }
            else
            {
                pos.z += dz;
            }

            // ����� ��ġ�� �����ϰ� �ڽ� ������Ʈ�� �����Ѵ�
            SetPosition(pos);
            mBoxComp->OnUpdateWorldTransform();
        }
    }
}