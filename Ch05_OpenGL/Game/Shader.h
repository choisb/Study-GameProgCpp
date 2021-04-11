#pragma once
#include <string>
#include <GL/glew.h>
class Shader
{
public:
    Shader() {}
    ~Shader() {}
    // �־��� �̸����� ���ؽ�/�����׸�Ʈ ���̴� �ε�
    bool Load(const std::string& vertName, 
              const std::string& fragName);
    // �� ���̴��� Ȱ��ȭ�� ���̴� ���α׷����� ����
    void SetActive();
    //Game::Shutdown()���� ���� �������� ȣ��
    void Unload();

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