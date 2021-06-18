#include "Collision.h"
#include <array>
LineSegment::LineSegment(const Vector3& start, const Vector3& end)
    :mStart(start)
    , mEnd(end)
{
}

Vector3 LineSegment::PointOnSegment(float t) const
{
    return mStart + (mEnd - mStart) * t;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
    // 벡터 선언
    Vector3 ab = mEnd - mStart;
    Vector3 ba = -1.0f * ab;
    Vector3 ac = point - mStart;
    Vector3 bc = point - mEnd;

    // 케이스 1: C와 A의 거리를 구하는 경우
    if (Vector3::Dot(ab, ac) < 0.0f)
    {
        return ac.LengthSq();
    }

    // 케이스 2: C와 B의 거리를 구하는 경우
    else if (Vector3::Dot(ba, bc) < 0.0f)
    {
        return bc.LengthSq();
    }

    // 케이스 3: C를 선분에 투영해서 최소 거리를 구하는 경우
    else
    {
        // p를 계산
        float scalar = Vector3::Dot(ac, bc) / Vector3::Dot(ab, ab);
        Vector3 p = scalar * ab;
        // ac - p 길이 제곱값 계산
        return (ac - p).LengthSq();
    }
}


float LineSegment::MinDistSq(const LineSegment & s1, const LineSegment & s2)
{
    Vector3   u = s1.mEnd - s1.mStart;
    Vector3   v = s2.mEnd - s2.mStart;
    Vector3   w = s1.mStart - s2.mStart;
    float    a = Vector3::Dot(u, u);         // always >= 0
    float    b = Vector3::Dot(u, v);
    float    c = Vector3::Dot(v, v);         // always >= 0
    float    d = Vector3::Dot(u, w);
    float    e = Vector3::Dot(v, w);
    float    D = a * c - b * b;        // always >= 0
    float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

                                   // compute the line parameters of the two closest points
    if (Math::NearZero(D)) { // the lines are almost parallel
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b*e - c * d);
        tN = (a*e - b * d);
        if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (Math::NearZero(sN) ? 0.0f : sN / sD);
    tc = (Math::NearZero(tN) ? 0.0f : tN / tD);

    // get the difference of the two closest points
    Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

    return dP.LengthSq();   // return the closest distance squared
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
    // a에서 b, a에서 c로의 벡터를 계산
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    // 법선 벡터를 얻기 위해 외적을 한 뒤 정규화
    mNormal = Vector3::Cross(ab, ac);
    mNormal.Normalize();
    // d = -p dot n
    mD = -Vector3::Dot(a, mNormal);
}
Plane::Plane(const Vector3& normal, float d)
    :mNormal(normal)
    ,mD(d)
{
}

float Plane::SignedDist(const Vector3& point) const
{
    return Vector3::Dot(point, mNormal) - mD;
}
Sphere::Sphere(const Vector3& center, float radius)
    :mCenter(center)
    , mRadius(radius)
{

}
bool Sphere::Contains(const Vector3& point) const
{
    // 구의 중심과 점 사이의 거리 제곱값 얻기
    float distSq = (mCenter - point).LengthSq();
    return distSq <= (mRadius * mRadius);
}

AABB::AABB(const Vector3& min, const Vector3& max)
    : mMin(min)
    , mMax(max)
{
}

