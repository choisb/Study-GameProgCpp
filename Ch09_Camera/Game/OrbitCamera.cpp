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
    // ���� ������ ���� ���Ϳ� �並 ����� ���ʹϾ� ����
    Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
    // �����°� ���� ���͸� �� ���ʹϾ��� ����ؼ� ��ȯ
    mOffset = Vector3::Transform(mOffset, yaw);
    mUp = Vector3::Transform(mUp, yaw);

    // ���� ���ͷκ��� ī�޶��� ����/���� �� ���͸� ���
    Vector3 forward = -1.0f * mOffset;
    forward.Normalize();
    Vector3 right = Vector3::Cross(mUp, forward);
    right.Normalize();

    // ī�޶� ���� �� ���͸� �������� ȸ���ϴ� ��ġ ���ʹϾ� ����
    Quaternion pitch(right, mPitchSpeed * deltaTime);
    // ī�޶� �����°� ���� ����(ī�޶��� ���� ����)�� ��ġ ���ʹϾ����� ȸ����Ŵ
    mOffset = Vector3::Transform(mOffset, pitch);
    mUp = Vector3::Transform(mUp, pitch);

    // ��ȯ ����� ���
    Vector3 target = mOwner->GetPosition();
    Vector3 cameraPos = target + mOffset;
    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
    SetViewMatrix(view);
}