#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
    :CameraComponent(owner)
    , mPitchSpeed(0.0f)
    , mMaxPitch(Math::Pi / 3.0f)
    , mPitch(0.0f)
{

}

void FPSCamera::Update(float deltaTime)
{
    CameraComponent::Update(deltaTime);
    // 카메라의 위치는 소유자 엑터의 위치와 동일
    Vector3 cameraPos = mOwner->GetPosition();

    // 피치 속도로 피치를 갱신
    mPitch += mPitchSpeed * deltaTime;
    // 피치 값을 [-max, +max] 범위로 제한
    mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);

    // 소유자의 오른 축 벡터에 대한
    // 피치 회전을 나타내는 쿼터니언으로 표현
    Quaternion q(mOwner->GetRight(), mPitch);

    // 피치 쿼터니언으로 소유자의 전방 벡터를 회전
    Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
    // 타깃은 소유자의 100 단위 앞에 있다고 가정
    Vector3 target = cameraPos + viewForward * 100.0f;
    // 피치 쿼터니언으로 상향축도 회전 시킨다.
    Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

    // look at 행렬을 생성한 뒤 뷰 행렬로 설정
    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
    SetViewMatrix(view);
}