#include "SplineCamera.h"
#include "Actor.h"

Vector3 Spline::Compute(size_t startIdx, float t) const
{
    // startIdx�� ��谪�� �ʰ��ϴ����� �˻�
    if (startIdx >= mControlPoints.size())
    {
        return mControlPoints.back();
    }
    else if (startIdx == 0)
    {
        return mControlPoints[startIdx];
    }
    else if (startIdx + 2 >= mControlPoints.size())
    {
        return mControlPoints[startIdx];
    }

    // p0���� p3 ���� ���� ��´�
    Vector3 p0 = mControlPoints[startIdx - 1];
    Vector3 p1 = mControlPoints[startIdx];
    Vector3 p2 = mControlPoints[startIdx + 1];
    Vector3 p3 = mControlPoints[startIdx + 2];

    // Ĺ�ַ� ���������� ��ġ�� ���
    Vector3 position = 0.5f * ((2.0f * p1) + (-1.0f * p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
        (-1.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
    return position;
}

SplineCamera::SplineCamera(Actor* owner)
    :CameraComponent(owner)
    ,mIndex(1)
    ,mT(0.0f)
    ,mSpeed(0.5f)
    ,mPaused(true)
{
}

void SplineCamera::Update(float deltaTime)
{
    CameraComponent::Update(deltaTime);
    // t�� ����
    if (!mPaused)
    {
        mT += mSpeed * deltaTime;
        // T���� 1.0���� ũ�ٸ� ���� ���������� �̵�
        // ī�޶� �̵� �ӵ��� �� �����ӿ� �������� �������� �ǳʶ� ��ŭ ������ �ʴٰ� ����
        if (mT >= 1.0f)
        {
            // ��θ� �����ϴ� �� �־� ����� ���� ������ �Ѵ�
            if (mIndex < mPath.GetNumPoints() - 3)
            {
                mIndex++;
                mT = mT - 1.0f;
            }
            else
            {
                // ��� ������ �Ϸ������Ƿ� ī�޶� �̵��� ������Ų��.
                mPaused = true;
            }
        }
    }

    // ī�޶� ��ġ�� ���� t/�ε������� �ش��ϴ� ���ö��ο� �ִ�.
    Vector3 cameraPos = mPath.Compute(mIndex, mT);
    // ��ǥ ������ t���� ���� ��Ÿ���� ���ؼ� ���� ��ġ��.
    Vector3 target = mPath.Compute(mIndex, mT + 0.01f);
    // ���ö��� ī�޶�� �Ųٷ� ���������� �ʴ´ٰ� ����
    const Vector3 up = Vector3::UnitZ;
    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
    SetViewMatrix(view);
}

void SplineCamera::Restart()
{
    mIndex = 1;
    mT = 0.0f;
    mPaused = false;
}