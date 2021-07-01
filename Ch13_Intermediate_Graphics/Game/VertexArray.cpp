#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const void* verts, unsigned int numVerts, 
    Layout layout, const unsigned int* indices, unsigned int numIndices)
    : mNumVerts(numVerts)
    , mNumIndices(numIndices)
{
    // *********** Vertex Array 생성*************************
    // vertex array 생성. mVertexArray에 ID를 저장
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    unsigned vertexSize = 8 * sizeof(float);
    if (layout == PosNormSkinTex)
    {
        vertexSize = 8 * sizeof(float) + 8 * sizeof(char);
    }

    // *********** Vertex Buffer 생성*************************
    // vertex buffer 생성. mVertexBuffer에 ID를 저장
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    // GL_ARRAY_BUFFER 파라미터는 버텍스 버퍼를 버퍼로 사용하겠다는 뜻

    // 버텍스 버퍼를 생성했으니 생성자로 전달된 버텍스 데이터를 이 버텍스 버퍼로 복사한다.
    // 첫번째 인자로 버퍼의 ID가 아닌 GL_ARRAY_BUFFER를 전달했다.
    // GL_ARRAY_BUFFER는 가장 최근에 생성한 버텍스 버퍼를 사용하겠다는 것을 뜻한다.
    glBufferData(
        GL_ARRAY_BUFFER,                // 데이터를 쓸 버퍼의 버퍼 타입
        numVerts * vertexSize,          // 버텍스의 데이터 크기
        verts,                          // 복사할 소스(포인터)
        GL_STATIC_DRAW                  // 이 데이터를 어떻게 사용할 것인가?
        );  //STATIC_DRAW: 데이터를 오직 한 번만 로드하며 버텍스가 자주 그려지는 경우에 사용되는 옵션
    

    // *********** Index Buffer 생성*************************
    // 이번에는 인덱스 버퍼를 생성한다.
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,            // 인덱스 버퍼
        numIndices * sizeof(unsigned int),  // 데이터의 크기
        indices,                            // 복사할 소스(포인터)
        GL_STATIC_DRAW);                    // 이 데이터를 어떻게 사용할 것인가?

    if (layout == PosNormTex)
    {
        // *********** 버텍스 레이아웃(속성) 설정 *************************
            // 첫 번째 버텍스 속성(속성0) 을 활성화. 
            // 속성0: 버텍스는 3차원 좌표값(x,y,z)을 가진다.
            // 다른 속성은 다음에 더 다룸.
        glEnableVertexAttribArray(0);

        // 버텍스의 크기와 타입, 속성의 포맷을 지정
        glVertexAttribPointer(
            0,                  // 속성 인덱스 (첫 번째 버텍스 속성[0]은 3차원 좌표 버텍스)
            3,                  // 요소의 수 (이번 예에서는 3 - x, y, z)
            GL_FLOAT,           // 요소의 타입
            GL_FALSE,           // (정수형 타입에서만 사용된다.)
            vertexSize,  // 간격. 한 버텍스의 시작점 ~ 다음 버텍스의 시작점 까지의 거리. 패딩값을 주지 않는다면 버텍스 사이즈를 간격으로 설정하면 된다.
            0                   // 버텍스의 시작에서 이 속성까지의 오프셋
        );

        // 두 번째 버텍스 속성(속성1)을 활성화
        // 속성1: 버텍스는 3차원 법선 벡터(x,y,z)를 가진다.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,          // 버텍스 속성 인덱스
            3,          // 요소의 수 법선벡터 (x,y,z) 2개의 컴포넌트 존재
            GL_FLOAT,   // 요소의 타입
            GL_FALSE,   // GL_FLOAT에서는 사용되지 않음
            vertexSize,  // 간격(간격은 항상 버텍스의 크기다.)
            reinterpret_cast<void*> (sizeof(float) * 3) // 오프셋 포인터
        );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,          // 버텍스 속성 인덱스
            2,          // 요소의 수 텍스쳐 좌표 (u,v) 2개의 컴포넌트 존재
            GL_FLOAT,   // 요소의 타입
            GL_FALSE,   // GL_FLOAT에서는 사용되지 않음
            vertexSize,  // 간격(간격은 항상 버텍스의 크기다.)
            reinterpret_cast<void*> (sizeof(float) * 6) // 오프셋 포인터
        );
    }
    else if (layout == PosNormSkinTex)
    {
        // *********** 버텍스 레이아웃(속성) 설정 *************************
            // 첫 번째 버텍스 속성(속성0) 을 활성화. 
            // 속성0: 버텍스는 3차원 좌표값(x,y,z)을 가진다.
            // 다른 속성은 다음에 더 다룸.
        glEnableVertexAttribArray(0);

        // 버텍스의 크기와 타입, 속성의 포맷을 지정
        glVertexAttribPointer(
            0,                  // 속성 인덱스 (첫 번째 버텍스 속성[0]은 3차원 좌표 버텍스)
            3,                  // 요소의 수 (이번 예에서는 3 - x, y, z)
            GL_FLOAT,           // 요소의 타입
            GL_FALSE,           // (정수형 타입에서만 사용된다.)
            vertexSize,  // 간격. 한 버텍스의 시작점 ~ 다음 버텍스의 시작점 까지의 거리. 패딩값을 주지 않는다면 버텍스 사이즈를 간격으로 설정하면 된다.
            0                   // 버텍스의 시작에서 이 속성까지의 오프셋
        );

        // 두 번째 버텍스 속성(속성1)을 활성화
        // 속성1: 버텍스는 3차원 법선 벡터(x,y,z)를 가진다.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,          // 버텍스 속성 인덱스
            3,          // 요소의 수 법선벡터 (x,y,z) 2개의 컴포넌트 존재
            GL_FLOAT,   // 요소의 타입
            GL_FALSE,   // GL_FLOAT에서는 사용되지 않음
            vertexSize,  // 간격(간격은 항상 버텍스의 크기다.)
            reinterpret_cast<void*> (sizeof(float) * 3) // 오프셋 포인터
        );

        // 세 번째 버텍스 속성(속성2)을 활성화
        // 속성2: 스키닝 본 정보. 1바이트 크기의 부호없는 정수 4개를 가진다.
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(
            2,          // 버텍스 속성 인덱스
            4,          // 버텍스가 영향받는 4개의 본 정보
            GL_UNSIGNED_BYTE,   // 요소의 타입
            vertexSize,  // 간격(간격은 항상 버텍스의 크기다.)
            reinterpret_cast<void*> (sizeof(float) * 6) // 오프셋 포인터
        );

        // 네 번째 버텍스 속성(속성3)을 활성화
        // 속성3: 스키닝 가중치 정보. 1바이트 실수 4개를 가진다.
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3,          // 버텍스 속성 인덱스
            4,          // 버텍스가 영향받는 4개의 본 각각의 가중치 
            GL_UNSIGNED_BYTE,   // 요소의 타입
            GL_TRUE,   
            vertexSize,  // 간격(간격은 항상 버텍스의 크기다.)
            reinterpret_cast<void*> (sizeof(float) * 6 + sizeof(char) * 4) // 오프셋 포인터
        );

        // 다섯번째 버텍스 속성(속성4)을 활성화
        // 속성4: 스키닝 가중치 정보. 1바이트 정수 4개를 가진다.
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4,          // 버텍스 속성 인덱스
            2,          // 요소의 수 텍스쳐 좌표 (u,v) 2개의 컴포넌트 존재
            GL_FLOAT,   // 요소의 타입
            GL_FALSE,   // GL_FLOAT에서는 사용되지 않음
            vertexSize,  // 간격(간격은 항상 버텍스의 크기다.)
            reinterpret_cast<void*> (sizeof(float) * 6 + sizeof(char) * 8) // 오프셋 포인터
        );
    }
    
}

VertexArray::~VertexArray()
{
    // 버텍스 버퍼와 인덱스 버퍼, 버텍스 배열 객체 해제
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mIndexBuffer);
    glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
    // 현재 사용할 버텍스 배열을 지정한다.
    glBindVertexArray(mVertexArray);
}
