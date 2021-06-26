#include "FollowCamera.h"
#include "Actor.h"

FollowCamera::FollowCamera(Actor* owner)
    :CameraComponent(owner)
    ,mHorzDist(350.0f)
    ,mVertDist(150.0f)
    ,mTargetDist(100.0f)
    ,mSpringConstant(64.0f)
{
}
void FollowCamera::Update(float deltaTime)
{
    CameraComponent::Update(deltaTime);

    // 스프링 상숫값으로부터 감쇄 인자값 계산
    float dampening = 2.0f * Math::Sqrt(mSpringConstant);

    // 이상적인 위치 계산
    Vector3 idealPos = ComputeCameraPos();
    // 이상적인 위치와 실제 위치의 차를 계산
    Vector3 diff = mActualPos - idealPos;
    // 스프링의 가속도를 계산한다.
    Vector3 acel = - mSpringConstant * diff - dampening * mVelocity;

    // 속도를 갱신
    mVelocity += acel * deltaTime;
    // 실제 카메라의 위치를 갱신
    mActualPos += mVelocity * deltaTime;

    // 타깃은 소유자 앞에 있는 대상을 의미
    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    // 카메라가 뒤집혀지는 경우는 없기에 위쪽 방향은 항상 UnitZ이다
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

void FollowCamera::SnapToIdeal()
{
    // 실제 위치를 이상적인 위치로 설정
    mActualPos = ComputeCameraPos();
    // 속도를 0으로 초기화
    mVelocity = Vector3::Zero;
    // 타깃 지점과 뷰 행렬을 계산
    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
    // 소유자의 뒤쪽 및 위쪽에 카메라 위치 설정
    Vector3 cameraPos = mOwner->GetPosition();
    cameraPos -= mOwner->GetForward() * mHorzDist;
    cameraPos += Vector3::UnitZ * mVertDist;
    return cameraPos;
}