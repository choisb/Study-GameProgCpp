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
    // 파일로부터 로드
    bool Load(const std::string& fileName);

    // Get 함수들
    size_t GetNumBones() const { return mBones.size(); }
    const Bone& GetBone(size_t idx) const { return mBones[idx]; }
    const std::vector<Bone>& GetBones() const { return mBones; }
    const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }

protected:
    // 각 본의 전역 인버스 바인드 포즈 계산
    // (뼈대를 로드할 때 호출됨)
    void ComputeGlobalInvBindPose();
private:
    // 뼈대를 구성하는 본
    std::vector<Bone> mBones;
    // 각 본의 전역 인버스 바인드 포즈
    std::vector<Matrix4> mGlobalInvBindPoses;
};