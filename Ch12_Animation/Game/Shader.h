#pragma once
#include <string>
#include <GL/glew.h>
#include "Math.h"
class Shader
{
public:
    Shader();
    ~Shader();
    // �־��� �̸����� ���ؽ�/�����׸�Ʈ ���̴� �ε�
    bool Load(const std::string& vertName, 
              const std::string& fragName);
    // �� ���̴��� Ȱ��ȭ�� ���̴� ���α׷����� ����
    void SetActive();
    //Game::Shutdown()���� ���� �������� ȣ��
    void Unload();
    // ���̴��� Matrix uniform ���� �Լ�
    // - name: ���̴� ���� ���� �����ϴ� uniform ���� �̸�
    // - matrix: �Է��ϰ��� �ϴ� ��
    void SetMatrixUniform(const char* name, const Matrix4& matrix);

    // ���̴��� Matrix (�迭) uniform ���� �Լ�
    // - name: ���̴� ���� ���� �����ϴ� uniform ���� �̸�
    // - matrices: �Է��ϰ��� �ϴ� �迭�� ���� �ּҰ�
    // - count: �迭�� ����
    void SetMatrixUniform(const char* name, const Matrix4* matrices, unsigned count);

    // ���̴��� vec3 uniform ���� �Լ�
    // - name: ���̴� ���� ���� �����ϴ� uniform ���� �̸�
    // - Vector3: �Է��ϰ��� �ϴ� Vector3��
    void SetVectorUniform(const char* name, const Vector3& vector);

    // ���̴��� float uniform ���� �Լ�
    // - name: ���̴� ���� ���� �����ϴ� uniform ���� �̸�
    // - float: �Է��ϰ��� �ϴ� Vector3��
    void SetFloatUniform(const char* name, float value);

private:
    // ������ ���̴��� ������
    bool CompileShader(const std::string& fileName, 
                       GLenum shaderType, 
                       GLuint& outShader);
    // ���̴��� ���������� �����ϵƴ��� Ȯ��
    bool IsCompiled(GLuint shader);
    // ���ؽ�/�����׸�Ʈ ���α׷��� ����ƴ��� Ȯ��
    bool IsValidProgram();
    // ���̴� ������Ʈ ID�� ����
    GLuint mVertexShader;
    GLuint mFragShader;
    GLuint mShaderProgram;
};