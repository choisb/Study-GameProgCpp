#include "Shader.h"
#include "SDL/SDL.h"
#include <fstream>
#include <sstream>

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
    // 버텍스와 프래그먼트 셰이더 컴파일
    if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
        !CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
    {
        return false;
    }
    // 버텍스/프래그먼트 셰이더를 서로 연결하는
    // 셰이더 프로그램을 생성
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragShader);
    glLinkProgram(mShaderProgram);

    // 프로그램이 성공적으로 연결됐는지 검증
    if (!IsValidProgram())
    {
        return false;
    }
    return true;
}
void Shader::SetActive()
{
    // 셰이더 프로그램을 활성화 시킨다.
    glUseProgram(mShaderProgram);
}
// 셰이더 프로그램과 버텍스 셰이더, 그리고 프래그먼트 셰이더를 삭제한다.
void Shader::Unload()
{
    glDeleteProgram(mShaderProgram);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragShader);
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
    // 파일을 연다
    std::ifstream shaderFile(fileName);
    if (shaderFile.is_open())
    {
        // 모든 텍스트를 읽어서 스트링으로 만든다
        std::stringstream sstream;
        sstream << shaderFile.rdbuf();
        std::string contents = sstream.str();
        const char* contentsChar = contents.c_str();

        // 지정된 타입의 셰이더를 생성한다.
        // 타입은 2가지가 들어올 수 있다. (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
        outShader = glCreateShader(shaderType);

        // outShader 셰이더 ID로 컴파일할 소스 코드 문자열 설정
        glShaderSource(outShader, 1, &(contentsChar), nullptr);
        // outShader 컴파일
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
    // 컴파일 상태를 질의한다. 컴파일 상태를 status에 반환한다.
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