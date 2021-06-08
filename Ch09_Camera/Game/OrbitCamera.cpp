#include "OrbitCamera.h"
#include "Actor.h"

OrbitCamera::OrbitCamera(Actor* owner)
    :CameraComponent(owner)
    , mOffset(-400.0f, 0.0f, 0.0f)
    , mUp(Vector3::UnitZ)
    , mPitchSpeed(0.0f)
    , mYawSpeed(0.0f)
{
}

void OrbitCamera::Update(float deltaTime)
{
    CameraComponent::Update(deltaTime);
    // 게임 세계의 상향 벡터와 요를 사용한 쿼터니언 생성
    Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
    // 오프셋과 상향 벡터를 요 쿼터니언을 사용해서 변환
    mOffset = Vector3::Transform(mOffset, yaw);
    mUp = Vector3::Transform(mUp, yaw);

    // 위의 벡터로부터 카메라의 전방/오른 축 벡터를 계산
    Vector3 forward = -1.0f * mOffset;
    forward.Normalize();
    Vector3 right = Vector3::Cross(mUp, forward);
    right.Normalize();

    // 카메라 오른 축 벡터를 기준으로 회전하는 피치 쿼터니언 생성
    Quaternion pitch(right, mPitchSpeed * deltaTime);
    // 카메라 오프셋과 상향 벡터(카메라의 상향 벡터)를 피치 쿼터니언으로 회전시킴
    mOffset = Vector3::Transform(mOffset, pitch);
    mUp = Vector3::Transform(mUp, pitch);

    // 변환 행렬을 계산
    Vector3 target = mOwner->GetPosition();
    Vector3 cameraPos = target + mOffset;
    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
    SetViewMatrix(view);
}