#pragma once
#include "CameraComponent.h"
#include <vector>

struct Spline
{
    // ���ö��ο� ���� ������
    // ���׸�Ʈ�� n���� ���� �ִٸ� �� n+2���� ���� �ʿ��ϴ�
    std::vector<Vector3> mControlPoints;

    // startIdx = P1�� ���ö��� ���׸�Ʈ�� �־����� ��
    // t���� ���� ��ġ�� ����Ѵ�.
    Vector3 Compute(size_t startIdx, float t) const;

    size_t GetNumPoints() const { return mControlPoints.size(); }
};

class SplineCamera : public CameraComponent
{
public:
    SplineCamera(class Actor* owner);

    void Update(float deltaTime) override;
    // ���ö����� ó������ �ٽ� ����
    void Restart();

    void SetSpeed(float speed) { mSpeed = speed; }
    void SetSpline(const Spline& spline) { mPath = spline; }
    void SetPaused(bool pause) { mPaused = pause; }

private:
    // ī�޶� ���󰡴� ���ö��� ���
    Spline mPath;
    // ���� ������ �ε��� �� t
    size_t mIndex;
    float mT;
    // �ʴ� T�� ��ȭ��
    float mSpeed;
    // ��θ� ���� ī�޶� �̵��ؾ� �ϴ����� ����
    bool mPaused;

};