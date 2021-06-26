#pragma once
#include "Math.h"

class BoneTransform
{
    Quaternion mRotation;
    Vector3 mTranslation;

    Matrix4 ToMatrix() const;

    static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};