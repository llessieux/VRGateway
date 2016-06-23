////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Texture.h"
#include <string>
#include "WICTextureLoader.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device*device, const char*filename)
{
    if (filename == nullptr)
        return false;

    std::string src(filename);
    std::wstring dst;
    for (auto c : src)
        dst.push_back(c);

    return Initialize(device, dst.c_str());
}

bool Texture::Initialize(ID3D11Device* device, const WCHAR* filename)
{
    if (filename == nullptr)
        return false;

    HRESULT result;
    std::wstring wsFileName(filename);

    result = DirectX::CreateWICTextureFromFile(device, wsFileName.c_str(), nullptr, &m_texture);
    if (FAILED(result))
    {
        return false;
    }
    return true;
}


void Texture::Shutdown()
{
    // Release the texture resource.
    if(m_texture != nullptr)
    {
        m_texture.Release();
    }
}

void Texture::SetTexture(ID3D11ShaderResourceView* texture)
{
    Shutdown();
    m_texture = texture;

}

ID3D11ShaderResourceView* Texture::GetTexture()
{
    return m_texture;
}