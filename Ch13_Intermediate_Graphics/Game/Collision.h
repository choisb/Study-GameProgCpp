#pragma once
#include "Math.h"
#include <vector>

struct LineSegment
{
    // ������ �׻� �������� ������ �־���Ѵ�.
    LineSegment(const Vector3& start, const Vector3& end);
    // t���� �־����� �� ���л��� �� ���� ��ȯ�ϴ� �Լ�
    Vector3 PointOnSegment(float t) const;
    // ���а� ������ �� ������ �ִܰŸ��� ������
    float MinDistSq(const Vector3& point) const;
    static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

    // ������ ���۰� ��
    Vector3 mStart;
    Vector3 mEnd;
};

struct Plane
{
    // �� ������ ��� ����
    Plane(const Vector3& a, const Vector3& b, const Vector3& c);
    // �������Ϳ� �������κ����� �Ÿ��� ��� ����
    Plane(const Vector3& normal, float d);
    float SignedDist(const Vector3& point) const;

    Vector3 mNormal;
    float mD;
};

struct Sphere
{
    Sphere(const Vector3& center, float radius);
    bool Contains(const Vector3& point) const;

    Vector3 mCenter;
    float mRadius;
};

struct AABB
{
    AABB(const Vector3& min, const Vector3& max);

    void UpdateMinMax(const Vector3& point);

    void Rotate(const Quaternion& q);
    bool Contains(const Vector3& point) const;
    float MinDistSq(const Vector3& point) const;

    Vector3 mMin;
    Vector3 mMax;
};

struct OBB
{
    Vector3 mCenter;
    Quaternion mRotation;
    Vector3 mExtents;
};

struct Capsule
{
    Capsule(const Vector3& start, const Vector3& end, float radius);
    // ���п��� 0 <= t <= 1 ���̿� �ش��ϴ� ���� ��ȯ
    Vector3 PointOnSegment(float t) const;
    bool Contains(const Vector3& point) const;

    LineSegment mSegment;
    float mRadius;
};

struct ConvexPolygon
{
    bool Contains(const Vector2& point) const;
    // ���ؽ��� �ð� �������� ���ĵ� �־�� �Ѵ�.
    std::vector<Vector2> mVertices;
};

// ���� �׽�Ʈ �Լ�
bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const Sphere& s, const AABB& box);
bool Intersect(const Capsule& a, const Capsule& b);

bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT, Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1, 
    const Sphere& Q0, const Sphere& Q1, float& outT);