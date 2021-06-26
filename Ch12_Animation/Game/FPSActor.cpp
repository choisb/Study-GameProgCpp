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

    // 액터 위치에 대한 FPS 모델의 상대적인 위치 갱신
    const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
    Vector3 modelPos = GetPosition();
    modelPos += GetForward() * modelOffset.x;
    modelPos += GetRight() * modelOffset.y;
    modelPos.z += modelOffset.z;
    mFPSModel->SetPosition(modelPos);

    // 액터 회전값으로 회전값을 초기화
    Quaternion q = GetRotation();

    // 카메라 피치 값으로 회전
    q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
    mFPSModel->SetRotation(q);

}

void FPSActor::ActorInput(const uint8_t* keys)
{
    float forwardSpeed = 0.0f;
    float strafeSpeed = 0.0f;

    //w,a,s,d 이동
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

    // 마우스 이동
    // SDL로 부터 상대적인 이동을 얻는다.
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    // 마우스 이동은 -500에서 500 사이의 값이라고 추정
    const int maxMouseSpeed = 500;
    // 초당 회전의 최대 속도
    const float maxAngularSpeed = Math::Pi * 8;
    float angularSpeed = 0.0f;
    if (x != 0)
    {
        // [-1.0, 1.0] 범위로 반환
        angularSpeed = static_cast<float>(x) / maxMouseSpeed;
        // 초당 회전을 곱한다.
        angularSpeed *= maxAngularSpeed;
    }
    mMoveComp->SetAngularSpeed(angularSpeed);

    // 피치 계산
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
    // 박스를 갱신하기 전에 자신의 세계 변환 재계산이 필요
    ComputeWorldTransform();

    const AABB& playerBox = mBoxComp->GetWorldBox();
    Vector3 pos = GetPosition();

    auto& planes = GetGame()->GetPlanes();
    for (auto pa : planes)
    {
        // 이 planeActor와 충돌하는가?
        const AABB& planeBox = pa->GetBox()->GetWorldBox();
        if (Intersect(playerBox, planeBox))
        {
            // 각 축으로 겹침값을 계산
            float dx1 = planeBox.mMin.x - playerBox.mMax.x;
            float dx2 = planeBox.mMax.x - playerBox.mMin.x;
            float dy1 = planeBox.mMin.y - playerBox.mMax.y;
            float dy2 = planeBox.mMax.y - playerBox.mMin.y;
            float dz1 = planeBox.mMin.z - playerBox.mMax.z;
            float dz2 = planeBox.mMax.z - playerBox.mMin.z;

            // dx1 / dx2 값중 절댓값이 가장 작은 값을 dx로 설정
            float dx = (Math::Abs(dx1) < Math::Abs(dx2)) ? dx1 : dx2;
            // 위의 로직과 똑같이 dy를 구함
            float dy = (Math::Abs(dy1) < Math::Abs(dy2)) ? dy1 : dy2;
            // 위의 로직과 똑같이 dz를 구함
            float dz = (Math::Abs(dz1) < Math::Abs(dz2)) ? dz1 : dz2;

            // 최소 겹침이 가장 작은 값으로 x/y/z 위치를 보정한다
            // dx가 가장 작을 경우
            if (Math::Abs(dx) <= Math::Abs(dy) &&
                Math::Abs(dx) <= Math::Abs(dz))
            {
                pos.x += dx;
            }
            // dy가 가장 작을 경우
            else if (Math::Abs(dy) <= Math::Abs(dx) &&
                     Math::Abs(dy) <= Math::Abs(dz))
            {
                pos.y += dy;
            }
            else
            {
                pos.z += dz;
            }

            // 재계산된 위치를 저장하고 박스 컴포넌트를 갱신한다
            SetPosition(pos);
            mBoxComp->OnUpdateWorldTransform();
        }
    }
}