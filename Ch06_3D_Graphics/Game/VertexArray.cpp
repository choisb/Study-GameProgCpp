#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts,
    const unsigned int* indices, unsigned int numIndices)
    : mNumVerts(numVerts)
    , mNumIndices(numIndices)
{
    // *********** Vertex Array ����*************************
    // vertex array ����. mVertexArray�� ID�� ����
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);


    // *********** Vertex Buffer ����*************************
    // vertex buffer ����. mVertexBuffer�� ID�� ����
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    // GL_ARRAY_BUFFER �Ķ���ʹ� ���ؽ� ���۸� ���۷� ����ϰڴٴ� ��

    // ���ؽ� ���۸� ���������� �����ڷ� ���޵� ���ؽ� �����͸� �� ���ؽ� ���۷� �����Ѵ�.
    // ù��° ���ڷ� ������ ID�� �ƴ� GL_ARRAY_BUFFER�� �����ߴ�.
    // GL_ARRAY_BUFFER�� ���� �ֱٿ� ������ ���ؽ� ���۸� ����ϰڴٴ� ���� ���Ѵ�.
    glBufferData(
        GL_ARRAY_BUFFER,                // �����͸� �� ������ ���� Ÿ��
        numVerts * 8 * sizeof(float),   // ������ ����Ʈ ũ�� (���� �� * 8 ��ġ (x,y,z) / ����(x,y,z) / �ؽ�ó(u,v) * 4byte)
        verts,                          // ������ �ҽ�(������)
        GL_STATIC_DRAW                  // �� �����͸� ��� ����� ���ΰ�?
        );  //STATIC_DRAW: �����͸� ���� �� ���� �ε��ϸ� ���ؽ��� ���� �׷����� ��쿡 ���Ǵ� �ɼ�
    

    // *********** Index Buffer ����*************************
    // �̹����� �ε��� ���۸� �����Ѵ�.
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,            // �ε��� ����
        numIndices * sizeof(unsigned int),  // �������� ũ��
        indices,                            // ������ �ҽ�(������)
        GL_STATIC_DRAW);                    // �� �����͸� ��� ����� ���ΰ�?


    // *********** ���ؽ� ���̾ƿ�(�Ӽ�) ���� *************************
    // ù ��° ���ؽ� �Ӽ�(�Ӽ�0) �� Ȱ��ȭ. 
    // �Ӽ�0: ���ؽ��� 3���� ��ǥ��(x,y,z)�� ������.
    // �ٸ� �Ӽ��� ������ �� �ٷ�.
    glEnableVertexAttribArray(0);

    // ���ؽ��� ũ��� Ÿ��, �Ӽ��� ������ ����
    glVertexAttribPointer(
        0,                  // �Ӽ� �ε��� (ù ��° ���ؽ� �Ӽ�[0]�� 3���� ��ǥ ���ؽ�)
        3,                  // ����� �� (�̹� �������� 3 - x, y, z)
        GL_FLOAT,           // ����� Ÿ��
        GL_FALSE,           // (������ Ÿ�Կ����� ���ȴ�.)
        sizeof(float) * 8,  // ����. �� ���ؽ��� ������ ~ ���� ���ؽ��� ������ ������ �Ÿ�. �е����� ���� �ʴ´ٸ� ���ؽ� ����� �������� �����ϸ� �ȴ�.
        0                   // ���ؽ��� ���ۿ��� �� �Ӽ������� ������
    );

    // �� ��° ���ؽ� �Ӽ�(�Ӽ�1)�� Ȱ��ȭ
    // �Ӽ�1: ���ؽ��� 3���� ���� ����(x,y,z)�� ������.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,          // ���ؽ� �Ӽ� �ε���
        3,          // ����� �� �������� (x,y,z) 2���� ������Ʈ ����
        GL_FLOAT,   // ����� Ÿ��
        GL_FALSE,   // GL_FLOAT������ ������ ����
        sizeof(float) * 8,  // ����(������ �׻� ���ؽ��� ũ���.)
        reinterpret_cast<void*> (sizeof(float) * 3) // ������ ������
        );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,          // ���ؽ� �Ӽ� �ε���
        2,          // ����� �� �ؽ��� ��ǥ (u,v) 2���� ������Ʈ ����
        GL_FLOAT,   // ����� Ÿ��
        GL_FALSE,   // GL_FLOAT������ ������ ����
        sizeof(float) * 8,  // ����(������ �׻� ���ؽ��� ũ���.)
        reinterpret_cast<void*> (sizeof(float) * 6) // ������ ������
    );
}

VertexArray::~VertexArray()
{
    // ���ؽ� ���ۿ� �ε��� ����, ���ؽ� �迭 ��ü ����
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mIndexBuffer);
    glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
    // ���� ����� ���ؽ� �迭�� �����Ѵ�.
    glBindVertexArray(mVertexArray);
}
