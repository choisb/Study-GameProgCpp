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

    // ������ ��������κ��� ���� ���ڰ� ���
    float dampening = 2.0f * Math::Sqrt(mSpringConstant);

    // �̻����� ��ġ ���
    Vector3 idealPos = ComputeCameraPos();
    // �̻����� ��ġ�� ���� ��ġ�� ���� ���
    Vector3 diff = mActualPos - idealPos;
    // �������� ���ӵ��� ����Ѵ�.
    Vector3 acel = - mSpringConstant * diff - dampening * mVelocity;

    // �ӵ��� ����
    mVelocity += acel * deltaTime;
    // ���� ī�޶��� ��ġ�� ����
    mActualPos += mVelocity * deltaTime;

    // Ÿ���� ������ �տ� �ִ� ����� �ǹ�
    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    // ī�޶� ���������� ���� ���⿡ ���� ������ �׻� UnitZ�̴�
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

void FollowCamera::SnapToIdeal()
{
    // ���� ��ġ�� �̻����� ��ġ�� ����
    mActualPos = ComputeCameraPos();
    // �ӵ��� 0���� �ʱ�ȭ
    mVelocity = Vector3::Zero;
    // Ÿ�� ������ �� ����� ���
    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
    // �������� ���� �� ���ʿ� ī�޶� ��ġ ����
    Vector3 cameraPos = mOwner->GetPosition();
    cameraPos -= mOwner->GetForward() * mHorzDist;
    cameraPos += Vector3::UnitZ * mVertDist;
    return cameraPos;
}