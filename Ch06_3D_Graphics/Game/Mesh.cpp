#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "Math.h"
#include "Renderer.h"

Mesh::Mesh()
    :mVertexArray(nullptr)
    ,mRadius(0.0f)
{

}
Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string & fileName, Renderer* game)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("File not found: Mesh %s", fileName.c_str());
        return false;
    }

    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string contents = fileStream.str();
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);

    if (!doc.IsObject())
    {
        SDL_Log("Mesh %s is not valid json", fileName.c_str());
        return false;
    }

    int ver = doc["version"].GetInt();

    // 버전 확인
    if (ver != 1)
    {
        SDL_Log("Mesh %s not version 1", fileName.c_str());
        return false;
    }

    mShaderName = doc["shader"].GetString();

    // Skip the vertex format / shader for now
    // (This is changed in a later chapter's code)
    size_t vertSize = 8;

    // Load textures
    const rapidjson::Value& textures = doc["textures"];
    if (!textures.IsArray() || textures.Size() < 1)
    {
        SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
        return false;
    }

    //mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

    // 여기서 더 작성해야함.
}

void Mesh::Unload()
{
    delete mVertexArray;
    mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index)
{
    if (index < mTextures.size())
    {
        return mTextures[index];
    }
    else
    {
        return nullptr;
    }
}
