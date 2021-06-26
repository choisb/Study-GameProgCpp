#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const void* verts, unsigned int numVerts, 
    Layout layout, const unsigned int* indices, unsigned int numIndices)
    : mNumVerts(numVerts)
    , mNumIndices(numIndices)
{
    // *********** Vertex Array ����*************************
    // vertex array ����. mVertexArray�� ID�� ����
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    unsigned vertexSize = 8 * sizeof(float);
    if (layout == PosNormSkinTex)
    {
        vertexSize = 8 * sizeof(float) + 8 * sizeof(char);
    }

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
        numVerts * vertexSize,          // ���ؽ��� ������ ũ��
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

    if (layout == PosNormTex)
    {
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
            vertexSize,  // ����. �� ���ؽ��� ������ ~ ���� ���ؽ��� ������ ������ �Ÿ�. �е����� ���� �ʴ´ٸ� ���ؽ� ����� �������� �����ϸ� �ȴ�.
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
            vertexSize,  // ����(������ �׻� ���ؽ��� ũ���.)
            reinterpret_cast<void*> (sizeof(float) * 3) // ������ ������
        );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,          // ���ؽ� �Ӽ� �ε���
            2,          // ����� �� �ؽ��� ��ǥ (u,v) 2���� ������Ʈ ����
            GL_FLOAT,   // ����� Ÿ��
            GL_FALSE,   // GL_FLOAT������ ������ ����
            vertexSize,  // ����(������ �׻� ���ؽ��� ũ���.)
            reinterpret_cast<void*> (sizeof(float) * 6) // ������ ������
        );
    }
    else if (layout == PosNormSkinTex)
    {
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
            vertexSize,  // ����. �� ���ؽ��� ������ ~ ���� ���ؽ��� ������ ������ �Ÿ�. �е����� ���� �ʴ´ٸ� ���ؽ� ����� �������� �����ϸ� �ȴ�.
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
            vertexSize,  // ����(������ �׻� ���ؽ��� ũ���.)
            reinterpret_cast<void*> (sizeof(float) * 3) // ������ ������
        );

        // �� ��° ���ؽ� �Ӽ�(�Ӽ�2)�� Ȱ��ȭ
        // �Ӽ�2: ��Ű�� �� ����. 1����Ʈ ũ���� ��ȣ���� ���� 4���� ������.
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(
            2,          // ���ؽ� �Ӽ� �ε���
            4,          // ���ؽ��� ����޴� 4���� �� ����
            GL_UNSIGNED_BYTE,   // ����� Ÿ��
            vertexSize,  // ����(������ �׻� ���ؽ��� ũ���.)
            reinterpret_cast<void*> (sizeof(float) * 6) // ������ ������
        );

        // �� ��° ���ؽ� �Ӽ�(�Ӽ�3)�� Ȱ��ȭ
        // �Ӽ�3: ��Ű�� ����ġ ����. 1����Ʈ �Ǽ� 4���� ������.
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3,          // ���ؽ� �Ӽ� �ε���
            4,          // ���ؽ��� ����޴� 4���� �� ������ ����ġ 
            GL_UNSIGNED_BYTE,   // ����� Ÿ��
            GL_TRUE,   
            vertexSize,  // ����(������ �׻� ���ؽ��� ũ���.)
            reinterpret_cast<void*> (sizeof(float) * 6 + sizeof(char) * 4) // ������ ������
        );

        // �ټ���° ���ؽ� �Ӽ�(�Ӽ�4)�� Ȱ��ȭ
        // �Ӽ�4: ��Ű�� ����ġ ����. 1����Ʈ ���� 4���� ������.
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4,          // ���ؽ� �Ӽ� �ε���
            2,          // ����� �� �ؽ��� ��ǥ (u,v) 2���� ������Ʈ ����
            GL_FLOAT,   // ����� Ÿ��
            GL_FALSE,   // GL_FLOAT������ ������ ����
            vertexSize,  // ����(������ �׻� ���ؽ��� ũ���.)
            reinterpret_cast<void*> (sizeof(float) * 6 + sizeof(char) * 8) // ������ ������
        );
    }
    
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
