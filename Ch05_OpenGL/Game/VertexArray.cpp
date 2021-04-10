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
        numVerts * 3 * sizeof(float),   // ������ ����Ʈ ũ�� (���� �� * 3(x,y,z) * 4)
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
        sizeof(float) * 3,  // ����. �� ���ؽ��� ������ ~ ���� ���ؽ��� ������ ������ �Ÿ�. �е����� ���� �ʴ´ٸ� ���ؽ� ����� �������� �����ϸ� �ȴ�.
        0                   // ���ؽ��� ���ۿ��� �� �Ӽ������� ������
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
