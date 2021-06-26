#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>

class Skeleton
{
public:
    struct Bone
    {
        BoneTransform mLocalBindPose;
        std::string mName;
        int mParent;
    };

private:
    std::vector<Bone> mBones;
};