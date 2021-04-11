#include "Shader.h"
#include "SDL/SDL.h"
#include <fstream>
#include <sstream>

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
    // ���ؽ��� �����׸�Ʈ ���̴� ������
    if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
        !CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
    {
        return false;
    }
    // ���ؽ�/�����׸�Ʈ ���̴��� ���� �����ϴ�
    // ���̴� ���α׷��� ����
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragShader);
    glLinkProgram(mShaderProgram);

    // ���α׷��� ���������� ����ƴ��� ����
    if (!IsValidProgram())
    {
        return false;
    }
    return true;
}
void Shader::SetActive()
{
    // ���̴� ���α׷��� Ȱ��ȭ ��Ų��.
    glUseProgram(mShaderProgram);
}
// ���̴� ���α׷��� ���ؽ� ���̴�, �׸��� �����׸�Ʈ ���̴��� �����Ѵ�.
void Shader::Unload()
{
    glDeleteProgram(mShaderProgram);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragShader);
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
    // ������ ����
    std::ifstream shaderFile(fileName);
    if (shaderFile.is_open())
    {
        // ��� �ؽ�Ʈ�� �о ��Ʈ������ �����
        std::stringstream sstream;
        sstream << shaderFile.rdbuf();
        std::string contents = sstream.str();
        const char* contentsChar = contents.c_str();

        // ������ Ÿ���� ���̴��� �����Ѵ�.
        // Ÿ���� 2������ ���� �� �ִ�. (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
        outShader = glCreateShader(shaderType);

        // outShader ���̴� ID�� �������� �ҽ� �ڵ� ���ڿ� ����
        glShaderSource(outShader, 1, &(contentsChar), nullptr);
        // outShader ������
        glCompileShader(outShader);

        if (!IsCompiled(outShader))
        {
            SDL_Log("Failed to compile shader %s", fileName.c_str());
            return false;
        }
    }
    else
    {
        SDL_Log("Shader file not found: %s", fileName.c_str());
        return false;
    }
    return true;
}

bool Shader::IsCompiled(GLuint shader)
{
    GLint status;
    // ������ ���¸� �����Ѵ�. ������ ���¸� status�� ��ȯ�Ѵ�.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetShaderInfoLog(shader, 511, nullptr, buffer);
        SDL_Log("GLSL Compile Failed:\n%s", buffer);
        return false;
    }
    return true;
}
bool Shader::IsValidProgram()
{
    GLint status;

    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
        SDL_Log("GLSL Link Status:\n%s", buffer);
        return false;
    }
    return true;
}