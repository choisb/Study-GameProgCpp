#include "CameraActor.h"
#include "MoveComponent.h"
#include "SDL/SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"
#include "AudioComponent.h"
#include "AudioSystem.h"

CameraActor::CameraActor(Game* game)
    :Actor(game)
{
    mMoveComp = new MoveComponent(this);
    mAudioComp = new AudioComponent(this);
    mLastFootstep = 0.0f;
    mFootstep = mAudioComp->PlayEvent("event:/Footstep");
    mFootstep.SetPaused(true);
}
void CameraActor::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);

    // footstep 효과음 재생
    mLastFootstep -= deltaTime;
    if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f)
    {
        mFootstep.SetPaused(false);
        mFootstep.Restart();
        mLastFootstep = 0.5f;
    }

    // 카메라의 변화된 위치나 회전에 따라 view 변환 행렬을 새롭게 계산
    Vector3 cameraPos = GetPosition();
    Vector3 target = GetPosition() + GetForward() * 100.f;
    Vector3 up = Vector3::UnitZ;

    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
    GetGame()->GetRenderer()->SetViewMatrix(view);
    // audioSystem에서 리스너 설정 갱신
    GetGame()->GetAudioSystem()->SetListener(view, mForwardSpeed * GetForward());
}

void CameraActor::ActorInput(const uint8_t* keys)
{
    float angularSpeed = 0.0f;
    mForwardSpeed = 0.f;

    // wasd movement
    if (keys[SDL_SCANCODE_W])
    {
        mForwardSpeed += 300.0f;
    }
    if (keys[SDL_SCANCODE_S])
    {
        mForwardSpeed -= 300.0f;
    }
    if (keys[SDL_SCANCODE_A])
    {
        angularSpeed -= Math::TwoPi;
    }
    if (keys[SDL_SCANCODE_D])
    {
        angularSpeed += Math::TwoPi;
    }

    mMoveComp->SetForwardSpeed(mForwardSpeed);
    mMoveComp->SetAngularSpeed(angularSpeed);
}

void CameraActor::SetFootstepSurface(float value)
{
    // "event:/Footstep" 의 이벤트 설정 상 정지 후 파라미터 변경만으로 다시 재생이됨. 
    mFootstep.SetPaused(true);
    mFootstep.SetParameter("Surface", value);
}