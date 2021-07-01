#pragma once
#include "BoneTransform.h"
#include <vector>
#include <string>

class Animation
{
public:
    bool Load(const std::string& fileName);

    size_t GetNumBones() const { return mNumBones; }
    size_t GetNumFrames() const { return mNumFrames; }
    float GetDuration() const { return mDuration; }
    float GetFrameDuration() const { return mFrameDuration; }

    // �ִϸ��̼ǿ��� Ư�� �ð��� �� ���� ����
    // ���� ���� ���� ����� �־��� ���Ϳ� ä���
    void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses,
        const class Skeleton* inSkeleton, float inTime) const;
private:
    // �ִϸ��̼��� ���� ���� ��
    size_t mNumBones;
    // �ִϸ��̼��� ������ ��
    size_t mNumFrames;
    // �ִϸ��̼� ��� �ð�(�� ����)
    float mDuration;
    // �ִϸ��̼ǿ��� �� �������� ��� �ð�
    float mFrameDuration;
    // Ʈ���� �� �����Ӻ� ��ȯ ����
    // �ܺ� ������ �ε����� �� �ε����̸�, ���� ������ �ε����� ������ �ε�����
    std::vector<std::vector<BoneTransform>> mTracks;

};