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
    // ī�޶��� ��ġ�� ������ ������ ��ġ�� ����
    Vector3 cameraPos = mOwner->GetPosition();

    // ��ġ �ӵ��� ��ġ�� ����
    mPitch += mPitchSpeed * deltaTime;
    // ��ġ ���� [-max, +max] ������ ����
    mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);

    // �������� ���� �� ���Ϳ� ����
    // ��ġ ȸ���� ��Ÿ���� ���ʹϾ����� ǥ��
    Quaternion q(mOwner->GetRight(), mPitch);

    // ��ġ ���ʹϾ����� �������� ���� ���͸� ȸ��
    Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
    // Ÿ���� �������� 100 ���� �տ� �ִٰ� ����
    Vector3 target = cameraPos + viewForward * 100.0f;
    // ��ġ ���ʹϾ����� �����൵ ȸ�� ��Ų��.
    Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

    // look at ����� ������ �� �� ��ķ� ����
    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
    SetViewMatrix(view);
}