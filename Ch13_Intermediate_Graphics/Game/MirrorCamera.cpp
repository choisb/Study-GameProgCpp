#include "MirrorCamera.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

MirrorCamera::MirrorCamera(Actor* owner)
    :CameraComponent(owner)
    ,mHorzDist(150.0f)
    ,mVertDist(200.0f)
    ,mTargetDist(400.0f)
{

}

void MirrorCamera::Update(float deltaTime)
{
    CameraComponent::Update(deltaTime);

    Vector3 idealPos = ComputeCameraPos();
    Vector3 target = mOwner->GetPosition() - mOwner->GetForward() * mTargetDist;

    Matrix4 view = Matrix4::CreateLookAt(idealPos, target, Vector3::UnitZ);
    Game* game = mOwner->GetGame();
    game->GetRenderer()->SetMirrorView(view);
}

void MirrorCamera::SnapToIdeal()
{
    Vector3 idealPos = ComputeCameraPos();
    Vector3 target = mOwner->GetPosition() - mOwner->GetForward() * mTargetDist;

    Matrix4 view = Matrix4::CreateLookAt(idealPos, target, Vector3::UnitZ);
    Game * game = mOwner->GetGame();
    game->GetRenderer()->SetMirrorView(view);
}

Vector3 MirrorCamera::ComputeCameraPos() const
{
    Vector3 cameraPos = mOwner->GetPosition();
    cameraPos += mOwner->GetForward() * mHorzDist;
    cameraPos += Vector3::UnitZ * mVertDist;
    return cameraPos;
}