#include "Animation.h"
#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>


bool Animation::Load(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("File not found: Animation %s", fileName.c_str());
        return false;
    }

    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string contents = fileStream.str();
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);

    if (!doc.IsObject())
    {
        SDL_Log("Animation %s is not valid json", fileName.c_str());
        return false;
    }

    int ver = doc["version"].GetInt();

    // Check the metadata
    if (ver != 1)
    {
        SDL_Log("Animation %s unknown format", fileName.c_str());
        return false;
    }

    const rapidjson::Value& sequence = doc["sequence"];
    if (!sequence.IsObject())
    {
        SDL_Log("Animation %s doesn't have a sequence.", fileName.c_str());
        return false;
    }

    const rapidjson::Value& frames = sequence["frames"];
    const rapidjson::Value& length = sequence["length"];
    const rapidjson::Value& bonecount = sequence["bonecount"];

    if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint())
    {
        SDL_Log("Sequence %s has invalid frames, length, or bone count.", fileName.c_str());
        return false;
    }

    mNumFrames = frames.GetUint();
    mDuration = length.GetFloat();
    mNumBones = bonecount.GetUint();
    mFrameDuration = mDuration / (mNumFrames - 1);

    mTracks.resize(mNumBones);

    const rapidjson::Value& tracks = sequence["tracks"];

    if (!tracks.IsArray())
    {
        SDL_Log("Sequence %s missing a tracks array.", fileName.c_str());
        return false;
    }

    for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
    {
        if (!tracks[i].IsObject())
        {
            SDL_Log("Animation %s: Track element %d is invalid.", fileName.c_str(), i);
            return false;
        }

        size_t boneIndex = tracks[i]["bone"].GetUint();

        const rapidjson::Value& transforms = tracks[i]["transforms"];
        if (!transforms.IsArray())
        {
            SDL_Log("Animation %s: Track element %d is missing transforms.", fileName.c_str(), i);
            return false;
        }

        BoneTransform temp;

        if (transforms.Size() < mNumFrames)
        {
            SDL_Log("Animation %s: Track element %d has fewer frames than expected.", fileName.c_str(), i);
            return false;
        }

        for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
        {
            const rapidjson::Value& rot = transforms[j]["rot"];
            const rapidjson::Value& trans = transforms[j]["trans"];

            if (!rot.IsArray() || !trans.IsArray())
            {
                SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
                return false;
            }

            temp.mRotation.x = rot[0].GetFloat();
            temp.mRotation.y = rot[1].GetFloat();
            temp.mRotation.z = rot[2].GetFloat();
            temp.mRotation.w = rot[3].GetFloat();

            temp.mTranslation.x = trans[0].GetFloat();
            temp.mTranslation.y = trans[1].GetFloat();
            temp.mTranslation.z = trans[2].GetFloat();

            mTracks[boneIndex].emplace_back(temp);
        }
    }

    return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses,
    const Skeleton* inSkeleton, float inTime) const
{
    // outPoses 벡터 크기 조정
    if (outPoses.size() != mNumBones)
    {
        outPoses.resize(mNumBones);
    }

    // 현재 프레임 인덱스와 다음 프레임을 알아낸다
    // (inTime이 [0, AnimDuration] 경계값 내부에 있다고 추정
    size_t frame = static_cast<size_t> (inTime / mFrameDuration);
    size_t nextFrame = frame + 1;
    // 프레임과 다음 프레임 사이의 분숫값 계산
    float pct = inTime / mFrameDuration - frame;

    // 루트 본의 포즈 셋업
    if (mTracks[0].size() > 0)
    {
        // 현재 프레임의 포즈와 다음 프레임 포즈를 보간
        BoneTransform interp = BoneTransform::Interpolate(mTracks[0][frame],
            mTracks[0][nextFrame], pct);
        outPoses[0] = interp.ToMatrix();
    }
    else
    {
        outPoses[0] = Matrix4::Identity;
    }
    const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
    // 루트를 제외한 나머지 본의 포즈를 설정한다.
    for (size_t bone = 1; bone < mNumBones; bone++)
    {
        Matrix4 localMat;   // (항등 행렬로 초기화됨)
        if (mTracks[bone].size() > 0)
        {
            BoneTransform interp = BoneTransform::Interpolate(
                mTracks[bone][frame], mTracks[bone][nextFrame], pct);
            localMat = interp.ToMatrix();
        }
        outPoses[bone] = localMat * outPoses[bones[bone].mParent];
    }
}