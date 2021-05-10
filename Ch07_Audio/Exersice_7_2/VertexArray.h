#pragma once
class VertexArray
{
public:
    // 생성자는 버텍스와 인덱스 버퍼 포인터를 인자로 받아서 그 데이터를 OpenGL로 전달한다.
    // 해당 데이터는 그래픽 하드웨어상에 로드된다.
    VertexArray(const float* verts, unsigned int numVerts,
        const unsigned int* indices, unsigned int numIndices);
    ~VertexArray();

    // 이 버텍스 배열을 활성화(활성화를 하면 이 버텍스 배열을 그릴 수 있다.)
    void SetActive();

    unsigned int GetNumIndices() const { return mNumIndices; }
    unsigned int GetNumVerts() const { return mNumVerts; }

private:
    // 버텍스 버퍼에 존재하는 버텍스의 수
    unsigned int mNumVerts;
    // 인덱스 버퍼에 존재하는 인덱스의 수
    unsigned int mNumIndices;
    // 버텍스 버퍼의 OpenGL ID
    unsigned int mVertexBuffer;
    // 인덱스 버퍼의 OpenGL ID
    unsigned int mIndexBuffer;
    // 버텍스 배열 개체 OpenGL ID
    unsigned int mVertexArray;
};