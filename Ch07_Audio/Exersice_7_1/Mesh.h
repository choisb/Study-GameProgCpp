#pragma once
#include <vector>
#include <string>

class Mesh
{
public:
    Mesh();
    ~Mesh();
    // 메시 로드/언로드
    // renderer 포인터를 받는 이유는 renderer가 게임에 로드된 텍스처를 모두 갖고 있고, Mesh클래스가 메시와 관련된 모든 텍스처에 접근할 수 있도록 하기 위함.
    bool Load(const std::string& fileName, class Renderer* renderer);
    void Unload();
    // 이 메시에 해당하는 버텍스 배열을 얻는다.
    class VertexArray* GetVertexArray() { return mVertexArray; }
    // 특정 인덱스에 해당하는 텍스처를 얻는다.
    class Texture* GetTexture(size_t index);
    // 셰이더의 이름을 얻는다.
    const std::string& GetShaderName() const { return mShaderName; }
    // 오브젝트 공간 바운딩 구체 반지름을 얻는다.
    float GetRadius() const { return mRadius; }
    // 정반사 지수 반환
    float GetSpecPower() const { return mSpecPower; }

private:

    // 이 메시에 사용되는 텍스처들
    std::vector<class Texture*> mTextures;
    // 메시의 버텍스 배열
    class VertexArray* mVertexArray;
    // 메시가 지정한 셰이더 이름
    std::string mShaderName;
    // 오브젝트 공간 바운딩 구체의 반지름값 저장. 충돌 검사에서 활용할 수 있다.
    // 원점에서 가장 멀리 떨어진 점까지의 거리ㄹ를 바운딩 구체의 반지름으로 사용.
    float mRadius;
    // 정반사 지수
    float mSpecPower;
};