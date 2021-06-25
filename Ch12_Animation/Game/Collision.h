#pragma once
#include "Math.h"
#include <vector>

struct LineSegment
{
    // 선분은 항상 시작점과 끝점이 있어야한다.
    LineSegment(const Vector3& start, const Vector3& end);
    // t값이 주어졌을 때 선분상의 한 점을 반환하는 함수
    Vector3 PointOnSegment(float t) const;
    // 선분과 임의의 점 사이의 최단거리의 제곱값
    float MinDistSq(const Vector3& point) const;
    static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

    // 선분의 시작과 끝
    Vector3 mStart;
    Vector3 mEnd;
};

struct Plane
{
    // 세 점으로 평명 생성
    Plane(const Vector3& a, const Vector3& b, const Vector3& c);
    // 법선벡터와 원점으로부터의 거리로 평명 생성
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
    // 선분에서 0 <= t <= 1 사이에 해당하는 점을 반환
    Vector3 PointOnSegment(float t) const;
    bool Contains(const Vector3& point) const;

    LineSegment mSegment;
    float mRadius;
};

struct ConvexPolygon
{
    bool Contains(const Vector2& point) const;
    // 버텍스는 시계 방향으로 정렬돼 있어야 한다.
    std::vector<Vector2> mVertices;
};

// 교차 테스트 함수
bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const Sphere& s, const AABB& box);
bool Intersect(const Capsule& a, const Capsule& b);

bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT, Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1, 
    const Sphere& Q0, const Sphere& Q1, float& outT);