void AABB::UpdateMinMax(const Vector3& point)
{
    // 각 요솟값을 별도로 갱신
    mMin.x = Math::Min(mMin.x, point.x);
    mMin.y = Math::Min(mMin.y, point.y);
    mMin.z = Math::Min(mMin.z, point.z);

    mMax.x = Math::Max(mMax.x, point.x);
    mMax.y = Math::Max(mMax.y, point.y);
    mMax.z = Math::Max(mMax.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
    // 박스의 구석 8개 점을 생성
    std::array<Vector3, 8> points;
    // Min 점은 항상 첫번째 코너
    points[0] = mMin;
    // Min점과 Max점 요소 조합으로 나머지 코너 구성
    points[1] = Vector3(mMax.x, mMin.y, mMin.z);
    points[2] = Vector3(mMin.x, mMax.y, mMin.z);
    points[3] = Vector3(mMin.x, mMin.y, mMax.z);

    points[4] = Vector3(mMin.x, mMax.y, mMax.z);
    points[5] = Vector3(mMax.x, mMin.y, mMax.z);
    points[6] = Vector3(mMax.x, mMax.y, mMin.z);

    // Max 점은 항상 마지막 코너
    points[7] = Vector3(mMax);

    // 최초 점을 쿼터니언으로 회전시킴
    Vector3 p = Vector3::Transform(points[0], q);
    // 회전된 최초의 점으로 min/max 리셋
    mMin = p;
    mMax = p;
    // 남아 있는 점들을 회전시킨 뒤 min/max 갱신
    for (size_t i = 1; i < points.size(); i++)
    {
        p = Vector3::Transform(points[i], q);
        UpdateMinMax(p);
    }
}
bool AABB::Contains(const Vector3& point) const
{
    bool outside = point.x < mMin.x ||
        point.y < mMin.y ||
        point.z < mMin.z ||
        point.x > mMax.x ||
        point.y > mMax.y ||
        point.z > mMax.z;
    // 하나라도 true 라면 점은 밖에 있다.
    return !outside;
}
float AABB::MinDistSq(const Vector3& point) const
{
    // 각각의 축과 점 사이의 거리를 계산한다.
    float dx = Math::Max(mMin.x - point.x, 0.0f);
    dx = Math::Max(dx, point.x - mMax.x);
    float dy = Math::Max(mMin.y - point.y, 0.0f);
    dy = Math::Max(dy, point.y - mMax.y);
    float dz = Math::Max(mMin.z - point.z, 0.0f);
    dz = Math::Max(dz, point.z - mMax.z);

    // 거리의 제곱 식
    return dx * dx + dy * dy + dz * dz;
}
Capsule::Capsule(const Vector3& start, const Vector3& end, float radius)
    :mSegment(start, end)
    , mRadius(radius)
{

}
Vector3 Capsule::PointOnSegment(float t) const
{
    return mSegment.PointOnSegment(t);
}
bool Capsule::Contains(const Vector3& point) const
{
    // 점과 선분 사이의 최소 거리 제곱값 구하기
    float distSq = mSegment.MinDistSq(point);
    return distSq <= (mRadius * mRadius);
}
bool ConvexPolygon::Contains(const Vector2& point) const
{
    float sum = 0.0f;
    Vector2 a, b;
    for (size_t i = 0; i < mVertices.size() - 1; i++)
    {
        // 점과 첫 번째 버텍스로 벡터 구성
        a = mVertices[i] - point;
        a.Normalize();

        // 점과 두 번째 버텍스로 벡터 구성
        b = mVertices[i + 1] - point;
        b.Normalize();
        // sum에 구한 각을 더한다.
        sum += Math::Acos(Vector2::Dot(a, b));
    }
    // 마지막 버텍스와 처음 버텍스가 이루는 각을 구한다.
    a = mVertices.back() - point;
    a.Normalize();

    b = mVertices.front() - point;
    b.Normalize();
    sum += Math::Acos(Vector2::Dot(a, b));
    // 값이 대략적으로 2pi에 가까우면 true 리턴
    return Math::NearZero(sum - Math::TwoPi);
}

bool Intersect(const Sphere& a, const Sphere& b)
{
    float distSq = (a.mCenter - b.mCenter).LengthSq();
    float sumRadii = a.mRadius + b.mRadius;
    return distSq <= (sumRadii * sumRadii);
}
bool Intersect(const AABB& a, const AABB& b)
{
    bool no = a.mMax.x < b.mMin.x ||
        a.mMax.y < b.mMin.y ||
        a.mMax.z < b.mMin.z ||
        b.mMax.x < a.mMin.x ||
        b.mMax.y < a.mMin.y ||
        b.mMax.z < a.mMin.z;
    // 모두 false라면 교차하는 것
    // 하나라도 true라면 두 물체는 교차하지 않는다.
    return !no;
}
bool Intersect(const Sphere& s, const AABB& box)
{
    float distSq = box.MinDistSq(s.mCenter);
    return distSq <= (s.mRadius * s.mRadius);
}
bool Intersect(const Capsule& a, const Capsule& b)
{
    float distSq = LineSegment::MinDistSq(a.mSegment, b.mSegment);
    float sumRadii = a.mRadius + b.mRadius;
    return distSq <= (sumRadii * sumRadii);
}
bool Intersect(const LineSegment& l, const Plane& p, float& outT)
{
    // 분모값을 확인해서 t의 해가 존재하는지 확인
    float denom = Vector3::Dot(l.mEnd - l.mStart, p.mNormal);
    if (Math::NearZero(denom)) // 분모가 0에 가깝다면
    {
        // 선과 평면이 교차하는 유일한 길은
        // 두 점이 모두 평면상의 점인 경우. 즉 (P dot N) == d 평면의 방벙식을 만족해야 한다.
        if (Math::NearZero(Vector3::Dot(l.mStart, p.mNormal) - p.mD))
        {
            outT = 0.0f;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        float numer = -Vector3::Dot(l.mStart, p.mNormal) - p.mD;
        outT = numer / denom;
        // t가 선분 범위 내의 값인지 검증
        if (outT >= 0.0f && outT <= 1.0f)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
bool Intersect(const LineSegment& l, const Sphere& s, float& outT)
{
    // 방정식에 등장한 X, Y, a, b, c를 계산
    Vector3 X = l.mStart - s.mCenter;
    Vector3 Y = l.mEnd - l.mStart;
    float a = Vector3::Dot(Y, Y);
    float b = 2.0f * Vector3::Dot(X, Y);
    float c = Vector3::Dot(X, X) - s.mRadius * s.mRadius;
    // 판별식을 계산
    float disc = b * b - 4.0f * a * c;
    if (disc < 0.0f)
    {
        return false;
    }
    else
    {
        disc = Math::Sqrt(disc);
        // t의 min / max 해를 계산
        float tMin = (-b - disc) / (2.0f * a);
        float tMax = (-b + disc) / (2.0f * a);
        // t의 값이 선분 범위 내에 있는지 검사
        if (tMin >= 0.0f && tMin <= 1.0f)
        {
            outT = tMin;
            return true;
        }
        else if (tMax >= 0.0f && tMax <= 1.0f)
        {
            outT = tMax;
            return true;
        }
        else
        {
            return false;
        }
    }
}
bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
    std::vector<std::pair<float, Vector3>>& out)
{
    float denom = end - start;
    if (Math::NearZero(denom))
    {
        return false;
    }
    else
    {
        float numer = -start + negd;
        float t = numer / denom;
        // Test that t is within bounds
        if (t >= 0.0f && t <= 1.0f)
        {
            out.emplace_back(t, norm);
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT,
    Vector3& outNorm)
{
    // Vector to save all possible t values, and normals for those sides
    std::vector<std::pair<float, Vector3>> tValues;
    // Test the x planes
    TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, Vector3::NegUnitX,
        tValues);
    TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, Vector3::UnitX,
        tValues);
    // Test the y planes
    TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, Vector3::NegUnitY,
        tValues);
    TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, Vector3::UnitY,
        tValues);
    // Test the z planes
    TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, Vector3::NegUnitZ,
        tValues);
    TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, Vector3::UnitZ,
        tValues);

    // Sort the t values in ascending order
    std::sort(tValues.begin(), tValues.end(), [](
        const std::pair<float, Vector3>& a,
        const std::pair<float, Vector3>& b) {
            return a.first < b.first;
        });
    // Test if the box contains any of these points of intersection
    Vector3 point;
    for (auto& t : tValues)
    {
        point = l.PointOnSegment(t.first);
        if (b.Contains(point))
        {
            outT = t.first;
            outNorm = t.second;
            return true;
        }
    }

    //None of the intersections are within bounds of box
    return false;
}


bool SweptSphere(const Sphere& P0, const Sphere& P1, 
    const Sphere& Q0, const Sphere& Q1, float& outT)
{
    // X, Y, a, b, c를 계산
    Vector3 X = P0.mCenter - Q0.mCenter;
    Vector3 Y = P1.mCenter - P0.mCenter - (Q1.mCenter - Q0.mCenter);
    float a = Vector3::Dot(Y, Y);
    float b = 2.0f * Vector3::Dot(X, Y);
    float sumRadii = P0.mRadius + Q0.mRadius;
    float c = Vector3::Dot(X, X) - sumRadii * sumRadii;
    // 판별식을 계산한다.
    float disc = b * b - 4.0f * a * c;
    if (disc < 0.0f)
    {
        return false;
    }
    else
    {
        disc = Math::Sqrt(disc);
        // 최초 충돌만이 의미가 있으므로 2개의 해 중 작은 해만 다룬다
        outT = (-b - disc) / (2.0f * a);
        if (outT >= 0.0f && outT <= 0.0f)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}