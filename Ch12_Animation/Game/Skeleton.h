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
    // ���Ϸκ��� �ε�
    bool Load(const std::string& fileName);

    // Get �Լ���
    size_t GetNumBones() const { return mBones.size(); }
    const Bone& GetBone(size_t idx) const { return mBones[idx]; }
    const std::vector<Bone>& GetBones() const { return mBones; }
    const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }

protected:
    // �� ���� ���� �ι��� ���ε� ���� ���
    // (���븦 �ε��� �� ȣ���)
    void ComputeGlobalInvBindPose();
private:
    // ���븦 �����ϴ� ��
    std::vector<Bone> mBones;
    // �� ���� ���� �ι��� ���ε� ����
    std::vector<Matrix4> mGlobalInvBindPoses;
};